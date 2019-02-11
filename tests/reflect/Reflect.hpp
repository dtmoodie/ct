#pragma once
#include "Data.hpp"
#include <ct/reflect.hpp>

namespace ct
{
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
// GCC 4.8 doesn't like this, can't do overloads this way with it
#ifdef __GNUC__
#if __GNUC__ >= 5
        MEMBER_FUNCTION(mul,
                        static_cast<TestA (TestA::*)(float) const>(&TestA::mul),
                        static_cast<TestA (TestA::*)(float, float) const>(&TestA::mul),
                        static_cast<TestA (TestA::*)(int) const>(&TestA::mul))
#else
MEMBER_FUNCTION(mul, static_cast<TestA (TestA::*)(float) const>(&TestA::mul))
/*MEMBER_FUNCTION(mul, static_cast<TestA (TestA::*)(float, float) const>(&TestA::mul))
MEMBER_FUNCTION(mul, static_cast<TestA (TestA::*)(int) const>(&TestA::mul))*/

#endif
#else

#endif
        STATIC_FUNCTION(create, &TestA::create)
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
        PROPERTY(x, &DataType::getX, &DataType::mutateX)
    REFLECT_END;

    REFLECT_BEGIN(PrivateGetAndSet)
        PROPERTY(x, &DataType::getX, &DataType::setX)
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

    REFLECT_BEGIN(Base)
        PUBLIC_ACCESS(base_x)
        PUBLIC_ACCESS(base_y)
        PUBLIC_ACCESS(base_z)
    REFLECT_END;

    REFLECT_DERIVED(DerivedA, Base)
        PUBLIC_ACCESS(derived_a)
    REFLECT_END;

    REFLECT_DERIVED(DerivedB, Base)
        PUBLIC_ACCESS(derived_b)
    REFLECT_END;

    REFLECT_DERIVED(DerivedC, DerivedA, DerivedB)
        PUBLIC_ACCESS(derived_c)
    REFLECT_END;
}
