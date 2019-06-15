#ifndef CT_TYPES_HPP
#define CT_TYPES_HPP

#include <cstdint>
namespace ct
{
    using Flag_t = uint64_t;
    using index_t = int16_t;

    enum Flags : Flag_t
    {
        NONE = 0,
        DO_NOT_SERIALIZE = 1,
        READABLE = DO_NOT_SERIALIZE << 1,
        WRITABLE = READABLE << 1,
        INVOKABLE = WRITABLE << 1,
        REQUIRED = INVOKABLE << 1,
        COMPILE_TIME_CONSTANT = REQUIRED << 1,

        // reserve the first 8 bits of the flag field for ct flags
        CT_RESERVED_FLAG_BITS = 8
    };
}

#endif // CT_TYPES_HPP
