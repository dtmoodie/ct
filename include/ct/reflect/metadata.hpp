#ifndef CT_REFLECT_METADATA_HPP
#define CT_REFLECT_METADATA_HPP

#include "../String.hpp"

#include <functional>

namespace ct
{
    struct Description
    {
        constexpr Description(const char* desc) : m_desc(desc) {}

        const char* m_desc;
    };

    template<class T>
    struct Initializer
    {
        constexpr Initializer(T(*ptr)(), StringView str):
            m_ptr(ptr),
            m_str(str)
        {
        }

        T operator()()
        {
            return m_ptr();
        }

        T(*m_ptr)();
        StringView m_str;
    };

    template<class T>
    constexpr Initializer<T> makeInitializer(T(*ptr)(), StringView str)
    {
        return Initializer<T>(ptr, str);
    }
}

#endif // CT_REFLECT_METADATA_HPP
