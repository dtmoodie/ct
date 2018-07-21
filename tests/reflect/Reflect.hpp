#pragma once
#include "Data.hpp"

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

#if __cplusplus <= 201103
    REFLECT_BEGIN(InternallyReflected)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
    REFLECT_END;
#endif
    
}
