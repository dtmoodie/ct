#ifndef CT_REFLECT_COMPARE_HPP
#define CT_REFLECT_COMPARE_HPP
#include <ct/reflect.hpp>

namespace ct
{
    struct Equal
    {
        template <class T>
        bool test(const char* /*name*/, const T& lhs, const T& rhs) const
        {
            return lhs == rhs;
        }
    };

    template <class T, class Comparator = Equal>
    auto compare(const T& lhs, const T& rhs, const Comparator& cmp = Comparator()) -> ct::EnableIfReflected<T, bool>;
    template <class T, class Comparator = Equal>
    auto compare(const T& lhs, const T& rhs, const Comparator& cmp = Comparator()) -> ct::DisableIfReflected<T, bool>;

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
    compareHelper(const T&, const T&, const ct::Indexer<0>, const Comparator&)
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

#endif // CT_REFLECT_COMPARE_HPP
