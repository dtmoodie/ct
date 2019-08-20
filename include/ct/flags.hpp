#ifndef CT_FLAGS_HPP
#define CT_FLAGS_HPP
#include "EnumBitset.hpp"
#include "enum.hpp"
#include "types.hpp"

namespace ct
{
    BITSET_START(Flags)
        ENUM_VALUE(DO_NOT_SERIALIZE, 0)
        ENUM_VALUE(READABLE, DO_NOT_SERIALIZE + 1)
        ENUM_VALUE(WRITABLE, READABLE + 1)
        ENUM_VALUE(INVOKABLE, WRITABLE + 1)
        ENUM_VALUE(REQUIRED, INVOKABLE + 1)
        ENUM_VALUE(COMPILE_TIME_CONSTANT, REQUIRED + 1)
        ENUM_VALUE(CT_RESERVED_FLAG_BITS, 8)
    ENUM_END;
}
#endif // CT_FLAGS_HPP
