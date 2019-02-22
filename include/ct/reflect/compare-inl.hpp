#ifndef CT_REFLECT_COMPARE_INL_HPP
#define CT_REFLECT_COMPARE_INL_HPP
#include "compare.hpp"
#include <ct/reflect.hpp>

namespace ct
{
    template <class T, class Comparator>
    EnableIf<!IsMemberFunction<T, 0>::value, bool>
    compareHelper(const T& lhs, const T& rhs, const ct::Indexer<0> idx, const Comparator& cmp)
    {
        auto accessor = Reflect<T>::getPtr(idx);
        const char* name = accessor.m_name;
        if (!cmp.test(name, accessor.get(lhs), accessor.get(rhs)))
        {
            return false;
        }
        return true;
    }

    template <class T, class Comparator>
    EnableIf<IsMemberFunction<T, 0>::value, bool>
    compareHelper(const T& lhs, const T& rhs, const ct::Indexer<0> idx, const Comparator& cmp)
    {
        return true;
    }

    template <class T, ct::index_t I, class Comparator>
    EnableIf<!IsMemberFunction<T, I>::value, bool>
    compareHelper(const T& lhs, const T& rhs, const ct::Indexer<I> idx, const Comparator& cmp)
    {
        auto accessor = Reflect<T>::getPtr(idx);
        const char* name = accessor.m_name;
        if (!cmp.test(name, accessor.get(lhs), accessor.get(rhs)))
        {
            return false;
        }
        return compareHelper(lhs, rhs, --idx, cmp);
    }

    template <class T, ct::index_t I, class Comparator>
    EnableIf<IsMemberFunction<T, I>::value, bool>
    compareHelper(const T& lhs, const T& rhs, const ct::Indexer<I> idx, const Comparator& cmp)
    {
        /*auto accessor = Reflect<T>::getPtr(idx);
        const char* name = Reflect<T>::getName(idx);
        if (!cmp.test(name, accessor.invoke(lhs), accessor.invoke(rhs)))
        {
            return false;
        }*/
        return compareHelper(lhs, rhs, --idx, cmp);
    }

    template <class T, class Comparator>
    auto compare(const T& lhs, const T& rhs, const Comparator& cmp) -> ct::EnableIfReflected<T, bool>
    {
        return compareHelper(lhs, rhs, Reflect<T>::end(), cmp);
    }

    template <class T, class Comparator>
    auto compare(const T& lhs, const T& rhs, const Comparator& cmp) -> ct::DisableIfReflected<T, bool>
    {
        return cmp.test(lhs, rhs);
    }
}

template <class T>
auto operator==(const T& lhs, const T& rhs) -> ct::EnableIfReflected<T, bool>
{
    return ct::compare(lhs, rhs);
}

template <class T>
auto operator!=(const T& lhs, const T& rhs) -> ct::EnableIfReflected<T, bool>
{
    return !(lhs == rhs);
}
#endif // CT_REFLECT_COMPARE_INL_HPP
