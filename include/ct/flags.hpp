#ifndef CT_FLAGS_HPP
#define CT_FLAGS_HPP
#include "EnumBitset.hpp"
#include "enum.hpp"
#include "reflect_macros.hpp"
#include "types.hpp"

namespace ct
{
    BITSET_BEGIN(Flags)
        ENUM_BITVALUE(DO_NOT_SERIALIZE, 0)
        ENUM_BITVALUE(READABLE, 1)
        ENUM_BITVALUE(WRITABLE, 2)
        ENUM_BITVALUE(INVOKABLE, 3)
        ENUM_BITVALUE(REQUIRED, 4)
        ENUM_BITVALUE(COMPILE_TIME_CONSTANT, 5)
        ENUM_BITVALUE(DO_NOT_PRINT, 6)
        ENUM_BITVALUE(CT_RESERVED_FLAG_BITS, 8)
    ENUM_END;
} // namespace ct
#endif // CT_FLAGS_HPP
