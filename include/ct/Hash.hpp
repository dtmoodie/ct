#pragma once
#include "detail/HashImpl.hpp"

namespace ct
{
    template <size_t len>
    constexpr uint32_t ctcrc32(const char(&str)[len])
    {
        return detail::crc32<len - 2>(str) ^ 0xFFFFFFFF;
    }

    constexpr uint32_t ctcrc32Range(const char* str, size_t len)
    {
        return len < 2 ? 0 : detail::ctcrc32(str, len - 2) ^ 0xFFFFFFFF;
    }
}