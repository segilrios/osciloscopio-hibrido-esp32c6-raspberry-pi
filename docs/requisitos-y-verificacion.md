# Requisitos y matriz de verificación

**Fecha de corte:** 2026-11-05. Esta matriz separa evidencia de código, pruebas locales, registros comunicados por la persona usuaria y trabajo pendiente. Una prueba Python no demuestra cableado, ADC, temporización física ni integración end-to-end.

## Convenciones

- **Verificado (software):** observado en código o en una ejecución local reproducible.
- **Registrado por usuario:** resultado comunicado por la persona usuaria; no se conserva aquí el log primario.
- **Documentado:** antecedente de build o flash sin artefacto/log primario en este checkout.
- **Pendiente:** no hay evidencia suficiente para afirmarlo.
- No se verifica aquí el tiempo de conversión ni la cadencia física del ADC.

## Matriz detallada

| ID | Requisito | Método | Esperado | Observado | Procedencia / fecha | Estado |
|---|---|---|---|---|---|---|
| REQ-001 | Dos canales por ventana | Inspección de firmware, lector y pruebas | CH0 y CH1 con `N` muestras | El contrato y los tests manejan dos canales; no es una medición ADC | Código local; 2026-11-05 | Verificado (software) |
| REQ-002 | Tamaños de ventana | Tests del dashboard e inspección | `N ∈ {64,256,512,1024}` | Casos de software cubiertos; no hay captura física de cada tamaño | Código/tests locales; 2026-11-05 | Verificado (software) |
| REQ-003 | Tasas base | Inspección y tests de configuración | `100`, `1000` y `10000` Sa/s por canal | Valores aceptados por el software; no son tasas ADC medidas | Código/tests locales; 2026-11-05 | Verificado (software) |
| REQ-004 | Preset experimental de 40000 | Test del preset y revisión de límites | Debe quedar marcado como experimental | El test simula el caso; no demuestra 40000 Sa/s físicos | `dashboard/tests`; 2026-11-05 | Verificado (software; no físico) |
| REQ-005 | Transporte PSC1 | Tests SPI e inspección | Pi master, C6 slave SPI2, mode 0, sin READY | Contrato y lector cumplen esa estructura | Código y tests locales; 2026-11-05 | Verificado (software) |
| REQ-006 | Pinout | Inspección de firmware/lector | MOSI 7, MISO 2, SCLK 6, CS 16; ADC 0/1 | Pines presentes en las fuentes; cableado no observado localmente | Código local; 2026-11-05 | Verificado (código) |
| REQ-007 | Chunks | Tests de lector y protocolo | Cada respuesta `<=2048` bytes | Manejo y casos de chunk cubiertos por tests SPI | Código/tests locales; 2026-11-05 | Verificado (software) |
| REQ-008 | Integridad de protocolo | Tests de CRC, secuencia, ACK, errores y duplicados | Tramas inválidas y duplicados rechazados | Casos SPI pasan en transporte simulado | `dashboard/real/tests`; 2026-11-05 | Verificado (software) |
| REQ-009 | Refresh | Tests API y revisión del contrato | Consultas cada `1–4 s` sin exigir frame nuevo | Estados de datos nuevos/obsoletos cubiertos; no prueba hardware | `dashboard/tests`; 2026-11-05 | Verificado (software) |
| REQ-010 | Ventana lenta | Cálculo reproducible | `1024/100 = 10,24 s` mínimo | Cálculo confirmado; no es una medición de ADC | Cálculo documental; 2026-11-05 | Verificado (cálculo) |
| REQ-011 | Build de firmware | Registro de sesión y artefacto/hash | `idf.py build` exitoso con versión identificada | Build exitoso y flash están documentados, pero faltan log y artefactos primarios aquí | Registro documental; fecha declarada en el informe | Documentado, no reproducible aquí |
| REQ-012 | SPI/ADC físico | Captura con C6, ADC y Pi conectados | Evidencia primaria de conexión y adquisición | Se comunicaron tres ventanas a 1 MHz; no hay log primario local ni medición ADC | Persona usuaria; 2026-11-05 | Registrado por usuario |
| REQ-013 | Despliegue Pi | Ejecutar script y conservar salida | Versión, checksum, servicios y logs | El script existe; ejecución no está demostrada en este checkout | `dashboard/real/deploy`; 2026-11-05 | Pendiente |
| REQ-014 | HTTP/dashboard físico | Consulta web alimentada por captura identificada | API/UI muestran una ventana del hardware | Servicio web fue comunicado, pero no se conserva trazabilidad primaria hasta ADC | Registro comunicado; 2026-11-05 | Registrado, alcance limitado |
| REQ-015 | Integración end-to-end | Prueba trazable ADC → MCU → SPI → API → dashboard | Fuente física identificada y log completo | No hay evidencia primaria suficiente | Ausencia de log local; 2026-11-05 | Pendiente |
| REQ-016 | Caso físico 1 MHz/N64 | Ejecutar tres ventanas y conservar salida | Tres ventanas duales con configuración identificada | PING PSC1 válido, ACK de CONFIG y tres ventanas fueron comunicados por la persona usuaria; no se inventan muestras ni tiempos | Persona usuaria; 2026-11-05 | Registrado por usuario |
| REQ-017 | Caso físico 2 MHz/N512 | Repetir prueba con `2 MHz`, `N=512` | Log primario y resultado reproducible | No ejecutado o no aportado | Estado del proyecto; 2026-11-05 | Pendiente |
| REQ-018 | Temporización ADC | Instrumentación o timestamps confiables | Cadencia y tiempos físicos medidos | No existe verificación de timing ADC | Ausencia de evidencia; 2026-11-05 | Pendiente |

## Pruebas locales observadas

Desde `dashboard/` se observó en U1 el conjunto local de **15 pruebas generales**, y se vuelve a ejecutar en la verificación de esta entrega. El registro conservado en [`dashboard/resultado_pruebas_version_real.txt`](../dashboard/resultado_pruebas_version_real.txt) muestra `Ran 15 tests ... OK`. Incluye API, emulación y flujo externo simulado: no es hardware.

El conjunto SPI se ejecutó también desde `dashboard/` mediante:

```bash
python3 -m unittest discover -s real/tests -v
```

La ejecución actual observa **10 casos de prueba SPI** (`Ran 10 tests ... OK`), consistente con el antecedente de U1. Las diez pruebas comprueban protocolo/transporte simulado y no cableado, ADC ni reloj físico.

## Build, flash y hardware

El build y el flash aparecen registrados en documentación previa, pero sus logs primarios y artefactos no están en este repositorio. La prueba física comunicada por la persona usuaria comprende **1 MHz, `N=64` y tres ventanas**; se conserva como resultado aportado, no como ejecución reproducible aquí. El caso **2 MHz, `N=512`** permanece pendiente.

No se afirma ningún valor de tensión, throughput, latencia, frecuencia ADC o máximo del SPI que no tenga evidencia primaria. En particular, las pruebas de software no sustituyen una medición física y no hay verificación de temporización del ADC.

## Enlaces de procedimiento

- [`docs/instalacion.md`](instalacion.md)
- [`docs/raspberry-pi.md`](raspberry-pi.md)
- [`docs/informe-proyecto-ia.md`](informe-proyecto-ia.md)
- [`docs/video.md`](video.md)
