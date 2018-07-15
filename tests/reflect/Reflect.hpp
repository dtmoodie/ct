#pragma once
#include "Data.hpp"


namespace ct
{
    namespace reflect
    {
        REFLECT_DATA_START(ReflectedStruct)
            REFLECT_DATA_MEMBER(x)
            REFLECT_DATA_MEMBER(y)
            REFLECT_DATA_MEMBER(z)
            REFLECT_DATA_MEMBER(id)
        REFLECT_DATA_END;

        REFLECT_DATA_START(Composite)
            REFLECT_DATA_MEMBER(a)
            REFLECT_DATA_MEMBER(b)
        REFLECT_DATA_END;

        REFLECT_DATA_START(TestA)
            REFLECT_DATA_MEMBER(x)
            REFLECT_DATA_MEMBER(y)
            REFLECT_DATA_MEMBER(z)
        REFLECT_DATA_END;

        REFLECT_DATA_START(TestB)
            REFLECT_DATA_MEMBER(x)
            REFLECT_DATA_MEMBER(y)
            REFLECT_DATA_MEMBER(z)
        REFLECT_DATA_END;

        REFLECT_DATA_START(TestC)
            REFLECT_DATA_MEMBER(y)
            REFLECT_DATA_MEMBER(x)
            REFLECT_DATA_MEMBER(z)
        REFLECT_DATA_END;

        REFLECT_DATA_DERIVED(Inherited, ReflectedStruct)
            REFLECT_DATA_MEMBER(w)
        REFLECT_DATA_END;

        REFLECT_DATA_START(TestVec)
            REFLECT_DATA_MEMBER(vec)
        REFLECT_DATA_END;
#if __cplusplus <= 201103
        REFLECT_DATA_START(InternallyReflected)
            REFLECT_DATA_MEMBER(x)
            REFLECT_DATA_MEMBER(y)
            REFLECT_DATA_MEMBER(z)
        REFLECT_DATA_END;
#endif
    }
}
