#pragma once
#include "StringView.hpp"
#include <utility>
namespace ct{
    constexpr size_t formatStringSize(const char* str, size_t sz = 0);
    constexpr size_t formatArgParseImpl(const char* str, size_t sz = 0){
        return sz + stoiRange(str, findFirst(str, '}') - 1);
    }

    constexpr size_t formatArgParse(const char* str, size_t sz){
        return formatStringSize(str + findFirst(str, '}'), sz + formatArgParseImpl(str));
    }

    constexpr size_t formatStringSize(const char* str, size_t sz){
        return *str == '\0' ? sz : 
                  (*str == '{') ? formatArgParse(str + 1, sz)  : formatStringSize(str + 1, sz + 1);
    }

    template<size_t N>
    constexpr std::array<int, N> specifierSize(const char* str, std::array<int, N> arr = std::array<int, N>(), int idx = 0){
        arr[idx] = stoiRange(str + findFirst(str, '{'), str + findFirst(str, '}') - 1);
        return idx == N - 1 ? arr : specifierSize(str + findFirst(str, '}') + 1, arr, idx + 1);
    }
}