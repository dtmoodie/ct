#pragma once

namespace ct
{
    namespace detail
    {
        constexpr size_t findFirst(const char* str, size_t N, size_t idx, char del)
        {
            return idx == N ? N : str[idx] == del ? idx : findFirst(str, N, idx + 1, del);
        }


        template<size_t idx, size_t N>
        constexpr size_t findFirst(const char* str, char del)
        {
            return str[idx] == del ? idx : idx == N - 1 ? N : findFirst<idx + 1, N>(str, del);    
        }
        

        // len needs to be length of string - 1
        constexpr size_t ctFindDeliminator(const char* str, size_t len)
        {
            return str[len] == ':' ? len : len == 0 ? 0 : ctFindDeliminator(str, len - 1);
        }

        constexpr size_t ctFindDeliminator(const char* str, size_t len, char del)
        {
            return str[len] == del ? len : len == 0 ? 0 : ctFindDeliminator(str, len - 1, del);
        }

        constexpr size_t findLast(const char* str, size_t len, char Del)
        {
            return str[len] == Del ? 
                len : 
                len == 0 ? 
                    0 : 
                    findLast(str, len - 1, Del);
        }
    }
}