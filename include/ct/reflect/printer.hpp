#pragma once
#include <ct/reflect/detail/printer.hpp>

#include <vector>
#include <ostream>
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
    template<class T>
    ct::reflect::enable_if_reflected<T, ostream>& operator<<(ostream& os, const std::vector<T>& data)
    {
        if (!data.empty())
        {
            os << '[';
            for (size_t i = 0; i < data.size(); ++i)
            {
                if (i != 0)
                    os << ',';
                os << data[i];
            }
            os << ']';
        }
        return os;
    }
}