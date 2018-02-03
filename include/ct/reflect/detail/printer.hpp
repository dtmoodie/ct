#pragma once
#include <ct/detail/counter.hpp>
#include <ct/reflect/reflect_data.hpp>

namespace ct
{
    namespace reflect
    {
        namespace detail
        {
            template <class T>
            static constexpr inline void printStructHelper(std::ostream& os, const T& data, _counter_<0>)
            {
                os << getName<0, T>() << ':' << get<0>(data);
            }

            template <int I, class T>
            static constexpr inline void printStructHelper(std::ostream& os, const T& data, _counter_<I>)
            {
                printStructHelper(os, data, _counter_<I - 1>());
                os << ' ' << getName<I, T>() << ':' << get<I>(data);
            }
        }
    }
}
