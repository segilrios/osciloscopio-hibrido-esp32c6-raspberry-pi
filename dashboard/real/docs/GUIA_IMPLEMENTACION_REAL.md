# Integración física PulseScope: ESP32 + Raspberry Pi Zero 2 W

**Estado:** código de referencia de integración **listo para llevar al laboratorio**, pero todavía **SIN validar en vuestro hardware**. El objetivo es ESP32-C6; no debe afirmarse que los pines propuestos estén disponibles ni que 40.000 Sa/s estén logrados sin pruebas físicas.

## Qué se conserva

Se mantiene el frontend original `web/`, el backend `server.py`, `PRUEBA_SIN_RASPBERRY.bat`, `demo_integracion.py` y el emulador. Se agrega **40.000 Sa/s (experimental)** al selector/validador y un indicador de enlace SPI reportado (no es prueba de exactitud temporal). La API HTTP/JSON original **no cambia**: las ventanas entran a `POST /api/ingest` y las gráficas consultan `GET /api/muestras`.

## 0. Antes de cablear: identificar la placa ESP32-C6

La hoja de datos del chip no permite deducir el pinout de una placa montada. El firmware C6 activo define SPI como MOSI=GPIO7, MISO=GPIO2, SCLK=GPIO6 y CS=GPIO16; son conexiones **condicionadas a que estén libres y expuestas en la placa concreta** y deben contrastarse con su esquemático y periféricos.

**No conectar hasta comprobar:** modelo comercial de la placa ESP32-C6, esquemático, disponibilidad de dos entradas ADC, cuatro señales SPI (MOSI/MISO/SCLK/CS), alimentación y masa compartida. **No se usa GPIO READY.** **No aplicar 5 V ni voltaje negativo a los GPIO**. Las señales del generador deben quedar dentro del rango admitido por el ADC y, si hace falta, pasar por un acondicionamiento adecuado. La referencia 3,3 V del backend es una conversión provisional, no calibración.

### Cableado propuesto, a CONVALIDAR con esquemático de la placa

| Raspberry Pi Zero 2 W, **BCM** | Pin físico de 40 pines | Señal | ESP32 **EJEMPLO**, adaptar |
|---|---:|---|---|
| GPIO10 | 19 | MOSI | GPIO7 (C6; verificar disponibilidad) |
| GPIO9 | 21 | MISO | GPIO2 (C6; verificar disponibilidad) |
| GPIO11 | 23 | SCLK | GPIO6 (C6; verificar disponibilidad) |
| GPIO8 | 24 | CE0/CS | GPIO16 (C6; verificar disponibilidad) |
| GND | 6 (o cualquier GND) | Masa compartida | GND |

Tensiones lógicas **3,3 V**. Conectar correctamente GND. No unir las alimentaciones 5V ni 3V3 de ambos equipos sin un diseño de potencia. Usar cables cortos, verificar orden MOSI/MISO/SCLK/CS y no confundir **BCM** con número físico.

## 1. Cargar firmware en el ESP32-C6 (integrante firmware)

Requiere **ESP-IDF 5.5** y herramientas ESP-IDF operativas en el **PC de desarrollo**, no en la Raspberry Pi. El firmware compatible está copiado en `../../../firmware/` (ruta relativa desde este documento); ejecuta allí:

```bash
cd ../../../firmware
idf.py set-target esp32c6
idf.py build
idf.py -p PUERTO_DEL_ESP32 flash monitor
```

Ajusta los GPIO SPI y canales ADC del firmware C6 al esquemático de la placa antes de compilar. En la Raspberry solo se instala/copia el dashboard (`/opt/dashboard_osciloscopio`) y se ejecuta el lector SPI; no es necesario copiar allí el checkout del firmware ni compilarlo allí. **No flashees `real/esp32_idf` de este dashboard en un ESP32-C6**: es una referencia clásica incompatible, usa otro objetivo/pinout y no forma parte de esta integración.

El `main.c` C6 activo configura ADC continuo DMA con dos canales alternados y `frecuencia_adc = 1000` para el preset de 100 Sa/s, o `2 * frecuencia` para 1000/10000/40000 Sa/s (1000, 2000, 20000 y 80000 conversiones/s agregadas, respectivamente). La decimación se calcula para entregar la frecuencia solicitada por canal. Captura cada ventana bloqueando hasta reunir N muestras válidas por canal; no usa un buffer circular para publicar ventanas parciales.

La fuente activa no debe describirse como si tuviera un registro `METRIC` periódico: la guía no promete logging cada 5 s ni una tasa temporal validada. La uniformidad temporal, duración de captura y rendimiento deben medirse en el hardware real.

## 2. Preparar Raspberry Pi (integrante SBC)

Usar Raspberry Pi OS con acceso a red y terminal; necesitarás `sudo`. Habilitar SPI mediante:

```bash
sudo raspi-config
# Interface Options -> SPI -> Enable
sudo reboot
ls /dev/spidev0.0
```

Entra en la carpeta `dashboard_osciloscopio` que se copió a la Raspberry; instalar con:

```bash
sudo bash real/deploy/preparar_servicios.sh
```

El script instala Nginx y `python3-spidev`, copia el proyecto a `/opt/dashboard_osciloscopio`, habilita el backend Python **modo external** solo en localhost, y publica los archivos de `web/` con **Nginx en el puerto 80**. **No arranca SPI automáticamente** hasta que se verifiquen conexiones.

Si se usan rutas o usuarios diferentes, adaptar los archivos de `real/deploy/`. El script asume nombres de paquetes disponibles en la versión de Raspberry Pi OS. Se usa SPI0 CE0. El servicio ejecuta Python del sistema, por lo que hay que instalar `python3-spidev` en ese entorno.

## 3. Probar SPI físicamente ANTES de activar el dashboard real

Con la placa ESP32-C6 conectada y el firmware del repositorio hermano cargado:

```bash
cd /opt/dashboard_osciloscopio/real/raspberry
python3 probar_spi_fisico.py --rate 1000 --window 512 --spi-hz 2000000
```

Debe responder `PING`, luego `ACK config` (incluye la tasa y ventana solicitadas) y diez ventanas de muestras con su conteo de bytes SPI. Si falla, revisar que GPIO7/2/6/16 estén realmente disponibles y cableados como MOSI/MISO/SCLK/CS, además de alimentación, masa y firmware C6. Si falla CRC, bajar reloj a 1 MHz, aumentar la pausa entre CS, acortar cables y revisar pinout. El firmware está configurado con **SPI modo 0**. La pausa y el timing físico aún no están verificados en laboratorio.

Las respuestas grandes se dividen en transacciones CS de como máximo 2048 bytes (N=512: 2048+20; N=1024: 2048+2048+20), por lo que no se depende de un xfer único de 4.116 bytes. La pausa conservadora por defecto es 15 ms y debe validarse físicamente.

## 4. Activar adquisición y publicación real

```bash
sudo systemctl enable --now pulsescope-spi.service
sudo systemctl status pulsescope-spi.service
sudo journalctl -u pulsescope-spi -f
```

Después abrir `http://IP_REAL_RASPBERRY/` desde un computador o celular **de la misma red**. En el dashboard, cambiar 100, 1.000, 10.000 y (experimentalmente) 40.000 Sa/s. La secuencia debe ser: navegador `POST /api/config` → adaptador Raspberry `GET /api/config` → comando SPI → **ACK ESP32** → lectura SPI con CRC → adaptador `POST /api/ingest` → navegador `GET /api/muestras`.

El backend siempre devuelve la configuración *solicitada*. El estado `spi_connected` y `applied_rate_hz` de `GET /api/estado` solo se activan cuando el adaptador declara haber obtenido ACK y recibido ventanas. **Ese estado NO certifica la tasa real, que requiere medición externa**.

Por seguridad Nginx bloquea `/api/ingest` y `/api/hardware` desde navegadores remotos. Los dos POST deben dirigirse por `127.0.0.1:8000`, **desde el SBC**, no mediante la IP pública de la Pi. Mantener red local confiable; no exponer HTTP a Internet sin autenticación y seguridad adicionales.

## 5. Pruebas previas SIN ningún equipo

En Windows, exactamente como antes:

```powershell
python demo_integracion.py
# http://127.0.0.1:8000
python -m unittest discover -s tests -v
python -m unittest discover -s real/tests -v
```

También está `PRUEBA_SIN_RASPBERRY.bat`: se conservó, igual que el generador artificial. Con el preset de 40.000 Sa/s el emulador solo configura el metadato de tiempo; **no está reproduciendo un ADC a 40 kHz**.

## 6. Escenarios que falta validar en laboratorio

- Compilación y ejecución en la **placa ESP32-C6 exacta**, con periféricos de la placa sin conflicto.
- Fs 100, 1000, 10000 y 40000 **por canal**; estabilidad, muestras perdidas, jitter y corrección de escala de voltajes.
- SPI con CS, chunks de 2048 bytes, longitud de ventana, CRC, pausa y tasa máxima experimental a varios clocks.
- Calibración del ADC y acondicionamiento de señales; amplitud/frecuencia reales con generador y osciloscopio.
- Latencia y actualización visual dentro de 1-4 s en ventanas que físicamente puedan completarse en ese intervalo. En particular, N=1024 a 100 Sa/s requiere al menos 10,24 s de adquisición por canal; no puede producir una ventana nueva en cada refresco de 1-4 s y es esperado recibir `NO_DATA` mientras se completa.
- Nginx accesible por red, servicios tras reboot, permisos GPIO/SPI, escritura de logs y seguridad.

**Si la placa ESP32-C6 no expone dos entradas ADC o los GPIO SPI previstos, hay que adaptar el hardware. Ningún ZIP puede garantizar el funcionamiento eléctrico antes de identificar la placa. No usar GPIO34/35 ni GPIO27 como supuesto pinout del C6; son referencias del firmware clásico y no deben cablearse por inferencia.**
