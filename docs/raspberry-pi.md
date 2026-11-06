# Raspberry Pi: preparación y smoke test

## Qué está implementado

El código real está en `dashboard/real/raspberry/`: `lector_spi.py`, `protocolo_spi.py`, `ejecutar_spi_real.py` y `probar_spi_fisico.py`. `spidev` sólo es necesario al abrir el dispositivo SPI. Las pruebas de `dashboard/real/tests/` usan transporte falso y no requieren una Pi.

El lector usa bus `0`, dispositivo `0` y 2 MHz por defecto, pero el primer smoke test documentado aquí baja deliberadamente a 1 MHz. No existe GPIO READY: cada comando y cada chunk es una transacción CS independiente. El firmware usa SPI mode 0 y estos pines del ESP32-C6:

| Señal | ESP32-C6 |
|---|---:|
| MOSI | GPIO7 |
| MISO | GPIO2 |
| SCLK | GPIO6 |
| CS | GPIO16 |
| ADC canal 1 / 2 | GPIO0 / GPIO1 |

Verifique niveles de 3.3 V, tierra común, alimentación y el bus/CS realmente cableados antes de energizar. No conecte una señal analógica fuera del rango admitido por la placa.

## Dependencias y comprobación local

En Raspberry Pi OS/Debian, una opción es:

```bash
sudo apt update
sudo apt install -y python3 python3-venv python3-pip python3-spidev
sudo raspi-config
ls -l /dev/spidev*
```

Habilite SPI en `raspi-config` y confirme el dispositivo concreto. Desde la raíz del repositorio, las pruebas de software se ejecutan en el cwd exigido por sus imports:

```bash
cd dashboard
python3 -m unittest discover -s tests -v
python3 -m unittest discover -s real/tests -v
```

Las pruebas del segundo comando cubren CRC, ACK, secuencias y chunks con un fake; una pasada no es prueba de cableado.

## Smoke test físico de bajo riesgo

Ejecute sólo después de cargar un firmware ESP32-C6 compatible, revisar el cableado y confirmar que `/dev/spidev0.0` corresponde al CS conectado. Desde `dashboard/real/raspberry/`:

```bash
python3 probar_spi_fisico.py --spi-hz 1000000 --rate 100 --window 64 --count 1
```

La combinación usa 1 MHz, una tasa baja, la ventana mínima y una sola lectura para reducir duración y tráfico. El script hace `PING`, aplica la configuración y lee ventanas; si no hay ventana nueva puede informar ese estado. Registre la salida sin convertirla en evidencia de ADC: el propio programa distingue throughput SPI observado de frecuencia física.

No ejecute todavía `ejecutar_spi_real.py` como servicio. Primero valide el smoke test, la API local y una ventana coherente; después puede iniciarlo en una terminal con `--api http://127.0.0.1:8000` mientras el servidor corre con `python3 dashboard/server.py --source external` desde la raíz adecuada. Esa prueba sigue siendo manual y depende del hardware real.

## Despliegue posterior, opcional

Sólo tras pruebas satisfactorias, transfiera manualmente el código fuente aprobado, compruebe checksums y revise [`dashboard/real/deploy/preparar_servicios.sh`](../dashboard/real/deploy/preparar_servicios.sh) en la Pi. Ejecútelo localmente allí según sus prerrequisitos; no se documenta SSH ni se afirma que el servicio haya sido instalado. Nunca copie `firmware/build/`, `__pycache__/`, logs o artefactos generados como parte del despliegue documentado.

## Evidencia que aún falta

Una prueba de Pi sin hardware sólo verifica instalación/API. La integración end-to-end exige captura o registro del ESP32-C6 conectado, datos ADC plausibles, SPI íntegro y la ventana visible en la API/UI. El vídeo y la exportación completa de chats siguen pendientes y deben incorporarse a [`../entregables/`](../entregables/) cuando exista material auténtico.
