#pragma once
#include "equal.hpp"
#include "reflect.hpp"

namespace ct
{
    template<class T>
    bool equalHelper(const T& lhs, const T& rhs, const ct::_counter_<0> idx)
    {
        auto accessor = Reflect<T>::getAccessor(idx);
        if(accessor.get(lhs) != accessor.get(rhs))
        {
            return false;
        }
        return true;
    }

    template<class T, int I>
    bool equalHelper(const T& lhs, const T& rhs, const ct::_counter_<I> idx)
    {
        auto accessor = Reflect<T>::getAccessor(idx);
        if(accessor.get(lhs) != accessor.get(rhs))
        {
            return false;
        }
        return equalHelper(lhs, rhs, --idx);
    }

    template<class T>
    bool equal(const T& lhs, const T& rhs)
    {
        return equalHelper(lhs, rhs, Reflect<T>::end());
    }
}

