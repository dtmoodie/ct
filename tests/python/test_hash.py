import zlib

import pytest


class TestCrc32:
    def test_crc32_nonzero(self):
        assert zlib.crc32(b"asdf") != 0

    def test_crc32_range_beginning(self):
        s = b"asdfasdf"
        assert zlib.crc32(s[:4]) == zlib.crc32(b"asdf")

    def test_crc32_range_middle(self):
        s = b"asdfasdf"
        assert zlib.crc32(s[4:8]) == zlib.crc32(b"asdf")

    def test_crc32_from_string_fn(self):
        def get_string():
            return "asdf"
        assert zlib.crc32(get_string().encode()) == zlib.crc32(b"asdf")

    def test_crc32_ignore_whitespace(self):
        import re
        def crc32_ignore_whitespace(s):
            return zlib.crc32(re.sub(r"\s+", "", s).encode())
        assert crc32_ignore_whitespace("as   df") == zlib.crc32(b"asdf")

    def test_crc32_long_string(self):
        s = b"asdfkjlaksjdfkjaslkdfjlkasjfklaslkfjsakdjflkasdfj"
        assert zlib.crc32(s) != 0

    def test_crc32_consistency(self):
        a = b"hello world"
        b = b"hello world"
        assert zlib.crc32(a) == zlib.crc32(b)
