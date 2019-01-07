#pragma once
#include "Data.hpp"
#include <ct/reflect.hpp>

namespace ct
{
    REFLECT_BEGIN(ReflectedStruct)
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

    REFLECT_BEGIN(TestVec)
        PUBLIC_ACCESS(vec)
    REFLECT_END;

    REFLECT_BEGIN(PrivateMutableAccess)
        ACCESSOR("x", &DataType::getX, &DataType::mutateX)
    REFLECT_END;

    REFLECT_BEGIN(PrivateGetAndSet)
        ACCESSOR("x", &DataType::getX, &DataType::setX)
    REFLECT_END;

#if __cplusplus <= 201103
    REFLECT_BEGIN(InternallyReflected)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
    REFLECT_END;
#endif

    REFLECT_BEGIN(PointerOwner)
        PUBLIC_ACCESS(ptr)
    REFLECT_END;

    REFLECT_BEGIN(WeirdWeakOwnerShip)
        PUBLIC_ACCESS(struct_vec)
        PUBLIC_ACCESS(pointer_owners)
    REFLECT_END;
}
