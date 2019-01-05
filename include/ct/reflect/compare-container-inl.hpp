#pragma once
#include "compare.hpp"
#include "compare-inl.hpp"
#include <vector>
#include <map>

namespace ct
{
template <class T, class Comparator>
bool compare(const std::vector<T>& lhs, const std::vector<T>& rhs, const Comparator& cmp)
{
    if(lhs.size() != rhs.size())
    {
        return false;
    }
    for(size_t i = 0; i < lhs.size(); ++i)
    {
        if(lhs[i] != rhs[i])
        {
            return false;
        }
    }

    return true;
}


template <class K, class V, class Comparator>
bool compare(const std::map<K, V>& lhs, const std::map<K, V>& rhs, const Comparator& cmp)
{
    if(lhs.size() != rhs.size())
    {
        return false;
    }

    for(const auto& kvp : lhs)
    {
        auto itr = rhs.find(kvp.first);
        if(itr != rhs.end())
        {
            if(itr->second != kvp.second)
            {
                return false;
            }
        }else
        {
            return false;
        }
    }

    return true;
}

}
