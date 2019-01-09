#pragma once
#include "accessor_traits.hpp"

#include <type_traits>

namespace ct
{
    template <class T, class E = void>
    struct Getter;

    template <class T, class D>
    struct Getter<D (T::*)() const, typename std::enable_if<!std::is_member_object_pointer<D (T::*)()>::value>::type>
    {
        using Get_t = typename std::decay<D>::type;
        using Return_t = D;
        using Getter_t = MemberAccessorGetterType;

        constexpr Getter(D (T::*getter)() const) : m_getter(getter) {}

        D get(const T& obj) const { return (obj.*m_getter)(); }

        D (T::*m_getter)() const;
    };

    template <class T, class D>
    struct Getter<D (T::*)(), typename std::enable_if<!std::is_member_object_pointer<D (T::*)()>::value>::type>
    {
        using Get_t = typename std::decay<D>::type;
        using Return_t = D;
        using Getter_t = MemberAccessorGetterType;

        constexpr Getter(D (T::*getter)()) : m_getter(getter) {}

        D get(T& obj) const { return (obj.*m_getter)(); }

        D (T::*m_getter)();
    };

    template <class T, class D>
    struct Getter<D (*)(const T&), void>
    {
        using Get_t = typename std::decay<D>::type;
        using Return_t = D;
        using Getter_t = ExternalAccessorGetterType;

        constexpr Getter(D (*getter)(const T&)) : m_getter(getter) {}
        D get(const T& obj) const { return m_getter(obj); }
      private:
        D (*m_getter)(const T&);
    };

    template <class T, class D>
    struct Getter<D T::*, typename std::enable_if<std::is_member_object_pointer<D T::*>::value>::type>
    {
        using Get_t = typename std::decay<D>::type;
        using Return_t = const D&;
        using Getter_t = FieldGetterType;

        constexpr Getter(D T::*getter) : m_getter(getter) {}
        const D& get(const T& obj) const { return obj.*m_getter; }

        D T::*m_getter;
    };
}
