import pytest


class TestStringLen:
    def test_str_len(self):
        assert len("asdf") == 4

    def test_str_len_function(self):
        def get_string():
            return "asdf"
        assert len(get_string()) == 4


class TestStringSearch:
    def test_find_first(self):
        s = "asdf:asdf"
        assert s.find(":") == 4
        assert s.find("a") == 0
        assert s.find("f") == 3

    def test_find_last(self):
        s = "asdf:asdf"
        assert s.rfind(":") == 4
        assert s.rfind("a") == 5

    def test_count(self):
        s = "asdfasdf"
        assert s.count("a") == 2
        assert s.count("g") == 0

    def test_find_nonexistent(self):
        s = "asdfasdf"
        assert s.find("bgr") == -1
        assert "bgr".find("asdfasdf") == -1


class TestStringEquality:
    def test_case_sensitive(self):
        assert "asdf" == "asdf"
        assert "asdf" != "Asdf"
        assert "asdf" != "asdff"
        assert "asdfg" != "asdff"

    def test_case_insensitive(self):
        assert "asdf".lower() == "ASDF".lower()
        assert "asdf".lower() == "Asdf".lower()


class TestStringSlicing:
    str1 = "asdfasdfasdf"

    def test_substr(self):
        assert self.str1[0:4] == "asdf"
        assert self.str1[1:5] == "sdfa"
        assert self.str1[-2:] == "df"

    def test_slice(self):
        assert self.str1[0:4] == "asdf"
        assert self.str1[1:4] == "sdf"
        assert self.str1[-5:-1] == "fasd"
        assert self.str1[-5:] == "fasdf"

    def test_slice_invalid(self):
        with pytest.raises(ValueError):
            self.str1[-5:-6]


class TestStringConversion:
    def test_to_integer(self):
        assert int("54321") == 54321

    def test_to_integer_invalid(self):
        with pytest.raises(ValueError):
            int("asdf")


class TestStringStrip:
    def test_strip(self):
        assert "  asdf  ".strip() == "asdf"

    def test_strip_keeps_inner(self):
        assert "  asdf  asdf  ".strip() == "asdf  asdf"


class TestStringSubstring:
    def test_find_substring(self):
        assert "asdfadfa".find("fadf") == 3

    def test_rfind_substring(self):
        assert "asdfadfa".rfind("fadf") == 3


class TestStringHash:
    def test_hash_consistency(self):
        str1 = "asdfasdfasdf"
        assert hash(str1[0:4]) == hash("asdf")
        assert hash(str1[1:5]) == hash("sdfa")

    def test_crc32(self):
        import zlib
        assert zlib.crc32(b"asdf") != 0
        assert zlib.crc32(b"asdfasdf"[0:4]) == zlib.crc32(b"asdf")
        assert zlib.crc32(b"asdfasdf"[4:8]) == zlib.crc32(b"asdf")


class TestStringCompareIgnoreCase:
    def test_compare_char_ignore_case(self):
        assert "a".lower() == "A".lower()

    def test_string_equal_ignore_case(self):
        assert "asdf".upper() == "ASDF".upper()
        assert "asdf".lower() == "ASDF".lower()


class TestFindFirstLastChar:
    def test_find_first_char(self):
        s = "  asdf  "
        stripped = s.strip()
        assert s.find(stripped[0]) == 2

    def test_find_last_char(self):
        s = "  asdf  "
        stripped = s.strip()
        assert s.rfind(stripped[-1]) == 5


class TestStringViewNpos:
    def test_find_npos(self):
        assert "bgr".find("asdfasdf") == -1
