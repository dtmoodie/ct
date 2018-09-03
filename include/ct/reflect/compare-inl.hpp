#pragma once
#include "compare.hpp"
#include <ct/reflect.hpp>

namespace ct
{
template <class T, class Comparator>
bool compareHelper(const T& lhs, const T& rhs, const ct::Indexer<0> idx, const Comparator& cmp)
{
    auto accessor = Reflect<T>::getAccessor(idx);
    const char* name = Reflect<T>::getName(idx);
    if (!cmp.test(name, accessor.get(lhs), accessor.get(rhs)))
    {
        return false;
    }
    return true;
}

template <class T, ct::index_t I, class Comparator>
bool compareHelper(const T& lhs, const T& rhs, const ct::Indexer<I> idx, const Comparator& cmp)
{
    auto accessor = Reflect<T>::getAccessor(idx);
    const char* name = Reflect<T>::getName(idx);
    if (!cmp.test(name, accessor.get(lhs), accessor.get(rhs)))
    {
        return false;
    }
    return compareHelper(lhs, rhs, --idx, cmp);
}

template <class T, class Comparator = Equal>
auto compare(const T& lhs, const T& rhs, const Comparator& cmp) -> ct::enable_if_reflected<T, bool>
{
    return compareHelper(lhs, rhs, Reflect<T>::end(), cmp);
}
template <class T, class Comparator = Equal>
auto compare(const T& lhs, const T& rhs, const Comparator& cmp) -> ct::enable_if_not_reflected<T, bool>
{
    return cmp.test(lhs, rhs);
}
}

template <class T>
auto operator==(const T& lhs, const T& rhs) -> ct::enable_if_reflected<T, bool>
{
    return ct::compare(lhs, rhs);
}

template <class T>
auto operator!=(const T& lhs, const T& rhs) -> ct::enable_if_reflected<T, bool>
{
    return !(lhs == rhs);
}
