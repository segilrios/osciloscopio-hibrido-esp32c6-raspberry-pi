# Instalación y verificación reproducible

Estas instrucciones trabajan con las copias que ya están dentro de este repositorio. No requieren rutas hermanas, acceso remoto ni artefactos generados previamente.

## 1. Dashboard en un equipo local

Requiere Python 3.10 o posterior (el código usa anotaciones `list[...]` y unión `|`). No necesita npm ni paquetes para las pruebas de API. Desde la raíz:

```bash
cd dashboard
python3 -m unittest discover -s tests -v
python3 -m unittest discover -s real/tests -v
python3 server.py --source simulation
```

Abra `http://127.0.0.1:8000`. Para la ruta de proveedor externo artificial, detenga el servidor anterior y ejecute:

```bash
python3 server.py --source external
python3 emulador_sbc.py --url http://127.0.0.1:8000 --count 2
```

Esta demostración ejercita HTTP y datos artificiales; no lee SPI ni ADC. El servidor real acepta `--source simulation` o `--source external`, como puede comprobarse en `dashboard/server.py`.

## 2. ESP-IDF 5.5.6 para ESP32-C6

En una máquina de desarrollo con Git, Python y las herramientas de compilación instaladas, una instalación aislada puede prepararse así:

```bash
mkdir -p "$HOME/esp"
git clone --branch v5.5.6 --depth 1 https://github.com/espressif/esp-idf.git "$HOME/esp/esp-idf"
cd "$HOME/esp/esp-idf"
./install.sh esp32c6
. "$HOME/esp/esp-idf/export.sh"
idf.py --version
```

La salida debe identificar ESP-IDF `v5.5.6`. En cada sesión posterior hay que activar el entorno con `. "$HOME/esp/esp-idf/export.sh"`. Desde la raíz del repositorio, la comprobación de compilación es:

```bash
cd firmware
idf.py set-target esp32c6
idf.py build
```

`set-target` y `build` pueden crear `firmware/build/`; ese directorio es generado y no debe copiarse ni añadirse a Git. La compilación sólo verifica el código y el toolchain. No demuestra una placa, ADC, SPI ni una tasa física.

El firmware define `main.c` en `firmware/main/CMakeLists.txt` y el proyecto raíz en `firmware/CMakeLists.txt`; no hay que mover esos archivos ni introducir una configuración de placa no versionada.

## 3. Dependencias locales de Raspberry Pi

En la Pi, instalar Python 3 y el módulo de acceso SPI del sistema según la distribución. Una ruta Debian/ Raspberry Pi OS es:

```bash
sudo apt update
sudo apt install -y python3 python3-venv python3-pip python3-spidev
sudo raspi-config
```

En `raspi-config`, habilite SPI y reinicie sólo si el sistema lo solicita. Compruebe localmente que existe `/dev/spidev0.0` (o el dispositivo elegido); no dé por hecho el bus ni el CS. Las pruebas de protocolo no requieren `spidev`, porque usan un transporte falso. El lector real sí importa `spidev` al abrir el bus.

Para probar el código no físico de nuevo, desde `dashboard/`:

```bash
python3 -m unittest discover -s tests -v
python3 -m unittest discover -s real/tests -v
```

En la verificación U4, con Python 3.14.4, estos comandos reprodujeron 15/15 pruebas generales y 10/10 pruebas SPI. El `ImportError` transitorio registrado anteriormente no se reprodujo.

## 4. Orden seguro para hardware

Antes de conectar, confirme tierra común, 3.3 V, niveles lógicos, MOSI `GPIO7`, MISO `GPIO2`, SCLK `GPIO6`, CS `GPIO16` en el ESP32-C6 y ADC en `GPIO0`/`GPIO1`. Revise [`docs/raspberry-pi.md`](raspberry-pi.md). El firmware no usa GPIO READY.

Tras compilar y cargar manualmente un firmware revisado, el smoke test mínimo usa una sola ventana pequeña a **1 MHz**:

```bash
cd dashboard/real/raspberry
python3 probar_spi_fisico.py --spi-hz 1000000 --rate 100 --window 64 --count 1
```

Este comando requiere hardware conectado y firmware cargado; no debe ejecutarse como prueba automática ni desde una máquina sin Pi. Su salida sólo puede registrar ACK, lectura y throughput SPI observado; no confirma por sí sola la frecuencia física del ADC ni el end-to-end del dashboard.

## Nota sobre declaraciones copiadas

La copia `entregables/declaracion-uso-ia.md` conserva referencias históricas al export completo del chat, al documento de tareas ausente y a la antigua ruta de participación. La ubicación canónica nueva es `entregables/`; para completar ese requisito, la persona responsable debe aportar un export sanitizado.

## 5. Despliegue manual opcional

Sólo después de pruebas locales y, si aplica, del smoke test físico, una persona puede transferir manualmente el código fuente aprobado a la Pi, verificar checksums y ejecutar allí `dashboard/real/deploy/preparar_servicios.sh` según su contenido. Esta guía no prescribe SSH, no ejecuta despliegues y no copia `build/`, cachés ni resultados generados.
