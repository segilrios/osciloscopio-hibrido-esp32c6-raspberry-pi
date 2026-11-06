"""Pruebas deterministas del contrato PSC1 y del transporte SPI por chunks."""
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "raspberry"))
from lector_spi import SPIReader
from protocolo_spi import (CMD_CONFIG, CMD_GET_WINDOW, NoNewWindow, ProtocolError,
                           STATUS_NO_DATA, command, frame_size, make_response,
                           parse_response)


class FakeSPI:
    def __init__(self, responses=None):
        self.responses = list(responses or [])
        self.transfers = []
        self.closed = False
        self.rate = 1000
        self.n = 64
        self.seq = 0

    def xfer2(self, request):
        request = bytes(request)
        self.transfers.append(request)
        if len(request) == 16:
            if request[5] == CMD_CONFIG:
                self.n = int.from_bytes(request[6:8], "little")
                self.rate = int.from_bytes(request[8:12], "little")
            self.seq += 1
            if not self.responses and not getattr(self, "disable_auto", False):
                self.responses.append(make_response(
                    rate=self.rate, n=self.n, seq=self.seq,
                    data_response=request[5] == CMD_GET_WINDOW))
            return [0] * 16
        if not getattr(self, "pending", b""):
            if not self.responses:
                raise TimeoutError("fake SPI timeout")
            self.pending = self.responses.pop(0)
        # Firmware sends the next contiguous bytes in each independent CS transfer.
        result, self.pending = self.pending[:len(request)], self.pending[len(request):]
        return list(result)

    def close(self):
        self.closed = True


class TestProtocol(unittest.TestCase):
    def test_command_binary_header(self):
        raw = command(CMD_CONFIG, 40000, 1024)
        self.assertEqual(len(raw), 16)
        self.assertEqual(raw[:4], b"PSC0")
        self.assertEqual(int.from_bytes(raw[8:12], "little"), 40000)

    def test_40k_two_channels_and_crc(self):
        a = [i % 4096 for i in range(512)]
        b = [4095 - i for i in range(512)]
        frame = make_response(rate=40000, n=512, a=a, b=b)
        self.assertEqual(len(frame), frame_size(512))
        parsed = parse_response(frame, expected_n=512)
        self.assertEqual(parsed.raw_1, tuple(a))
        self.assertEqual(parsed.raw_2, tuple(b))

    def test_tampered_crc_fails(self):
        data = bytearray(make_response(rate=40000, n=64))
        data[-1] ^= 0x80
        with self.assertRaises(ProtocolError):
            parse_response(bytes(data), expected_n=64)

    def test_no_data(self):
        data = make_response(rate=1000, n=64, status=STATUS_NO_DATA)
        with self.assertRaises(NoNewWindow):
            parse_response(data, expected_n=64)

    def test_no_data_n512_and_n1024_consumes_zero_payload(self):
        for n in (512, 1024):
            data = make_response(rate=1000, n=n, status=STATUS_NO_DATA,
                                  a=[0] * n, b=[0] * n)
            self.assertEqual(len(data), frame_size(n))
            with self.assertRaises(NoNewWindow):
                parse_response(data, expected_n=n)

    def test_no_gpio_dependency_and_ack_rates(self):
        self.assertNotIn("RPi.GPIO", sys.modules)
        for rate in (100, 1000, 10000, 40000):
            spi = FakeSPI()
            reader = SPIReader(spi=spi, sleeper=lambda _: None)
            ack = reader.apply_config(rate, 64)
            self.assertEqual((ack.sample_rate_hz, ack.window_size), (rate, 64))

    def test_response_chunking_n64_512_1024_and_pacing(self):
        for n, expected_chunks in ((64, (276,)), (256, (1044,)),
                                   (512, (2048, 20)),
                                   (1024, (2048, 2048, 20))):
            spi = FakeSPI()
            sleeps = []
            reader = SPIReader(spi=spi, sleeper=sleeps.append, transfer_delay_s=.015)
            reader.apply_config(1000, n)
            spi.transfers.clear()
            sleeps.clear()
            reader.read_window(n)
            self.assertEqual([len(x) for x in spi.transfers], [16, *expected_chunks])
            self.assertEqual(len(sleeps), len(expected_chunks))
            self.assertTrue(all(delay == .015 for delay in sleeps))

    def test_sequence_duplicate_is_not_connected_as_new_data(self):
        spi = FakeSPI()
        reader = SPIReader(spi=spi, sleeper=lambda _: None)
        reader.apply_config(1000, 64)
        reader.read_window(64)
        spi.seq -= 1
        with self.assertRaises(NoNewWindow):
            reader.read_window(64)

    def test_timeout_or_bad_data_closes_transport_and_never_retries_command(self):
        spi = FakeSPI()
        reader = SPIReader(spi=spi, sleeper=lambda _: None)
        reader.apply_config(1000, 512)
        command_count = len([x for x in spi.transfers if len(x) == 16])
        spi.responses.clear()
        spi.disable_auto = True
        with self.assertRaises(TimeoutError):
            reader.read_window(512)
        self.assertTrue(spi.closed)
        self.assertIsNone(reader.confirmed)
        self.assertEqual(command_count + 1,
                         len([x for x in spi.transfers if len(x) == 16]))

    def test_bad_crc_is_protocol_error_and_closes_transport(self):
        spi = FakeSPI()
        reader = SPIReader(spi=spi, sleeper=lambda _: None)
        reader.apply_config(1000, 64)
        spi.responses.clear()
        bad = bytearray(make_response(rate=1000, n=64))
        bad[-1] ^= 1
        spi.responses.append(bytes(bad))
        with self.assertRaises(ProtocolError):
            reader.read_window(64)
        self.assertTrue(spi.closed)
        self.assertIsNone(reader.confirmed)


if __name__ == "__main__":
    unittest.main()
