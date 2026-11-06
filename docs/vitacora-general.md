# Bitácora general del proyecto

## Propósito y estado histórico

Esta bitácora conserva decisiones y estados de distintas etapas. Las entradas que siguen al encabezado **histórico** no deben leerse como el estado final actual; la evidencia posterior está reconciliada en [`docs/requisitos-y-verificacion.md`](requisitos-y-verificacion.md) y [`docs/informe-proyecto-ia.md`](informe-proyecto-ia.md).

## Estado histórico (anterior a la evidencia posterior)

- El código local describía ESP32-C6 slave SPI2 con PSC1, sin READY, y ADC en GPIO0/GPIO1.
- El dashboard registraba pruebas Python, separadas de hardware.
- En esta etapa se anotaba que el toolchain, hardware, HTTP en Pi y end-to-end no estaban verificados.
- El script de despliegue está en [`../dashboard/real/deploy/preparar_servicios.sh`](../dashboard/real/deploy/preparar_servicios.sh); su existencia nunca se trató como ejecución.

Estas notas son contexto histórico y no deben borrar ni sobreescribir la evidencia posterior.

## Evidencia posterior registrada

- U1 observó 15 pruebas generales locales del dashboard.
- El registro histórico `dashboard/resultado_pruebas.txt` conserva 5 pruebas SPI; la ejecución actual desde `dashboard/` observa 10 casos (además de 15 generales). El número anterior corresponde a otra versión y no sustituye el resultado reproducido ahora.
- El build y el flash del firmware quedaron registrados documentalmente, pero sus logs y artefactos primarios no están en este repositorio.
- La persona usuaria comunicó una prueba física de 1 MHz, `N=64` y tres ventanas con respuesta PING/ACK y datos de dos canales. No se inventan muestras, tiempos ni valores de medición.
- 2 MHz/`N=512`, la integración end-to-end y la temporización del ADC siguen pendientes.

## Decisiones

| ID | Decisión | Motivo | Estado |
|---|---|---|---|
| DEC-001 | Pi master; C6 slave SPI2, mode 0, sin READY | Contrato implementado | Verificado en código; hardware separado |
| DEC-002 | Ventanas `64/256/512/1024` y tasas base `100/1000/10000` | Contrato común | Verificado en software |
| DEC-003 | `40000` Sa/s sólo experimental | No convertir un preset en medición | Adoptada |
| DEC-004 | Refresh `1–4 s` no garantiza ventana nueva | `1024/100 = 10,24 s` mínimo | Adoptada |
| DEC-005 | Transferencia manual y despliegue local | Procedimiento documentado | Ejecución primaria no conservada |
| DEC-006 | Separar software, hardware comunicado y pendientes | Evitar sobreafirmaciones | Vigente |

## Próximos pasos

1. Conservar logs primarios de build, flash y prueba física.
2. Ejecutar y registrar 2 MHz con `N=512`.
3. Demostrar la ruta end-to-end con una fuente física identificada.
4. Medir la cadencia del ADC sólo con instrumentación y evidencia apropiadas.
5. Aportar vídeo y exportación de chat auténticos, sanitizados y revisables.
