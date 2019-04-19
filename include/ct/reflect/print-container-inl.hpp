#ifndef CT_REFLECT_PRINT_CONTAINER_HPP
#define CT_REFLECT_PRINT_CONTAINER_HPP
#include "print.hpp"

#include <map>
#include <string>
#include <vector>

namespace ct
{
    template <class Options = PrintOptions, class T, class A>
    auto printStruct(std::ostream& os, const std::vector<T, A>& obj) -> ct::EnableIfReflected<T>
    {
        if (!obj.empty())
        {
            os << "[";
            for (size_t i = 0; i < obj.size(); ++i)
            {
                if (i != 0)
                {
                    os << " ";
                }
                printStruct<Options>(os, obj[i]);
            }
            os << "]";
        }
    }

    template <class Options = PrintOptions, class K, class V>
    auto printStruct(std::ostream& os, const std::map<K, V>& obj) -> ct::EnableIfReflected<V>
    {
        if (!obj.empty())
        {
            os << "{";
            size_t count = 0;

            for (const auto& itr : obj)
            {
                if (count != 0)
                {
                    os << " ";
                }

                os << itr.first << ":";
                printStruct<Options>(os, itr.second);

                ++count;
            }
            os << "}";
        }
    }
}
#endif // CT_REFLECT_PRINT_CONTAINER_HPP
