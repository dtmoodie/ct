#pragma once
#include "StringImpl.hpp"
#include <cstddef>
#include <type_traits>
#include <array>


namespace ct
{
    template<size_t N>
    constexpr size_t strLen(const char(&str)[N]){
        return N;
    }

    constexpr size_t strLen(const char* str, size_t len = 0){
        return *str ? strLen(str + 1, len + 1) : len;
    }

    template<size_t N>
    constexpr size_t findFirst(const char(&str)[N], char character = ':'){
        return detail::findFirst(str, N, 0, character);
    }

    constexpr size_t findFirst(const char* str, char character = ':', size_t pos = 0){
        return detail::findFirst(str, strLen(str), 0, character);
    }

    constexpr size_t countOccurances(const char* str, char character = ':', size_t count = 0){
        return *str ? countOccurances(str + 1, character,  *str == character ? count + 1 : count) : count;
    }

    template<std::size_t N>
    constexpr std::size_t findLast(const char(&str)[N], char character = ':'){
        return detail::ctFindDeliminator(str, N - 1, character);
    }

    constexpr std::size_t findLast(const char* str, char character = ':')
    {
        return detail::ctFindDeliminator(str, ct::strLen(str) - 1, character);
    }

    template<std::size_t Tlen>
    constexpr std::size_t classNameIdx(const char(&str)[Tlen]){
        return detail::ctFindDeliminator(str, Tlen - 1) - 2;
    }

    
    constexpr std::size_t classNameIdx(const char* str) {
#ifdef _MSC_VER
        return detail::ctFindDeliminator(str, strLen(str) - 1) - 1;
#else
        return detail::ctFindDeliminator(str, strLen(str) - 1) - 1;
#endif
    }

    // https://stackoverflow.com/questions/25195176/how-do-i-convert-a-c-string-to-a-int-at-compile-time
    constexpr bool isDigit(char c) {
        return c <= '9' && c >= '0';
    }

    constexpr int stoiImplRange(const char* str, const char* end, int value = 0){
        return str != end ?
            isDigit(*str) ?
            stoiImplRange(str + 1, end, (*str - '0') + value * 10)
            : throw "compile-time-error: not a digit"
            : value;
    }

    constexpr int stoiImpl(const char* str, int value = 0) {
        return *str ?
            isDigit(*str) ?
            stoiImpl(str + 1, (*str - '0') + value * 10)
            : throw "compile-time-error: not a digit"
            : value;
    }

    constexpr int stoi(const char* str) {
        return stoiImpl(str);
    }

    constexpr int stoiRange(const char* start, const char* end){
        return stoiImplRange(start, end);
    }

    constexpr int stoiRange(const char* start, int len) {
        return stoiImplRange(start, start + len);
    }
} // namespace ct
