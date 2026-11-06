# Osciloscopio híbrido ESP32-C6 + Raspberry Pi

Repositorio unificado del firmware ESP32-C6, el dashboard web y la documentación del proyecto. La adquisición prevista usa dos canales ADC en el C6, un enlace SPI PSC1 y una API HTTP local para la interfaz.

## Estado honesto

El código fuente y las pruebas automatizadas están aquí; las pruebas Python no demuestran cableado, ADC, temporización física ni integración end-to-end. No se afirma que el firmware haya sido compilado en este entorno ni que una Raspberry Pi haya sido conectada. El vídeo y la exportación completa/redactada de chats siguen **pendientes**: deben aportarse como evidencia humana auténtica en [`entregables/`](entregables/), sin fabricar sustitutos.

La referencia [`dashboard/LEEME_PRIMERO.txt`](dashboard/LEEME_PRIMERO.txt) es histórica y está desactualizada para este repositorio: describe un ejemplo ESP32 original excluido de la entrega. Se conserva sin editar; las instrucciones vigentes son las de esta documentación.

## Contenido

- [`firmware/main/main.c`](firmware/main/main.c): ESP32-C6, ADC continuo y slave SPI PSC1.
- [`dashboard/`](dashboard/): servidor HTTP, UI, emulador, lector SPI y pruebas.
- [`docs/instalacion.md`](docs/instalacion.md): preparación reproducible local.
- [`docs/arquitectura.md`](docs/arquitectura.md): componentes, contrato y límites de evidencia.
- [`docs/raspberry-pi.md`](docs/raspberry-pi.md): dependencias y smoke test físico opcional.
- [`docs/requisitos-y-verificacion.md`](docs/requisitos-y-verificacion.md): matriz existente, pendiente de reconciliación en U3.
- [`entregables/`](entregables/): declaraciones existentes.

## Prueba local del dashboard (sin hardware)

Desde la carpeta `dashboard/`:

```bash
python3 -m unittest discover -s tests -v
python3 -m unittest discover -s real/tests -v
python3 server.py --source simulation
```

La primera orden cubre API, emulación y estados; la segunda cubre el protocolo SPI con transporte falso. El servidor se abre en `http://127.0.0.1:8000`; sus datos simulados no son mediciones. Para probar el flujo HTTP externo artificial, en otra terminal, también desde `dashboard/`, use `python3 server.py --source external` y `python3 emulador_sbc.py`.

## Flujo de trabajo recomendado

1. Leer [`docs/instalacion.md`](docs/instalacion.md) y ejecutar primero las pruebas locales.
2. Preparar ESP-IDF 5.5.6 y comprobar el firmware con `idf.py build`, sin confundir compilación con prueba física.
3. Preparar la Pi y revisar pines, niveles de 3.3 V y alimentación; ejecutar el smoke test de 1 MHz sólo con hardware revisado.
4. Integrar el adaptador SPI y la API, registrar resultados y sólo después considerar un despliegue manual.

No se copian builds, artefactos generados ni credenciales al repositorio; tampoco se ejecutan comandos remotos desde esta guía.
