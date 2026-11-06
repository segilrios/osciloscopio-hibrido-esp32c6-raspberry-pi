#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include "esp_adc/adc_continuous.h"
#include "driver/spi_slave.h"

#define TAG "OSCILOSCOPIO"

#define OSC_MAX_MUESTRAS 1024
#define OSC_PAUSA_MS 500
#define ADC_CH1 ADC_CHANNEL_0
#define ADC_CH2 ADC_CHANNEL_1
#define ADC_FRAME_SIZE 256
#define ADC_BUFFER_SIZE 4096
#define ADC_READ_TIMEOUT 1000
#define COMANDO_COLA 8
#define COMANDO_STACK 3072
#define SPI_STACK 8192
#define SPI_PIN_MOSI 7
#define SPI_PIN_MISO 2
#define SPI_PIN_SCLK 6
#define SPI_PIN_CS 16
#define SPI_CMD_BYTES 16
#define SPI_HEADER_BYTES 20
#define SPI_MAX_RESPONSE (SPI_HEADER_BYTES + 4 * OSC_MAX_MUESTRAS)
#define SPI_CHUNK_BYTES 2048

static const uint32_t frecuencias[] = { 100, 1000, 10000, 40000 };
static const uint16_t tamanos[] = { 64, 256, 512, 1024 };

typedef struct {
    uint32_t frecuencia;
    uint32_t periodo_us;
    uint32_t frecuencia_adc;
    uint32_t decimacion;
    uint16_t muestras;
    uint32_t generation;
} osc_config_t;

typedef struct {
    uint32_t id;
    uint16_t ch1[OSC_MAX_MUESTRAS];
    uint16_t ch2[OSC_MAX_MUESTRAS];
    uint16_t n;
    uint32_t frecuencia;
    uint32_t n1;
    uint32_t n2;
    uint64_t tiempo_us;
    uint32_t errores;
} osc_ventana_t;

typedef struct {
    double frecuencia_real;
    double error_porcentual;
    bool cumple;
} osc_diagnostico_t;

static osc_config_t config;
static osc_ventana_t ventana_publicada;
static osc_ventana_t ventana_captura;
static osc_diagnostico_t diagnostico;
static SemaphoreHandle_t estado_mutex;
static adc_continuous_handle_t adc_handle;
static QueueHandle_t cola_comandos;
static uint32_t contador_ventanas;
static uint32_t ultima_frecuencia_impresa;
static bool adc_activo;
static volatile bool adc_error_reciente;
DMA_ATTR static uint8_t spi_cmd_rx[SPI_CMD_BYTES] __attribute__((aligned(4)));
DMA_ATTR static uint8_t spi_response_tx[SPI_MAX_RESPONSE] __attribute__((aligned(4)));

static bool preset_valido(uint32_t rate, uint16_t n)
{
    for (size_t i = 0; i < sizeof(frecuencias) / sizeof(frecuencias[0]); ++i) {
        if (frecuencias[i] == rate && tamanos[i] != 0) {
            for (size_t j = 0; j < sizeof(tamanos) / sizeof(tamanos[0]); ++j)
                if (tamanos[j] == n) return true;
        }
    }
    return false;
}

static void osc_configurar_indice(uint32_t indice)
{
    config.frecuencia = frecuencias[indice];
    if (config.muestras == 0) config.muestras = tamanos[0];
    config.periodo_us = 1000000 / config.frecuencia;
    config.frecuencia_adc = (config.frecuencia == 100) ? 1000 : config.frecuencia * 2;
    config.decimacion = config.frecuencia_adc / (2 * config.frecuencia);
    if (config.decimacion == 0) config.decimacion = 1;
    config.generation++;
    ventana_publicada.n = 0;
}

static bool copiar_config(osc_config_t *dst)
{
    bool ok = xSemaphoreTake(estado_mutex, portMAX_DELAY) == pdTRUE;
    if (ok) { *dst = config; xSemaphoreGive(estado_mutex); }
    return ok;
}

static void osc_adc_inicializar(void)
{
    adc_continuous_handle_cfg_t cfg = {
        .max_store_buf_size = ADC_BUFFER_SIZE,
        .conv_frame_size = ADC_FRAME_SIZE
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&cfg, &adc_handle));
    ESP_LOGI(TAG, "ADC continuo DMA inicializado (GPIO0/GPIO1)");
}

static void osc_adc_configurar(const osc_config_t *cfg)
{
    if (adc_activo) {
        ESP_ERROR_CHECK(adc_continuous_stop(adc_handle));
        adc_activo = false;
    }
    ESP_ERROR_CHECK(adc_continuous_flush_pool(adc_handle));
    adc_digi_pattern_config_t patron[2] = {
        { .atten = ADC_ATTEN_DB_12, .channel = ADC_CH1, .unit = ADC_UNIT_1, .bit_width = ADC_BITWIDTH_12 },
        { .atten = ADC_ATTEN_DB_12, .channel = ADC_CH2, .unit = ADC_UNIT_1, .bit_width = ADC_BITWIDTH_12 }
    };
    adc_continuous_config_t adc_cfg = {
        .pattern_num = 2, .adc_pattern = patron, .sample_freq_hz = cfg->frecuencia_adc,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1, .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2
    };
    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &adc_cfg));
}

static bool osc_capturar_ventana(const osc_config_t *cfg, osc_ventana_t *out)
{
    adc_continuous_data_t datos[64];
    uint32_t c1 = 0, c2 = 0;
    memset(out, 0, sizeof(*out));
    out->n = cfg->muestras;
    out->frecuencia = cfg->frecuencia;
    out->id = cfg->generation;
    osc_adc_configurar(cfg);
    int64_t inicio = esp_timer_get_time();
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));
    adc_activo = true;
    while (out->n1 < cfg->muestras || out->n2 < cfg->muestras) {
        uint32_t cantidad = 0;
        esp_err_t err = adc_continuous_read_parse(adc_handle, datos, 64, &cantidad, ADC_READ_TIMEOUT);
        if (err != ESP_OK) {
            out->errores++;
            if (out->errores >= 5) break;
            continue;
        }
        for (uint32_t i = 0; i < cantidad; ++i) {
            if (!datos[i].valid) { out->errores++; continue; }
            if (datos[i].channel == ADC_CH1 && out->n1 < cfg->muestras && c1++ % cfg->decimacion == 0)
                out->ch1[out->n1++] = (uint16_t)datos[i].raw_data;
            else if (datos[i].channel == ADC_CH2 && out->n2 < cfg->muestras && c2++ % cfg->decimacion == 0)
                out->ch2[out->n2++] = (uint16_t)datos[i].raw_data;
        }
    }
    out->tiempo_us = (uint64_t)(esp_timer_get_time() - inicio);
    ESP_ERROR_CHECK(adc_continuous_stop(adc_handle));
    adc_activo = false;
    return out->n1 == cfg->muestras && out->n2 == cfg->muestras && out->errores == 0;
}

static void publicar_ventana(const osc_ventana_t *captura)
{
    if (xSemaphoreTake(estado_mutex, portMAX_DELAY) == pdTRUE) {
        ventana_publicada = *captura;
        ventana_publicada.id = ++contador_ventanas;
        xSemaphoreGive(estado_mutex);
    }
}

static uint16_t crc16_ccitt(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xffff;
    while (len--) {
        crc ^= (uint16_t)*data++ << 8;
        for (int bit = 0; bit < 8; ++bit)
            crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
    }
    return crc;
}

static void put_u16(uint8_t *p, uint16_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void put_u32(uint8_t *p, uint32_t v) { for (int i = 0; i < 4; ++i) p[i] = (uint8_t)(v >> (8 * i)); }
static uint16_t get_u16(const uint8_t *p) { return (uint16_t)p[0] | (uint16_t)p[1] << 8; }
static uint32_t get_u32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }

static size_t preparar_respuesta(const uint8_t *cmd, uint8_t *tx, uint32_t *last_sent)
{
    uint8_t status = 0;
    memset(tx, 0, SPI_MAX_RESPONSE);
    uint16_t n = 0;
    uint32_t rate = 0, seq = 0;
    uint8_t opcode = cmd[5];
    const bool get_window = (opcode == 2);
    if (memcmp(cmd, "PSC0", 4) != 0 || cmd[4] != 1 || get_u32(cmd + 12) != 0) status = 2;
    if (opcode == 1) {
        uint16_t requested_n = get_u16(cmd + 6); uint32_t requested_rate = get_u32(cmd + 8);
        if (!preset_valido(requested_rate, requested_n)) status = 2;
        else if (xSemaphoreTake(estado_mutex, portMAX_DELAY) == pdTRUE) {
            config.frecuencia = requested_rate; config.muestras = requested_n;
            config.periodo_us = 1000000 / requested_rate;
            config.frecuencia_adc = (requested_rate == 100) ? 1000 : requested_rate * 2;
            config.decimacion = config.frecuencia_adc / (2 * requested_rate);
            if (!config.decimacion) config.decimacion = 1;
            config.generation++;
            ventana_publicada.n = 0;
            xSemaphoreGive(estado_mutex);
        }
    } else if (opcode != 2 && opcode != 3) status = 2;
    if (xSemaphoreTake(estado_mutex, portMAX_DELAY) == pdTRUE) {
        n = ventana_publicada.n ? ventana_publicada.n : config.muestras;
        rate = ventana_publicada.n ? ventana_publicada.frecuencia : config.frecuencia;
        if (opcode != 2) { n = config.muestras; rate = config.frecuencia; }
        seq = ventana_publicada.id;
        if (get_window && (!ventana_publicada.n || seq <= *last_sent)) status = status ? status : (adc_error_reciente ? 3 : 1);
        if (get_window && ventana_publicada.n && status == 0)
            for (uint16_t i = 0; i < n; ++i) { put_u16(tx + 20 + i * 4, ventana_publicada.ch1[i]); put_u16(tx + 22 + i * 4, ventana_publicada.ch2[i]); }
        if (get_window && status != 0)
            memset(tx + SPI_HEADER_BYTES, 0, (size_t)n * 4);
        xSemaphoreGive(estado_mutex);
    }
    memset(tx, 0, 20);
    memcpy(tx, "PSC1", 4); tx[4] = 1; tx[5] = status;
    put_u16(tx + 6, n); put_u32(tx + 8, rate); put_u32(tx + 12, seq);
    uint16_t crc = crc16_ccitt(tx, 16);
    if (get_window) {
        for (size_t i = 0; i < (size_t)n * 4; ++i) {
            crc ^= (uint16_t)tx[20 + i] << 8;
            for (int b = 0; b < 8; ++b)
                crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
        }
    }
    put_u16(tx + 16, crc);
    /* GET_WINDOW always has the configured payload length, including errors;
       the zero-filled payload is part of the CRC and is still clocked. */
    return SPI_HEADER_BYTES + (get_window ? (size_t)n * 4 : 0);
}

static void osc_tarea_spi(void *param)
{
    uint32_t last_sent = 0;
    spi_slave_transaction_t t;
    while (true) {
        memset(&t, 0, sizeof(t)); memset(spi_cmd_rx, 0, sizeof(spi_cmd_rx));
        t.length = SPI_CMD_BYTES * 8;
        t.rx_buffer = spi_cmd_rx;
        esp_err_t err = spi_slave_transmit(SPI2_HOST, &t, portMAX_DELAY);
        if (err != ESP_OK) { ESP_LOGW(TAG, "SPI comando: %s", esp_err_to_name(err)); continue; }
        if (t.trans_len != SPI_CMD_BYTES * 8) {
            ESP_LOGW(TAG, "SPI comando corto: %zu bits", t.trans_len);
            continue;
        }
        size_t bytes = preparar_respuesta(spi_cmd_rx, spi_response_tx, &last_sent);
        bool response_ok = true;
        /* For N=512/1024 the host must clock these fixed chunks as separate
           CS assertions: 2048 bytes, then the remaining response bytes. */
        for (size_t offset = 0; offset < bytes; ) {
            size_t chunk = bytes - offset;
            if (chunk > SPI_CHUNK_BYTES) chunk = SPI_CHUNK_BYTES;
            memset(&t, 0, sizeof(t));
            t.length = chunk * 8;
            t.tx_buffer = spi_response_tx + offset;
            err = spi_slave_transmit(SPI2_HOST, &t, portMAX_DELAY);
            if (err != ESP_OK || t.trans_len != chunk * 8) {
                ESP_LOGW(TAG, "SPI respuesta incompleta: %s, %zu/%u bits",
                         esp_err_to_name(err), t.trans_len, (unsigned)(chunk * 8));
                response_ok = false;
                break;
            }
            offset += chunk;
        }
        if (response_ok && spi_cmd_rx[5] == 2 && spi_response_tx[5] == 0)
            last_sent = get_u32(spi_response_tx + 12);
    }
}

static void osc_spi_inicializar(void)
{
    spi_bus_config_t bus = { .mosi_io_num = SPI_PIN_MOSI, .miso_io_num = SPI_PIN_MISO, .sclk_io_num = SPI_PIN_SCLK, .quadwp_io_num = -1, .quadhd_io_num = -1 };
    spi_slave_interface_config_t slave = { .spics_io_num = SPI_PIN_CS, .flags = 0, .queue_size = 1, .mode = 0 };
    ESP_ERROR_CHECK(spi_slave_initialize(SPI2_HOST, &bus, &slave, SPI_DMA_CH_AUTO));
    ESP_LOGI(TAG, "SPI2 esclavo PSC1 MOSI=%d MISO=%d SCLK=%d CS=%d", SPI_PIN_MOSI, SPI_PIN_MISO, SPI_PIN_SCLK, SPI_PIN_CS);
}

static void osc_tarea_comandos(void *param)
{
    while (1) {
        int tecla = getchar();
        if (tecla == EOF) { clearerr(stdin); vTaskDelay(pdMS_TO_TICKS(10)); continue; }
        uint8_t comando = (uint8_t)tecla;
        if (comando >= '1' && comando <= '4') xQueueSend(cola_comandos, &comando, 0);
    }
}

static uint32_t osc_a_milivoltios(uint16_t raw) { int32_t mv = ((int32_t)raw - 32) * 1000 / 1024; return (uint32_t)(mv < 0 ? 0 : mv); }

void app_main(void)
{
    ESP_LOGI(TAG, "OSCILOSCOPIO DIGITAL ESP32-C6");
    estado_mutex = xSemaphoreCreateMutex();
    if (!estado_mutex) { ESP_LOGE(TAG, "Error creando mutex"); return; }
    osc_adc_inicializar();
    cola_comandos = xQueueCreate(COMANDO_COLA, sizeof(uint8_t));
    if (!cola_comandos) { ESP_LOGE(TAG, "Error creando cola"); return; }
    if (xSemaphoreTake(estado_mutex, portMAX_DELAY) == pdTRUE) { osc_configurar_indice(0); xSemaphoreGive(estado_mutex); }
    osc_spi_inicializar();
    if (xTaskCreate(osc_tarea_spi, "spi_psc1", SPI_STACK, NULL, 6, NULL) != pdPASS) { ESP_LOGE(TAG, "Error creando tarea SPI"); return; }
    if (xTaskCreate(osc_tarea_comandos, "comandos", COMANDO_STACK, NULL, 5, NULL) != pdPASS) { ESP_LOGE(TAG, "Error creando tarea USB"); return; }
    while (1) {
        uint8_t comando; while (xQueueReceive(cola_comandos, &comando, 0) == pdTRUE) {
            if (xSemaphoreTake(estado_mutex, portMAX_DELAY) == pdTRUE) { osc_configurar_indice(comando - '1'); xSemaphoreGive(estado_mutex); }
        }
        osc_config_t cfg; copiar_config(&cfg);
        bool completa = osc_capturar_ventana(&cfg, &ventana_captura);
        bool config_sigue_vigente = false;
        if (xSemaphoreTake(estado_mutex, portMAX_DELAY) == pdTRUE) {
            config_sigue_vigente = config.generation == cfg.generation &&
                                   config.frecuencia == cfg.frecuencia &&
                                   config.muestras == cfg.muestras;
            xSemaphoreGive(estado_mutex);
        }
        adc_error_reciente = !completa;
        if (completa && config_sigue_vigente) {
            publicar_ventana(&ventana_captura);
            diagnostico.frecuencia_real = ventana_captura.tiempo_us ? (double)ventana_captura.n1 * 1000000.0 / ventana_captura.tiempo_us : 0;
            diagnostico.error_porcentual = fabs(diagnostico.frecuencia_real - cfg.frecuencia) * 100.0 / cfg.frecuencia;
            diagnostico.cumple = diagnostico.error_porcentual <= 5.0;
            ESP_LOGI(TAG, "Ventana #%" PRIu32 " (%" PRIu32 " Sa/s, N=%u)",
                     ventana_publicada.id, cfg.frecuencia, cfg.muestras);
        } else {
            ESP_LOGW(TAG, "Captura ADC incompleta; no se publica ventana");
        }
        vTaskDelay(pdMS_TO_TICKS(OSC_PAUSA_MS));
    }
}
