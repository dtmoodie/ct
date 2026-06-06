import pytest


class TestReflectedStructRepr:
    def test_repr_contains_fields(self, bindings):
        obj = bindings.ReflectedStruct(x=0.0, y=1.0, z=2.0, id=3)
        r = repr(obj)
        assert "x" in r
        assert "y" in r
        assert "z" in r
        assert "id" in r

    def test_repr_contains_values(self, bindings):
        obj = bindings.ReflectedStruct(x=0.0, y=1.0, z=2.0, id=3)
        r = repr(obj)
        assert "0" in r
        assert "1" in r
        assert "2" in r
        assert "3" in r

    def test_repr_type(self, bindings):
        obj = bindings.ReflectedStruct(x=0.0, y=1.0, z=2.0, id=3)
        r = repr(obj)
        assert isinstance(r, str)
        assert len(r) > 0


class TestTestARepr:
    def test_repr_contains_fields(self, bindings):
        obj = bindings.TestA(x=0.0, y=1.0, z=2.0)
        r = repr(obj)
        assert "x" in r
        assert "y" in r
        assert "z" in r

    def test_repr_contains_values(self, bindings):
        obj = bindings.TestA(x=0.0, y=1.0, z=2.0)
        r = repr(obj)
        assert "0" in r
        assert "1" in r
        assert "2" in r


class TestTestBRepr:
    def test_repr_contains_fields(self, bindings):
        obj = bindings.TestB(x=0.0, y=1.0, z=2.0)
        r = repr(obj)
        assert "x" in r


class TestTestCRepr:
    def test_repr_contains_fields(self, bindings):
        obj = bindings.TestC(y=0.0, x=1.0, z=2.0)
        r = repr(obj)
        assert "y" in r
        assert "x" in r
        assert "z" in r


class TestInheritedRepr:
    def test_repr_contains_all_fields(self, bindings):
        obj = bindings.Inherited()
        obj.x = 0.0
        obj.y = 1.0
        obj.z = 2.0
        obj.id = 3
        obj.w = 4.0
        r = repr(obj)
        assert "x" in r
        assert "y" in r
        assert "z" in r
        assert "id" in r
        assert "w" in r


class TestCompositeRepr:
    def test_repr_contains_nested(self, bindings):
        a = bindings.ReflectedStruct(x=0.0, y=1.0, z=2.0, id=3)
        b = bindings.ReflectedStruct(x=4.0, y=5.0, z=6.0, id=7)
        obj = bindings.Composite(a=a, b=b)
        r = repr(obj)
        assert "a" in r
        assert "b" in r


class TestDerivedRepr:
    def test_derived_a_repr(self, bindings):
        obj = bindings.DerivedA()
        obj.base_x = 0.0
        obj.base_y = 1.0
        obj.base_z = 2.0
        obj.derived_a = 3.0
        r = repr(obj)
        assert "base_x" in r
        assert "base_y" in r
        assert "base_z" in r
        assert "derived_a" in r

    def test_derived_c_repr(self, bindings):
        obj = bindings.DerivedC()
        obj.base_x = 0.0
        obj.base_y = 1.0
        obj.base_z = 2.0
        obj.derived_a = 3.0
        obj.derived_b = 4.0
        obj.derived_c = 5.0
        r = repr(obj)
        assert "base_x" in r
        assert "derived_a" in r
        assert "derived_b" in r
        assert "derived_c" in r


class TestPrivatePropertyRepr:
    def test_private_mutable_repr(self, bindings):
        obj = bindings.PrivateMutableAccess(4)
        r = repr(obj)
        assert "private_mutable_property" in r

    def test_private_get_set_repr(self, bindings):
        obj = bindings.PrivateGetAndSet(5.2)
        r = repr(obj)
        assert "private_get_and_set" in r


class TestInternallyReflectedRepr:
    def test_repr_contains_fields(self, bindings):
        obj = bindings.InternallyReflected(x=5.0, y=10.0, z=15.0)
        r = repr(obj)
        assert "x" in r
        assert "y" in r
        assert "z" in r
        assert "5" in r
        assert "10" in r
        assert "15" in r


class TestMultipleInheritanceRepr:
    def test_repr_contains_all_fields(self, bindings):
        obj = bindings.MultipleInheritance()
        obj.asdf = 5.0
        r = repr(obj)
        assert "private_mutable_property" in r
        assert "asdf" in r


class TestExplicitThisPropertyRepr:
    def test_repr_contains_prop(self, bindings):
        obj = bindings.ExplicitThisProperty(prop=0.0)
        r = repr(obj)
        assert "prop" in r


class TestWithEnumRepr:
    def test_repr_contains_enum(self, bindings):
        obj = bindings.WithEnum()
        obj.m_enum = bindings.MyEnum.k2345
        r = repr(obj)
        assert "m_enum" in r
