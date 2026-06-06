import pytest


class TestEnumerate:
    def test_my_enum_values(self, bindings):
        assert hasattr(bindings.MyClass, "MyEnum")
        assert int(bindings.MyClass.MyEnum.kVALUE0) == 1
        assert int(bindings.MyClass.MyEnum.kVALUE1) == 2
        assert int(bindings.MyClass.MyEnum.kVALUE2) == 3
        assert int(bindings.MyClass.MyEnum.kVALUE3) == 4

    def test_second_enum_values(self, bindings):
        assert int(bindings.MyClass.SecondEnum.kBGR) == 0
        assert int(bindings.MyClass.SecondEnum.kRGB) == 1
        assert int(bindings.MyClass.SecondEnum.kHSV) == 2
        assert int(bindings.MyClass.SecondEnum.kYUV) == 3
        assert int(bindings.MyClass.SecondEnum.kHSL) == 4

    def test_standard_enum_values(self, bindings):
        assert int(bindings.MyClass.StandardEnum.k0) == 0
        assert int(bindings.MyClass.StandardEnum.k1) == 1
        assert int(bindings.MyClass.StandardEnum.k2) == 2
        assert int(bindings.MyClass.StandardEnum.k3) == 3

    def test_standard_enum2_values(self, bindings):
        assert int(bindings.MyClass.StandardEnum2.kASDF) == 0
        assert int(bindings.MyClass.StandardEnum2.k1234) == 1


class TestEnumGlobal:
    def test_global_my_enum_values(self, bindings):
        assert hasattr(bindings, "MyEnum")
        assert int(bindings.MyEnum.k1234) == 0
        assert int(bindings.MyEnum.k2345) == 1
        assert int(bindings.MyEnum.k3456) == 2


class TestEnumToString:
    def test_my_enum_repr(self, bindings):
        val = bindings.MyClass.MyEnum.kVALUE0
        assert repr(val) is not None

    def test_standard_enum_repr(self, bindings):
        val = bindings.MyClass.StandardEnum.k0
        assert repr(val) is not None


class TestEnumInStruct:
    def test_with_enum_default(self, bindings):
        obj = bindings.WithEnum()
        assert hasattr(obj, "m_enum")

    def test_with_enum_assign_global(self, bindings):
        obj = bindings.WithEnum()
        obj.m_enum = bindings.MyEnum.k2345
        assert int(obj.m_enum) == 1

    def test_with_enum_assign_class_enum(self, bindings):
        obj = bindings.WithEnum()
        obj.m_enum = bindings.MyEnum.k2345
        assert obj.m_enum == bindings.MyEnum.k2345
        assert int(obj.m_enum) == 1


class TestEnumBitwise:
    def test_bitset_enum_values(self, bindings):
        assert hasattr(bindings.MyClass, "BitwiseEnum")
        assert int(bindings.MyClass.BitwiseEnum.kVALUE0) == 1
        assert int(bindings.MyClass.BitwiseEnum.kVALUE1) == 2
        assert int(bindings.MyClass.BitwiseEnum.kVALUE2) == 4

    def test_extended_bitset_values(self, bindings):
        assert hasattr(bindings.MyClass, "ExtendedEnum")
        assert int(bindings.MyClass.ExtendedEnum.kVALUE3) == 8
        assert int(bindings.MyClass.ExtendedEnum.kVALUE4) == 16

    def test_bitset_values(self, bindings):
        assert hasattr(bindings.MyClass, "Bitset")
        assert int(bindings.MyClass.Bitset.v0) == 0
        assert int(bindings.MyClass.Bitset.v1) == 1
        assert int(bindings.MyClass.Bitset.v2) == 2
        assert int(bindings.MyClass.Bitset.v3) == 3
        assert int(bindings.MyClass.Bitset.v4) == 4
        assert int(bindings.MyClass.Bitset.v5) == 5

    def test_mixed_bitwise_values(self, bindings):
        assert hasattr(bindings.MyClass, "MixedBitwise")
        assert int(bindings.MyClass.MixedBitwise.kVALUE0) == 1
        assert int(bindings.MyClass.MixedBitwise.kVALUE1) == 2
        assert int(bindings.MyClass.MixedBitwise.kVALUE2) == 3
        assert int(bindings.MyClass.MixedBitwise.kVALUE3) == 4
        assert int(bindings.MyClass.MixedBitwise.kVALUE4) == 5
        assert int(bindings.MyClass.MixedBitwise.kVALUE5) == 6
        assert int(bindings.MyClass.MixedBitwise.kVALUE6) == 7
        assert int(bindings.MyClass.MixedBitwise.kFLAG0) == 16
        assert int(bindings.MyClass.MixedBitwise.kFLAG1) == 32
        assert int(bindings.MyClass.MixedBitwise.kFLAG2) == 64
