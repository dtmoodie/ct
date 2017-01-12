#pragma once
#include "detail/StringImpl.hpp"
#include "Hash.hpp"
namespace ct
{
    template<size_t N>
    constexpr size_t strLen(const char(&str)[N])
    {
        return N;
    }

    template<size_t N>
    constexpr size_t findFirst(const char(&str)[N], char character = ':')
    {
        return detail::findFirst(str, N, 0, character);
    }

    template<size_t N>
    constexpr size_t findLast(const char(&str)[N], char character = ':')
    {
        return detail::ctFindDeliminator(str, N - 1, character);
    }

    template<size_t Tlen>
    constexpr size_t classNameIdx(const char(&str)[Tlen])
    {
        return detail::ctFindDeliminator(str, Tlen - 1);
    }
    
    template<size_t Tlen>
    constexpr uint32_t hashClassName(const char(&str)[Tlen])
    {
        return ctcrc32Range(str, classNameIdx(str));
    }
#define DECLARE_CLASS_HASH \
static constexpr uint32_t getHash() {return hashClassName(__FUNCTION__);} \
enum : uint32_t {hash = getHash()};

#define DECLARE_MODULE_HASH(N) \
static constexpr uint32_t getHash() {return hashClassName(__FUNCTION__) ^ N;} \
enum : uint32_t {hash = getHash()};

} // namespace ct
