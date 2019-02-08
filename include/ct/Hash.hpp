#pragma once
#include "HashImpl.hpp"
#include "String.hpp"

namespace ct
{
    template <std::size_t len>
    constexpr uint32_t crc32(const char (&str)[len])
    {
        return detail::crc32<len>(str) ^ 0xFFFFFFFF;
    }

    constexpr uint32_t crc32(const char* str) { return detail::crc32(str) ^ 0xFFFFFFFF; }

    template <std::size_t len>
    constexpr uint32_t crc32_ignore_whitespace(const char (&str)[len])
    {
        return detail::crc32_ignore_whitespace<len - 2>(str) ^ 0xFFFFFFFF;
    }

    constexpr uint32_t crc32Range(const char* str, std::size_t end)
    {
        return end < 2 ? 0 : detail::crc32(str, end - 1) ^ 0xFFFFFFFF;
    }

    constexpr uint32_t crc32Range(const char* str, std::size_t begin, std::size_t len)
    {
        return len < 2 ? 0 : detail::crc32(str + begin, len - 1) ^ 0xFFFFFFFF;
    }

    constexpr uint32_t crc32(const StringView view) { return crc32Range(view.data(), view.size()); }

    template <class T>
    constexpr T combineHash(T seed, T hash)
    {
        return (seed ^ hash) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}
