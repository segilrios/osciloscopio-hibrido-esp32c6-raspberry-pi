# Informe del proyecto y uso de IA

**Proyecto:** osciloscopio híbrido Raspberry Pi–ESP32-C6
**Fecha de corte documental:** 2026-11-05

## 1. Alcance y arquitectura

El repositorio contiene el firmware, el dashboard, las pruebas locales y la documentación en [`firmware/`](../firmware/), [`dashboard/`](../dashboard/) y [`docs/`](.). La arquitectura prevista usa dos canales ADC en el ESP32-C6, PSC1 sobre SPI2 hacia la Raspberry Pi y una API HTTP para el dashboard. La configuración nominal y sus límites se describen en [`docs/arquitectura.md`](arquitectura.md) y [`docs/requisitos-y-verificacion.md`](requisitos-y-verificacion.md).

La evidencia de software no demuestra cableado, conversión ADC, temporización física ni una cadena ADC → MCU → SPI → API → dashboard. El caso físico comunicado es 1 MHz, `N=64` y tres ventanas; 2 MHz/`N=512` y la integración end-to-end permanecen pendientes.

## 2. Evidencia y procedencia

| Evidencia | Procedencia en este checkout | Alcance y límite |
|---|---|---|
| Código y contrato PSC1 | [`firmware/main/main.c`](../firmware/main/main.c), [`dashboard/real/raspberry/lector_spi.py`](../dashboard/real/raspberry/lector_spi.py) | Permite inspección del diseño; no acredita hardware. |
| 15 pruebas generales | [`dashboard/resultado_pruebas_version_real.txt`](../dashboard/resultado_pruebas_version_real.txt) y ejecución local | Python/API/emulación; no medición física. |
| Pruebas SPI | [`dashboard/real/tests/test_spi_protocol.py`](../dashboard/real/tests/test_spi_protocol.py) y ejecución local | Transporte/protocolo simulado; el checkout actual observa 10 casos, consistente con U1. |
| Build y flash | Registro documental histórico | Se declaran como registrados; los logs y artefactos primarios no están aquí. |
| 1 MHz/`N=64`, tres ventanas | Resultado comunicado por la persona usuaria | Observación de usuario sin log primario en este checkout; no aporta valores inventados ni demuestra end-to-end. |
| Declaraciones académicas | [`entregables/declaracion-uso-ia.md`](../entregables/declaracion-uso-ia.md) y [`entregables/declaracion-participacion-y-aportes.md`](../entregables/declaracion-participacion-y-aportes.md) | Declaraciones existentes, enlazadas sin modificación. |

La fecha de corte evita presentar como nueva una evidencia histórica. El informe no contiene hashes, capturas, salidas seriales ni logs que no estén almacenados en este repositorio.

## 3. Declaración explícita de uso de IA

La IA se utilizó como asistencia para inspeccionar fuentes, proponer y revisar documentación, analizar consistencia, preparar comandos y ayudar a interpretar resultados aportados. Las personas responsables conservaron las decisiones, la autorización de operaciones y la validación experimental. La IA no realizó por sí misma la adquisición física ni convierte una simulación en una medición.

La identificación de modelos se rige por la declaración existente enlazada arriba: se conservan únicamente los identificadores que esa declaración ya consigna y se reconoce que las sesiones históricas no tienen metadatos completos verificables en este checkout. No se inventan identificadores de modelos anteriores ni se supone que todas las interacciones usaron el mismo modelo.

La procedencia de cada afirmación se distingue así:

1. **Fuente local directa:** código, tests y documentos presentes en las rutas relativas de este repositorio.
2. **Registro documental secundario:** build y flash descritos, sin logs o binarios primarios locales.
3. **Resultado comunicado por usuario:** prueba física 1 MHz/`N=64` de tres ventanas, sin salida primaria conservada aquí.
4. **Pendiente:** 2 MHz/`N=512`, end-to-end físico y temporización del ADC.

## 4. Interacciones y exportación de chats

Los documentos disponibles permiten describir temas y decisiones —arquitectura, PSC1, pruebas, build/flash, diagnóstico y límites de evidencia—, pero no contienen un export íntegro y literal de todas las conversaciones. Cualquier resumen de interacción es reconstrucción documental: no se presenta como chat completo, no atribuye frases exactas no conservadas y no rellena fechas, modelos o turnos ausentes.

La brecha de exportación/redacción sigue abierta: para cerrar este requisito se necesita un export auténtico, revisión humana de secretos y PII, y una copia sanitizada incorporada explícitamente. Hasta entonces, el vídeo y la transcripción completa no se declaran entregados. Las declaraciones existentes permanecen sin cambios y son la referencia de participación y uso de IA.

## 5. Estado y próximos pasos

- **Verificado localmente:** código y pruebas Python; separar las 15 pruebas generales de las pruebas SPI.
- **Registrado:** build/flash históricos y prueba física comunicada de 1 MHz/`N=64` con tres ventanas.
- **Pendiente:** conservar logs primarios, repetir 2 MHz/`N=512`, demostrar end-to-end con fuente física identificada y medir —si se requiere— la cadencia ADC con instrumentación.
- **No afirmado:** valores de tensión, máximo de SPI, throughput, latencia o timing ADC sin evidencia primaria.

La guía de filmación segura y su checklist están en [`docs/video.md`](video.md). No se añade un nombre de archivo de vídeo como si existiera; cualquier placeholder de archivo queda explícitamente pendiente.
