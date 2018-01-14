#pragma once

#include <ostream>
#include <ct/reflect/detail/printer.hpp>

namespace ct
{
    namespace reflect
    {
        template <class T>
        static constexpr inline void printStruct(std::ostream& os, const T& data)
        {
            detail::printStructHelper(os, data, _counter_<ReflectData<T>::N - 1>());
        }
    }
}


namespace std
{
    template <class T>
    ct::reflect::enable_if_reflected<T, ostream>& operator<<(ostream& os, const T& data)
    {
        ct::reflect::printStruct(os, data);
        return os;
    }
}