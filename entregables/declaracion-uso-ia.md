# Declaración de uso de inteligencia artificial

## 1. Objeto de la declaración

La presente declaración documenta el uso de herramientas de inteligencia artificial en el proyecto de osciloscopio híbrido ESP32-C6–Raspberry Pi. Para tal efecto, identifica los modelos reconocibles, las tareas asistidas, los procedimientos de revisión humana, el alcance de la evidencia disponible y el contenido recuperable de las interacciones. Los resúmenes no se presentan como transcripciones literales ni se atribuyen a la IA verificaciones experimentales que no fueron realizadas.

## 2. Modelos identificados y límites de identificación

| Contexto de uso | Identificación disponible | Alcance de la identificación |
|---|---|---|
| Declaración histórica | `gpt-6-sol` | Figura como declaración documental en `docs/informe-proyecto-ia.md`. Se conserva como antecedente, dado que esta copia local no contiene metadatos primarios de la sesión que permitan validarla de manera independiente. |
| Inspección y redacción actuales | `openai/gpt-5.6-sol` | Es el modelo utilizado en la sesión actual para inspeccionar las fuentes, reconciliar afirmaciones y preparar estas declaraciones. |
| Otras sesiones históricas | Identidad desconocida | No se infiere ni se completa la denominación del modelo. Los registros disponibles no permiten afirmar que todas las interacciones se realizaron con el mismo sistema. |

La identificación del modelo permite reconocer la herramienta empleada, pero no demuestra por sí misma la exactitud de sus respuestas ni sustituye la revisión académica y técnica.

## 3. Actividades asistidas mediante inteligencia artificial

De acuerdo con los registros históricos y con la sesión actual, la asistencia de IA comprendió las siguientes actividades:

- Exploración técnica de la arquitectura ESP32-C6, Raspberry Pi, SPI, ADC, API, Nginx y dashboard.
- Redacción, reorganización y revisión de bitácoras, arquitectura, requisitos, guías e informes.
- Asistencia histórica en código y pruebas para el firmware, el lector SPI y las comprobaciones Python, según lo consignado en la documentación del proyecto.
- Formulación de comandos para compilación, flasheo, monitor serial, pruebas y diagnóstico, sin que su sola formulación permita presumir la ejecución.
- Interpretación de resultados comunicados por integrantes del equipo o consignados en documentos, incluidas las pruebas Python, la compilación, el flasheo y las observaciones físicas.
- Revisión de consistencia, trazabilidad, cálculos y separación entre evidencia de software, evidencia documental y resultados experimentales.
- Preparación actual de la declaración de participación y aportes y de esta declaración de uso de IA.

La intervención de la IA tuvo carácter auxiliar en las actividades intelectuales y operativas. No efectuó de manera autónoma la adquisición de señales físicas, no observó directamente la totalidad del hardware y no sustituyó las decisiones ni la validación del equipo.

## 4. Procedimientos de revisión y verificación humana

El contenido generado o sugerido fue revisado, seleccionado y corregido por las personas responsables. Se emplearon o registraron los siguientes procedimientos de verificación:

1. **Inspección del repositorio y de la documentación:** contraste entre el README, la bitácora, la arquitectura, la guía de Raspberry Pi, la matriz de requisitos, el informe de IA y los registros ODD.
2. **Comprobaciones aritméticas:** revisión de relaciones como `1024 / 100 = 10,24 s` y distinción entre tasa nominal, tiempo de ventana, frecuencia de refresco y throughput de una prueba.
3. **Pruebas registradas:** contraste documental de 15 pruebas generales y 10 pruebas SPI ejecutadas en Python. Esta copia local no contiene los registros ni los repositorios fuente primarios; por ello, los resultados se presentan como antecedentes del proyecto y no como una nueva ejecución.
4. **Compilación, hash y flasheo registrados:** revisión documental del uso de ESP-IDF 5.5.6, de la compilación satisfactoria, del SHA-256 del binario y del flasheo con hashes verificados. Los registros y artefactos primarios no se encuentran en esta copia local.
5. **Resultados físicos comunicados por integrantes del equipo:** revisión del registro de una respuesta PING válida, del ACK de CONFIG y de tres ventanas de dos canales. La salida primaria no se encuentra disponible en esta copia local y la observación no demuestra la totalidad de la cadena ADC–dashboard.
6. **Revisión de afirmaciones y límites:** búsqueda de contradicciones históricas, identificación de pendientes y rechazo de conclusiones que excedan la evidencia.

### 4.1 Alcance probatorio de la evidencia

| Nivel de evidencia | Ejemplo | Alcance de la afirmación |
|---|---|---|
| Inspección directa de esta copia local | Documentos y referencias locales | Contenido y coherencia de la documentación disponible. |
| Registro documental sin artefacto primario local | 15 + 10 pruebas; compilación y flasheo; PING, CONFIG y tres ventanas | El proyecto conserva estas observaciones como antecedentes, pero no es posible repetirlas a partir de esta copia local. |
| Cálculo derivado | Tiempo mínimo de una ventana | Resultado matemático bajo los parámetros declarados, no comportamiento físico medido. |
| Propuesta o pendiente | 40 kSa/s, máximo SPI, integración ADC–web completa | Objetivo o caso de prueba que todavía requiere evidencia experimental. |

En particular, **40 kSa/s no está demostrado experimentalmente**. Que exista un preset o una frecuencia configurada no prueba que las conversiones ADC de ambos canales se produzcan efectivamente a esa tasa.

## 5. Contenido recuperable de las interacciones y secuencia documental

El repositorio no contiene un registro literal íntegro de todas las conversaciones. En consecuencia, la reconstrucción siguiente no incorpora fechas, instrucciones ni respuestas exactas que no estén documentadas.

### 5.1 Resumen temático histórico disponible

`contexto_chat_proyecto_esp32c6.txt` constituye un **resumen temático**, no una transcripción literal. El archivo reúne, entre otros, los siguientes asuntos:

- objetivo del osciloscopio híbrido, reparto funcional entre ESP32-C6 y Raspberry Pi, y comunicación SPI;
- identificación de la placa, entorno ESP-IDF, comandos habituales de build, flash y monitor;
- adquisición ADC en dos canales, observaciones de tensión comunicadas por el usuario y necesidad de tierra común;
- presets de 100, 1.000 y 10.000 Sa/s, opción experimental de 40.000 Sa/s y ventanas de 64 muestras;
- precauciones para no confundir frecuencia configurada con conversión física medida;
- conservación de datos ADC crudos y límites de una conversión aproximada a voltios;
- preparación de SPI y Raspberry Pi, pinout propuesto en la etapa descrita y necesidad de comprobar el código y hardware reales;
- criterios de pruebas para la exposición, alcance académico, preferencias de documentación y próximos pasos.

El propio resumen señala que algunas partes del historial fueron omitidas o condensadas y que no incluye el archivo fuente más reciente de aquella etapa. Por consiguiente, no constituye una conversación completa ni una reproducción exacta de intervenciones individuales.

### 5.2 Desarrollo histórico reconstruido

Los documentos del proyecto permiten reconstruir la siguiente secuencia general: preparación inicial de la arquitectura y las guías; inspección y adaptación del contrato PSC1 sin READY; ejecución registrada de pruebas Python; instalación de ESP-IDF 5.5.6; compilación y flasheo autorizados; comunicación de resultados físicos PING, CONFIG y tres ventanas; y revisión documental para distinguir hechos, resultados comunicados y asuntos pendientes. Esta secuencia corresponde a una síntesis documental y no a una cronología literal de instrucciones y respuestas.

### 5.3 Contenido de la interacción actual

Durante la interacción actual se proporcionaron y confirmaron los siguientes elementos:

- requisitos detallados para la documentación académica y sus criterios de verificación;
- identificación de las tres parejas de participantes y de sus respectivas áreas de responsabilidad;
- solicitud de una declaración de participación y de una declaración de uso de IA;
- confirmación expresa de que todas las actividades documentadas de cada área podían atribuirse conjuntamente a las dos personas de la pareja correspondiente.

La IA intervino en la inspección de las fuentes indicadas, el contraste de afirmaciones históricas, la redacción de ambos documentos y la ejecución de comprobaciones estructurales. Esta descripción sintetiza el contenido funcional de la interacción y no constituye una reproducción literal de las instrucciones ni de las respuestas.

## 6. Responsabilidad académica

La dirección del trabajo permaneció bajo responsabilidad de las personas integrantes del equipo, quienes definieron los objetivos, aportaron la información, seleccionaron alternativas, autorizaron operaciones, revisaron resultados y corrigieron el alcance de las afirmaciones. Asimismo, conservaron la responsabilidad por la exactitud, la pertinencia y la citación del contenido final.

La IA no reemplazó la verificación experimental. Las propuestas de comandos, explicaciones, código o pruebas solo adquieren valor probatorio cuando son revisadas y, cuando corresponde, ejecutadas y registradas por una persona en un entorno identificable. Ningún texto generado convierte por sí mismo una simulación, una prueba de software o un valor nominal en una medición física.

## 7. Trazabilidad documental y reproducibilidad de la revisión

| Referencia relativa al repositorio | Contenido pertinente | Función documental y limitación |
|---|---|---|
| `README.md` | Estado resumido, contrato de adquisición y navegación | Sirve para orientación; conserva afirmaciones de una etapa histórica. |
| `contexto_chat_proyecto_esp32c6.txt` | Resumen de temas, decisiones, observaciones y preferencias | No es transcripción literal completa; debe presentarse como resumen. |
| `docs/informe-proyecto-ia.md` | Arquitectura, evidencia histórica, cronología y declaración `gpt-6-sol` | Fuente documental secundaria; los artefactos primarios citados no están en este checkout. |
| `docs/vitacora-general.md` | Decisiones y separación entre software y hardware | Refleja un estado anterior al build y a las observaciones físicas posteriores. |
| `docs/arquitectura.md` | Componentes, PSC1, pines, ventanas y límites | Sustenta la estructura técnica documentada, no una medición física completa. |
| `docs/requisitos-y-verificacion.md` | Matriz de requisitos, pruebas y pendientes | Permite trazar criterios y distinguir software de hardware. |
| `docs/raspberry-pi.md` | Lector, servicios, despliegue y comprobaciones SBC | Describe procedimiento; la mera existencia de la guía no acredita ejecución. |
| `odd/tasks/osciloscopio-hibrido.md` | Evolución del trabajo, build/flash y pendientes | Registro operativo documental; faltan aquí logs y binarios primarios. |
| `odd/tasks/informe-proyecto-ia.md` | Alcance y comprobaciones del informe anterior | Explicita que la documentación pasiva no sustituyó pruebas físicas. |
| `odd/tasks/declaraciones-academicas.md` | Alcance, autorización, tareas y evidencia de estas declaraciones | Permite revisar cómo se preparó y verificó la entrega actual. |
| `docs/declaracion-participacion-y-aportes.md` | Atribución conjunta autorizada y límites | Declaración directa del grupo, no inferencia basada en commits. |

La reproducibilidad de la revisión documental exige conservar la estructura relativa de estos archivos y contrastar cada afirmación con el nivel de evidencia correspondiente. La repetición de pruebas de software o hardware requiere, además, los repositorios fuente, las versiones, los registros, los binarios, las conexiones y los equipos que no forman parte de esta copia local.
