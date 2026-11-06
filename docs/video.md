# Guion y checklist para vídeo de evidencia

El vídeo debe ser una grabación humana del montaje real. Este archivo no es un vídeo ni sustituye sus logs. No se debe grabar ni publicar información sensible.

## Preparación segura

- Usar una mesa despejada, alimentación de 3,3 V adecuada y protección contra cortocircuitos.
- Revisar GND común, niveles lógicos, orientación de cables y pinout antes de energizar.
- Ocultar SSID, contraseñas, tokens, claves, números de serie innecesarios, direcciones privadas y datos personales.
- No mostrar terminales con credenciales ni ejecutar comandos remotos durante la toma.
- Preparar una captura de pantalla o pizarra con fecha, commit/hash disponible y configuración; no escribir valores que no se hayan medido.

## Lista concreta de tomas

1. **Identificación:** plano general del montaje y una tarjeta que indique ESP32-C6, Raspberry Pi, configuración de prueba y fecha.
2. **Seguridad eléctrica:** plano de alimentación apagada, GND común y cables; mostrar el pinout sin exponer información privada.
3. **Firmware:** terminal local mostrando la versión/configuración usada y, si existe, la salida de build o flash. Si no existe el log primario, decirlo en pantalla.
4. **Arranque:** monitor serial del C6 mostrando sólo líneas pertinentes; no simular una salida ausente.
5. **SPI físico:** plano del montaje y del comando ejecutado para el caso real, indicando reloj y `N`; registrar la salida completa en un archivo aparte.
6. **Tres ventanas:** mostrar PING/CONFIG y las tres ventanas de 1 MHz/`N=64` sólo si se están ejecutando o reproduciendo desde un log auténtico aportado por la persona usuaria.
7. **Dashboard:** mostrar la URL local y la ventana recibida, indicando claramente si la fuente es hardware o emulación; no presentar datos simulados como ADC.
8. **Pendientes:** mostrar una tarjeta final que marque 2 MHz/`N=512`, end-to-end y timing ADC como pendientes si no tienen evidencia.
9. **Cierre:** conservar una toma de la configuración, el nombre del log y el checksum si están disponibles, sin inventar placeholders como archivos existentes.

## Procedencia que debe acompañar al vídeo

Registrar junto con la grabación:

- fecha y hora de la toma;
- persona que ejecutó la prueba y entorno general, sin PII innecesaria;
- versión/commit del código y modelo de placa si se conocen;
- comando exacto, reloj SPI, tamaño `N`, número de ventanas y fuente de datos;
- log primario completo, checksum del log y cualquier limitación;
- distinción explícita entre prueba local de software, prueba física comunicada y evidencia end-to-end.

La toma no debe afirmar frecuencia de muestreo ADC, latencia, throughput o tensión si esos valores no fueron medidos y registrados. En particular, una ventana recibida por SPI no verifica el timing del ADC.

## Checklist de aceptación

- [ ] El vídeo muestra hardware real y una fecha/configuración identificable.
- [ ] El montaje y el pinout son visibles sin revelar secretos o PII.
- [ ] Cada resultado tiene comando, configuración y procedencia.
- [ ] Las tres ventanas 1 MHz/`N=64` se marcan como resultado comunicado por usuario si no existe el log primario en el repositorio.
- [ ] No se presenta emulación como adquisición física.
- [ ] Build/flash se etiquetan como documentados cuando faltan sus logs primarios.
- [ ] 2 MHz/`N=512`, end-to-end y temporización ADC quedan pendientes si no se observan.
- [ ] El log y el vídeo se revisan para eliminar secretos/PII antes de compartirlos.
- [ ] Se anota el archivo final sólo cuando exista una grabación real.

**Archivo de vídeo:** `[PENDIENTE: incorporar nombre del archivo auténtico tras grabar y revisar]`
