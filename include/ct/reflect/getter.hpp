#pragma once
#include <type_traits>

namespace ct
{
    template <class T>
    struct Getter;

    struct DefaultGetterTraits
    {
    };
    struct DefaultSetterTraits
    {
    };

    struct CalculatedValue
    {
    };

    template <class T, class D>
    struct Getter<D (T::*)() const>
    {
        using GetType = typename std::decay<D>::type;
        using RetType = D;

        constexpr Getter(D (T::*getter)() const) : m_getter(getter) {}

        D get(const T& obj) const { return (obj.*m_getter)(); }
      private:
        D (T::*m_getter)() const;
    };

    template <class T, class D>
    struct Getter<D (*)(const T&)>
    {
        using GetType = typename std::decay<D>::type;
        using RetType = D;

        constexpr Getter(D (*getter)(const T&)) : m_getter(getter) {}
        D get(const T& obj) const { return m_getter(obj); }
      private:
        D (*m_getter)(const T&);
    };
}
