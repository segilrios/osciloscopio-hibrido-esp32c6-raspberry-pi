# Declaración de participación y aportes

## 1. Identificación del trabajo

**Proyecto:** osciloscopio híbrido ESP32-C6 y Raspberry Pi

**Asignatura:** Linux Embebidos

**Programa:** Ingeniería Electrónica

**Institución:** Universidad Nacional de Colombia, sede Manizales

La presente declaración identifica las responsabilidades asociadas con el desarrollo, la integración, la verificación y la documentación del proyecto. Su alcance se limita a describir los aportes reconocidos por el equipo, sin asignar porcentajes individuales ni establecer autorías exclusivas.

## 2. Criterio de atribución y responsabilidad compartida

Las responsabilidades se organizaron en tres áreas de trabajo, cada una a cargo de una pareja. De conformidad con lo expresado por el grupo, **todas las actividades documentadas de cada área se atribuyen conjuntamente a las dos personas que integran la pareja correspondiente**. Por esta razón, las descripciones individuales reproducen las actividades compartidas del área y no implican una división, jerarquización o asignación exclusiva de los aportes.

La atribución anterior constituye una **declaración directa del grupo**. La documentación del proyecto acredita la existencia de las áreas y actividades técnicas, pero no contiene historial Git previo, registros individualizados de autoría ni evidencia suficiente para determinar una distribución personal. Tampoco se dispone de un repositorio remoto que aporte información adicional. En consecuencia, no se presentan porcentajes de participación ni se deducen contribuciones individuales a partir de commits.

## 3. Correspondencia entre áreas de trabajo y categorías académicas

| Área de responsabilidad compartida | Integrantes | Actividades y categorías académicas comprendidas |
|---|---|---|
| Dashboard | Luis Alejandro Pérez Muñoz; Juan Eduardo Páez Galvis | Desarrollo y revisión de la interfaz, API y servicios web; visualización de ventanas; apoyo a pruebas unitarias de software SBC y a la verificación de integración con la fuente SPI. |
| ESP32-C6 | Philip Santiago Adarmes Aldana; Sebastián Torres Gamboa | Configuración y documentación del microcontrolador; firmware de adquisición ADC y protocolo PSC1 sin READY; pruebas preliminares, criterios experimentales y apoyo a pruebas unitarias del MCU. |
| Raspberry Pi e integración | Jerónimo Novoa Giraldo; Sergio Alejandro Gil | Configuración y operación de la SBC; lector SPI, servicios API/Nginx, coordinación del repositorio, integración y unificación del proyecto; matriz de verificación y pruebas de integración. |

La correspondencia anterior se establece con base en la evidencia disponible. Las pruebas ejecutadas en Python y los registros documentales no sustituyen una medición experimental integral del hardware.

## 4. Participación por integrante

### 4.1 Luis Alejandro Pérez Muñoz

**Rol compartido:** desarrollo, configuración y documentación del dashboard.

Actividades atribuidas conjuntamente con Juan Eduardo Páez Galvis:

- Desarrollo y revisión de la presentación de los dos canales y de la actualización de ventanas en la interfaz web.
- Organización del flujo entre lector, API HTTP, Nginx y dashboard, conservando la separación entre datos simulados y datos procedentes del hardware.
- Revisión del comportamiento de refresco de `1–4 s`, sin interpretarlo como garantía de una ventana nueva cuando la adquisición tarda más.
- Apoyo a las pruebas unitarias del software de la SBC y a la revisión de los resultados registrados para API, transporte y visualización.
- Participación en la verificación de integración del dashboard con el contrato de datos PSC1 y con los servicios de la Raspberry Pi.

### 4.2 Juan Eduardo Páez Galvis

**Rol compartido:** desarrollo, configuración y documentación del dashboard.

Actividades atribuidas conjuntamente con Luis Alejandro Pérez Muñoz:

- Desarrollo y revisión de la presentación de los dos canales y de la actualización de ventanas en la interfaz web.
- Organización del flujo entre lector, API HTTP, Nginx y dashboard, conservando la separación entre datos simulados y datos procedentes del hardware.
- Revisión del comportamiento de refresco de `1–4 s`, sin interpretarlo como garantía de una ventana nueva cuando la adquisición tarda más.
- Apoyo a las pruebas unitarias del software de la SBC y a la revisión de los resultados registrados para API, transporte y visualización.
- Participación en la verificación de integración del dashboard con el contrato de datos PSC1 y con los servicios de la Raspberry Pi.

### 4.3 Philip Santiago Adarmes Aldana

**Rol compartido:** configuración, firmware y documentación del ESP32-C6.

Actividades atribuidas conjuntamente con Sebastián Torres Gamboa:

- Configuración del ESP32-C6 y documentación de los canales ADC en GPIO0 y GPIO1 y de la interfaz SPI2.
- Desarrollo y revisión del firmware para adquisición por ventanas, selección de presets y conservación de las muestras de ambos canales.
- Implementación y documentación del protocolo PSC1 como esclavo SPI sin señal READY, con pines MOSI GPIO7, MISO GPIO2, SCLK GPIO6 y CS GPIO16.
- Preparación y análisis de pruebas preliminares del microcontrolador, incluidos valores ADC, ventanas, temporización nominal y mensajes de diagnóstico.
- Participación en el diseño experimental y en los criterios de verificación del MCU, diferenciando configuración nominal, pruebas de software y medición física.
- Apoyo a pruebas unitarias y de integración del firmware con el lector SPI de la Raspberry Pi.

### 4.4 Sebastián Torres Gamboa

**Rol compartido:** configuración, firmware y documentación del ESP32-C6.

Actividades atribuidas conjuntamente con Philip Santiago Adarmes Aldana:

- Configuración del ESP32-C6 y documentación de los canales ADC en GPIO0 y GPIO1 y de la interfaz SPI2.
- Desarrollo y revisión del firmware para adquisición por ventanas, selección de presets y conservación de las muestras de ambos canales.
- Implementación y documentación del protocolo PSC1 como esclavo SPI sin señal READY, con pines MOSI GPIO7, MISO GPIO2, SCLK GPIO6 y CS GPIO16.
- Preparación y análisis de pruebas preliminares del microcontrolador, incluidos valores ADC, ventanas, temporización nominal y mensajes de diagnóstico.
- Participación en el diseño experimental y en los criterios de verificación del MCU, diferenciando configuración nominal, pruebas de software y medición física.
- Apoyo a pruebas unitarias y de integración del firmware con el lector SPI de la Raspberry Pi.

### 4.5 Jerónimo Novoa Giraldo

**Rol compartido:** configuración, operación, integración y unificación del proyecto en Raspberry Pi.

Actividades atribuidas conjuntamente con Sergio Alejandro Gil:

- Configuración y operación del software y los servicios de la SBC, incluidos lector SPI, API HTTP, Nginx y rutas de despliegue local.
- Integración del lector como maestro SPI con el protocolo PSC1 sin READY y manejo de transacciones independientes y respuestas por chunks.
- Coordinación del repositorio documental, unificación de arquitectura, requisitos, bitácora y referencias entre firmware, SBC y dashboard.
- Elaboración y revisión de la matriz de requisitos y verificación, con separación entre evidencia de código, pruebas Python, resultados físicos comunicados y pendientes.
- Apoyo a pruebas unitarias del software de la SBC y a pruebas de integración Pi–ESP32-C6, API y dashboard.
- Consolidación de instrucciones reproducibles para transferencia manual, instalación y comprobación de servicios en la Raspberry Pi.

### 4.6 Sergio Alejandro Gil

**Rol compartido:** configuración, operación, integración y unificación del proyecto en Raspberry Pi.

Actividades atribuidas conjuntamente con Jerónimo Novoa Giraldo:

- Configuración y operación del software y los servicios de la SBC, incluidos lector SPI, API HTTP, Nginx y rutas de despliegue local.
- Integración del lector como maestro SPI con el protocolo PSC1 sin READY y manejo de transacciones independientes y respuestas por chunks.
- Coordinación del repositorio documental, unificación de arquitectura, requisitos, bitácora y referencias entre firmware, SBC y dashboard.
- Elaboración y revisión de la matriz de requisitos y verificación, con separación entre evidencia de código, pruebas Python, resultados físicos comunicados y pendientes.
- Apoyo a pruebas unitarias del software de la SBC y a pruebas de integración Pi–ESP32-C6, API y dashboard.
- Consolidación de instrucciones reproducibles para transferencia manual, instalación y comprobación de servicios en la Raspberry Pi.

## 5. Fundamento documental y limitaciones de la evidencia

| Elemento documentado | Fuente disponible | Alcance probatorio y limitación |
|---|---|---|
| Firmware ESP32-C6, ADC y PSC1 sin READY | `README.md`, `docs/arquitectura.md`, `docs/informe-proyecto-ia.md` | La documentación describe el código y su configuración; los repositorios fuente y los registros primarios no forman parte de esta copia local. |
| Software SBC, API, Nginx y dashboard | `docs/raspberry-pi.md`, `docs/arquitectura.md`, `docs/informe-proyecto-ia.md` | Permite identificar componentes y actividades; no demuestra por sí sola una integración física end-to-end. |
| Pruebas Python | Registro documental de 15 pruebas generales y 10 pruebas SPI | Los resultados constan en la documentación del proyecto, pero no se dispone localmente de los registros ni de los artefactos primarios; por tanto, no equivalen a una prueba eléctrica. |
| Compilación y flasheo | Registro documental de ESP-IDF 5.5.6, compilación, hash y flasheo verificado | El binario y los registros primarios no se encuentran en esta copia local; este antecedente no demuestra la temporización física del ADC. |
| Integración física | Registro documental de PING, CONFIG y tres ventanas comunicadas por integrantes del equipo | La observación consta en la documentación del proyecto, sin un registro primario disponible en esta copia local; no demuestra el máximo de SPI ni la totalidad de la cadena ADC–web. |
| Preset de 40 kSa/s | Código y documentación lo tratan como opción experimental | **No fue demostrado experimentalmente** y no se declara como prestación verificada. |

Los documentos históricos corresponden a etapas distintas del proyecto. Ante eventuales diferencias, esta declaración conserva la procedencia de cada afirmación y evita presentar un plan, un resultado comunicado o una prueba de software como evidencia experimental de mayor alcance.

## 6. Declaración de conformidad

Mediante su firma, las personas integrantes manifiestan haber revisado esta declaración y reconocen que la atribución compartida por parejas representa el acuerdo del grupo. La conformidad académica expresada no modifica el estado de las prestaciones técnicas que permanecen limitadas o pendientes de verificación.


