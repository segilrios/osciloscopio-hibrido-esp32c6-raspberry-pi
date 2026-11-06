"""Lector SPI real para Raspberry Pi Zero 2 W <-> ESP32-C6.

El firmware PSC1 no expone GPIO READY: cada xfer2() es una transaccion CS
independiente. Se envia un comando y despues se leen todos los chunks de la
respuesta conocida, sin muestrear una respuesta corta de forma optimista.
"""
from __future__ import annotations

import time

from protocolo_spi import (CMD_CONFIG, CMD_GET_WINDOW, CMD_PING, RESP_BYTES,
                           NoNewWindow, ProtocolError, command, frame_size,
                           parse_response)


class SPIReader:
    def __init__(self, bus=0, device=0, clock_hz=2_000_000,
                 voltage_fullscale=3.3, timeout_s=2.0, spi=None,
                 transfer_delay_s=0.015, sleeper=time.sleep):
        if not 1 <= clock_hz <= 5_000_000:
            raise ValueError("Probar un reloj entre 1 y 5 MHz; no asumir limite real")
        if not (0 < voltage_fullscale <= 3.3):
            raise ValueError("Escala ADC provisional debe ser <= 3.3 V")
        self.bus, self.device, self.clock_hz = bus, device, clock_hz
        self.timeout = timeout_s
        self.transfer_delay_s = transfer_delay_s
        self._sleep = sleeper
        self.vref = voltage_fullscale
        self.spi = spi
        self._can_reopen = spi is None
        if self.spi is None:
            self.spi = self._open_spi()
        self._configure_spi()
        self.last_esp_seq = -1
        self.confirmed = None
        self.bytes_spi = 0
        self.start = time.monotonic()
        self._closed = False

    def _open_spi(self):
        import spidev
        device = spidev.SpiDev()
        device.open(self.bus, self.device)
        return device

    def _configure_spi(self):
        self.spi.mode = 0
        self.spi.max_speed_hz = self.clock_hz
        self.spi.bits_per_word = 8

    def _pause_between_cs(self):
        if self.transfer_delay_s:
            self._sleep(self.transfer_delay_s)

    def _transfer(self, request: bytes) -> bytes:
        if self._closed:
            raise ProtocolError("SPI cerrado tras desincronizacion; requiere reinicializacion")
        started = time.monotonic()
        result = bytes(self.spi.xfer2(list(request)))
        elapsed = time.monotonic() - started
        if elapsed > self.timeout:
            raise TimeoutError("Timeout durante transferencia SPI")
        if len(result) != len(request):
            raise ProtocolError("SPI devolvio cantidad de bytes diferente")
        self.bytes_spi += len(request)
        return result

    def _invalidate_transport(self):
        self.confirmed = None
        close = getattr(self.spi, "close", None)
        if callable(close):
            close()
        if not self._can_reopen:
            self._closed = True
            return
        try:
            self.spi = self._open_spi()
            self._configure_spi()
            self._closed = False
        except Exception:
            self._closed = True

    def _exchange(self, cmd: bytes, response_n: int) -> bytes:
        try:
            # Nunca repetir cmd despues de enviarlo: el C6 puede seguir esperando
            # los chunks de esta misma respuesta.
            self._transfer(cmd)
            self._pause_between_cs()
            chunks = []
            remaining = response_n
            while remaining:
                chunk = min(2048, remaining)
                chunks.append(self._transfer(bytes(chunk)))
                remaining -= chunk
                if remaining:
                    self._pause_between_cs()
            return b"".join(chunks)
        except (ProtocolError, TimeoutError, OSError):
            self._invalidate_transport()
            raise

    def ping(self):
        return parse_response(self._exchange(command(CMD_PING), RESP_BYTES))

    def apply_config(self, rate, n):
        data = self._exchange(command(CMD_CONFIG, rate, n), RESP_BYTES)
        try:
            ack = parse_response(data)
        except ProtocolError:
            self._invalidate_transport()
            raise
        if ack.sample_rate_hz != rate or ack.window_size != n:
            self._invalidate_transport()
            raise ProtocolError("El ACK del ESP32 no coincide con la configuracion pedida")
        self.confirmed = (rate, n)
        self.last_esp_seq = -1
        return ack

    def read_window(self, n):
        if self.confirmed is None or self.confirmed[1] != n:
            raise ProtocolError("Primero confirmar configuracion por SPI")
        response = self._exchange(command(CMD_GET_WINDOW), frame_size(n))
        try:
            packet = parse_response(response, expected_n=n)
        except ProtocolError:
            self._invalidate_transport()
            raise
        if packet.sample_rate_hz != self.confirmed[0]:
            self._invalidate_transport()
            raise ProtocolError("Fs reportada por uC distinta de la configurada")
        if packet.sequence <= self.last_esp_seq:
            raise NoNewWindow("Ventana ya recibida antes")
        self.last_esp_seq = packet.sequence
        # APROXIMACION SIN CALIBRAR. Cambiar escala por calibracion real de ADC.
        k = self.vref / 4095.0
        return ([round(x * k, 4) for x in packet.raw_1],
                [round(x * k, 4) for x in packet.raw_2])

    def close(self):
        if self._closed:
            return
        close = getattr(self.spi, "close", None)
        if callable(close):
            close()
        self._closed = True
        self.confirmed = None
