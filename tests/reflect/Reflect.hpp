#pragma once
#include "Data.hpp"
#include <ct/reflect.hpp>

namespace ct
{
    REFLECT_BEGIN(MyEnum)
        ENUM(k1234)
        ENUM(k2345)
        ENUM(k3456)
    REFLECT_END;

    REFLECT_BEGIN(WithEnum)
        PUBLIC_ACCESS(m_enum)
    REFLECT_END;

    REFLECT_BEGIN(ReflectedStruct)
        // This is a work in progress for some reason ... causes an error in hashing of structs
        // MEMBER_FUNCTION(description, &ReflectedStruct::description)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
        PUBLIC_ACCESS(id)
    REFLECT_END;

    REFLECT_BEGIN(Composite)
        PUBLIC_ACCESS(a)
        PUBLIC_ACCESS(b)
    REFLECT_END;

    REFLECT_BEGIN(TestA)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
        MEMBER_FUNCTION(norm, &TestA::norm)
        MEMBER_FUNCTION(mul, constFunctionCast<TestA, float>(&TestA::mul))
        MEMBER_FUNCTION(mul, constFunctionCast<TestA, float, float>(&TestA::mul))
        MEMBER_FUNCTION(mul, constFunctionCast<TestA, int>(&TestA::mul))
        STATIC_FUNCTION(create, &TestA::create)
    REFLECT_END;

    REFLECT_TEMPLATED_BEGIN(Templated)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
    REFLECT_END;

    REFLECT_BEGIN(TestB)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
    REFLECT_END;

    REFLECT_BEGIN(TestC)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(z)
    REFLECT_END;

    REFLECT_DERIVED(Inherited, ReflectedStruct)
        PUBLIC_ACCESS(w)
    REFLECT_END;

    REFLECT_DERIVED(MultipleInheritance, ReflectedStruct, PrivateMutableAccess)
        PUBLIC_ACCESS(asdf)
    REFLECT_END;

    REFLECT_BEGIN(TestVec)
        PUBLIC_ACCESS(vec)
    REFLECT_END;

    REFLECT_BEGIN(PrivateMutableAccess)
        PROPERTY(private_mutable_property, &DataType::getX, &DataType::mutateX)
    REFLECT_END;

    REFLECT_BEGIN(PrivateGetAndSet)
        PROPERTY(private_get_and_set, &DataType::getX, &DataType::setX)
    REFLECT_END;

    REFLECT_BEGIN(PointerOwner)
        PUBLIC_ACCESS(ptr)
    REFLECT_END;

    REFLECT_BEGIN(WeirdWeakOwnerShip)
        PUBLIC_ACCESS(struct_vec)
        PUBLIC_ACCESS(pointer_owners)
    REFLECT_END;

    REFLECT_BEGIN(ExplicitThisProperty)
        PROPERTY(prop, &getExplicitThisProperty, &setExplicitThisProperty)
    REFLECT_END;

    REFLECT_BEGIN(::Base)
        PUBLIC_ACCESS(base_x)
        PUBLIC_ACCESS(base_y)
        PUBLIC_ACCESS(base_z)
    REFLECT_END;

    REFLECT_DERIVED(DerivedA, ::Base)
        PUBLIC_ACCESS(derived_a)
    REFLECT_END;

    REFLECT_DERIVED(DerivedB, ::Base)
        PUBLIC_ACCESS(derived_b)
    REFLECT_END;

    REFLECT_DERIVED(DerivedC, DerivedA, DerivedB)
        PUBLIC_ACCESS(derived_c)
    REFLECT_END;
} // namespace ct
