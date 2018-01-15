#pragma once
#include "detail/HashImpl.hpp"

namespace ct
{
    template <size_t len>
    constexpr uint32_t ctcrc32(const char(&str)[len])
    {
        return detail::crc32<len - 2>(str) ^ 0xFFFFFFFF;
    }

    template <size_t len>
    constexpr uint32_t ctcrc32_ignore_whitespace(const char(&str)[len])
    {
        return detail::crc32_ignore_whitespace<len - 2>(str) ^ 0xFFFFFFFF;
    }

    constexpr uint32_t ctcrc32Range(const char* str, size_t end)
    {
        return end < 2 ? 0 : detail::ctcrc32(str, end - 1) ^ 0xFFFFFFFF;
    }

    constexpr uint32_t ctcrc32Range(const char* str, size_t begin, size_t len)
    {
        return len < 2 ? 0 : detail::ctcrc32(str + begin, len - 1) ^ 0xFFFFFFFF;
    }
    template<class T>
    constexpr T combineHash(T seed, T hash)
    {
        return seed ^ hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}
