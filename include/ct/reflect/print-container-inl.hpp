#pragma once
#include "print.hpp"

#include <vector>
#include <map>

namespace ct
{
template <class Options = PrintOptions, class T>
auto printStruct(std::ostream& os, const std::vector<T>& obj) -> ct::enable_if_reflected<T>
{
    if(!obj.empty())
    {
        os << "[";
        for(size_t i = 0; i < obj.size(); ++i)
        {
            if(i != 0)
            {
                os << " ";
            }
            printStruct<Options>(os, obj[i]);
        }
        os << "]";
    }
}

template <class Options = PrintOptions, class K, class V>
auto printStruct(std::ostream& os, const std::map<K, V>& obj) -> ct::enable_if_reflected<V>
{
    if(!obj.empty())
    {
        os << "{";
        size_t count = 0;

        for(const auto& itr : obj)
        {
            if(count != 0)
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
