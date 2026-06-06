import pytest


class TestReflectedStruct:
    def test_create_and_access(self, bindings):
        obj = bindings.ReflectedStruct(x=1.0, y=2.0, z=3.0, id=10)
        assert obj.x == 1.0
        assert obj.y == 2.0
        assert obj.z == 3.0
        assert obj.id == 10

    def test_default_construct(self, bindings):
        obj = bindings.ReflectedStruct()
        assert hasattr(obj, "x")
        assert hasattr(obj, "y")
        assert hasattr(obj, "z")
        assert hasattr(obj, "id")

    def test_modify_members(self, bindings):
        obj = bindings.ReflectedStruct(x=1.0, y=2.0, z=3.0, id=10)
        obj.x = 100.0
        obj.id = 42
        assert obj.x == 100.0
        assert obj.id == 42


class TestTestA:
    def test_create_and_access(self, bindings):
        obj = bindings.TestA(x=1.0, y=2.0, z=3.0)
        assert obj.x == 1.0
        assert obj.y == 2.0
        assert obj.z == 3.0

    def test_call_methods(self, bindings):
        obj = bindings.TestA(x=3.0, y=4.0, z=0.0)
        n = obj.norm()
        assert n == pytest.approx(5.0, rel=1e-5)

    def test_mul_method(self, bindings):
        obj = bindings.TestA(x=1.0, y=2.0, z=3.0)
        result = obj.mul(2)
        assert result.x == 2.0
        assert result.y == 4.0
        assert result.z == 6.0

    def test_static_create(self, bindings):
        obj = bindings.TestA.create()
        assert obj.x == 0.0
        assert obj.y == 0.0
        assert obj.z == 0.0


class TestComposite:
    def test_nested_struct(self, bindings):
        a = bindings.ReflectedStruct(x=1.0, y=2.0, z=3.0, id=0)
        b = bindings.ReflectedStruct(x=4.0, y=5.0, z=6.0, id=1)
        obj = bindings.Composite(a=a, b=b)
        assert obj.a.x == 1.0
        assert obj.a.y == 2.0
        assert obj.a.z == 3.0
        assert obj.b.x == 4.0
        assert obj.b.y == 5.0
        assert obj.b.z == 6.0

    def test_modify_nested(self, bindings):
        a = bindings.ReflectedStruct(x=1.0, y=2.0, z=3.0, id=0)
        b = bindings.ReflectedStruct(x=4.0, y=5.0, z=6.0, id=1)
        obj = bindings.Composite(a=a, b=b)
        obj.a.x = 10.0
        assert obj.a.x == 10.0


class TestInherited:
    def test_inherited_members(self, bindings):
        obj = bindings.Inherited()
        obj.x = 1.0
        obj.y = 2.0
        obj.z = 3.0
        obj.id = 4
        obj.w = 5.0
        assert obj.x == 1.0
        assert obj.y == 2.0
        assert obj.z == 3.0
        assert obj.id == 4
        assert obj.w == 5.0


class TestDerived:
    def test_derived_a(self, bindings):
        obj = bindings.DerivedA()
        obj.base_x = 0.0
        obj.base_y = 1.0
        obj.base_z = 2.0
        obj.derived_a = 3.0
        assert obj.base_x == 0.0
        assert obj.base_y == 1.0
        assert obj.base_z == 2.0
        assert obj.derived_a == 3.0

    def test_derived_b(self, bindings):
        obj = bindings.DerivedB()
        obj.base_x = 0.0
        obj.base_y = 1.0
        obj.base_z = 2.0
        obj.derived_b = 4.0
        assert obj.derived_b == 4.0

    def test_derived_c(self, bindings):
        obj = bindings.DerivedC()
        obj.base_x = 0.0
        obj.base_y = 1.0
        obj.base_z = 2.0
        obj.derived_a = 3.0
        obj.derived_b = 4.0
        obj.derived_c = 5.0
        assert obj.base_x == 0.0
        assert obj.derived_a == 3.0
        assert obj.derived_b == 4.0
        assert obj.derived_c == 5.0


class TestTestB:
    def test_create_and_access(self, bindings):
        obj = bindings.TestB(x=1.0, y=2.0, z=3.0)
        assert obj.x == 1.0
        assert obj.y == 2.0
        assert obj.z == 3.0


class TestTestC:
    def test_create_and_access(self, bindings):
        obj = bindings.TestC(y=0.0, x=1.0, z=2.0)
        assert obj.y == 0.0
        assert obj.x == 1.0
        assert obj.z == 2.0


class TestTestVec:
    def test_vector_float(self, bindings):
        obj = bindings.TestVec(vec=[0.0, 1.0, 2.0, 3.0, 4.0])
        assert len(obj.vec) == 5
        assert obj.vec[0] == 0.0
        assert obj.vec[3] == 3.0

    def test_modify_vector(self, bindings):
        obj = bindings.TestVec(vec=[0.0, 1.0, 2.0])
        obj.vec.append(3.0)
        assert len(obj.vec) == 4


class TestWithEnum:
    def test_enum_member(self, bindings):
        obj = bindings.WithEnum()
        val = obj.m_enum
        assert val is not None

    def test_enum_assign(self, bindings):
        obj = bindings.WithEnum()
        obj.m_enum = bindings.MyEnum.k2345
        assert int(obj.m_enum) == 1


class TestMultipleInheritance:
    def test_multiple_base_members(self, bindings):
        obj = bindings.MultipleInheritance()
        obj.x = 1.0
        obj.y = 2.0
        obj.z = 3.0
        obj.id = 4
        obj.asdf = 5.0
        assert obj.x == 1.0
        assert obj.y == 2.0
        assert obj.z == 3.0
        assert obj.id == 4
        assert obj.asdf == 5.0


class TestInternallyReflected:
    def test_create_and_access(self, bindings):
        obj = bindings.InternallyReflected(x=5.0, y=10.0, z=15.0)
        assert obj.x == 5.0
        assert obj.y == 10.0
        assert obj.z == 15.0

    def test_call_methods(self, bindings):
        obj = bindings.InternallyReflected(x=1.0, y=2.0, z=3.0)
        result = obj.foo()
        assert result == 0.0

    def test_call_method_with_arg(self, bindings):
        obj = bindings.InternallyReflected(x=1.0, y=2.0, z=3.0)
        result = obj.foo(42.0)
        assert result == 42.0


class TestPrivateMutableAccess:
    def test_create(self, bindings):
        obj = bindings.PrivateMutableAccess(4)
        assert hasattr(obj, "private_mutable_property")

    def test_set_property(self, bindings):
        obj = bindings.PrivateMutableAccess(4)
        obj.private_mutable_property = 10
        assert obj.private_mutable_property == 10


class TestPrivateGetAndSet:
    def test_create(self, bindings):
        obj = bindings.PrivateGetAndSet(5.2)
        assert hasattr(obj, "private_get_and_set")

    def test_set_property(self, bindings):
        obj = bindings.PrivateGetAndSet(5.2)
        obj.private_get_and_set = 10.0
        assert obj.private_get_and_set == pytest.approx(10.0)


class TestExplicitThisProperty:
    def test_create_and_access(self, bindings):
        obj = bindings.ExplicitThisProperty(prop=0.0)
        assert obj.prop == 0.0

    def test_modify(self, bindings):
        obj = bindings.ExplicitThisProperty(prop=1.0)
        obj.prop = 5.0
        assert obj.prop == 5.0


class TestReflectedTypesExist:
    def test_types_are_reflected(self, bindings):
        expected = [
            "ReflectedStruct",
            "Inherited",
            "Composite",
            "TestA",
            "TestB",
            "TestC",
            "TestVec",
            "DerivedA",
            "DerivedB",
            "DerivedC",
            "WithEnum",
            "PrivateMutableAccess",
            "InternallyReflected",
            "PrivateGetAndSet",
            "MultipleInheritance",
            "ExplicitThisProperty",
            "MyEnum",
        ]
        for name in expected:
            assert hasattr(bindings, name), f"Missing type: {name}"

    def test_myclass_nested_types(self, bindings):
        expected = [
            "MyEnum",
            "SecondEnum",
            "BitwiseEnum",
            "ExtendedEnum",
            "Bitset",
            "MixedBitwise",
            "StandardEnum",
            "StandardEnum2",
        ]
        for name in expected:
            assert hasattr(bindings.MyClass, name), f"Missing nested type: {name}"


class TestVectorOfReflectedStruct:
    def test_create_and_index(self, bindings):
        vec = bindings["std::vector<ReflectedStruct>"]()
        assert len(vec) == 0

    def test_append_and_access(self, bindings):
        vec = bindings["std::vector<ReflectedStruct>"]()
        a = bindings.ReflectedStruct(x=1.0, y=2.0, z=3.0, id=0)
        b = bindings.ReflectedStruct(x=4.0, y=5.0, z=6.0, id=1)
        vec.append(a)
        vec.append(b)
        assert len(vec) == 2
        assert vec[0].x == 1.0
        assert vec[1].id == 1
