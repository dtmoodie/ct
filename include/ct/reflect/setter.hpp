#pragma once
#include "access_token.hpp"
#include "accessor_traits.hpp"

#include <utility>

namespace ct
{
    template <class T>
    struct Setter;

    template <class T, class D>
    struct Setter<void (T::*)(D)>
    {
        using Set_t = D;
        using Setter_t = MemberAccessorSetterType;

        constexpr Setter(void (T::*setter)(D)) : m_setter(setter) {}

        void set(T& obj, D&& data) const { (obj.*m_setter)(std::forward(data)); }
        void set(T& obj, const D& data) const { (obj.*m_setter)(data); }

        AccessToken<void (T::*)(D)> set(T& obj) const { return AccessToken<void (T::*)(D)>(obj, m_setter); }

        void (T::*m_setter)(D);
    };

    template <class T, class D>
    struct Setter<D& (T::*)()>
    {
        using Set_t = D&;
        using Setter_t = MemberAccessorSetterType;

        constexpr Setter(D& (T::*setter)()) : m_setter(setter) {}

        void set(T& obj, D&& data) const { (obj.*m_setter)() = std::move(data); }
        void set(T& obj, const D& data) const { (obj.*m_setter)() = data; }

        D& set(T& obj) const { return (obj.*m_setter)(); }

        D& (T::*m_setter)();
    };

    template <class T, class D>
    struct Setter<D (*)(T&)>
    {
        using Set_t = D;
        using Setter_t = ExternalAccessorSetterType;

        constexpr Setter(D (*setter)(T&)) : m_setter(setter) {}

        void set(T& obj, typename std::decay<D>::type&& data) const { m_setter(obj) = std::move(data); }

        void set(T& obj, const typename std::decay<D>::type& data) const { m_setter(obj) = data; }

        D set(T& obj) const { return m_setter(obj); }

        D (*m_setter)(T&);
    };

    template <class T, class D>
    struct Setter<void (*)(T&, D)>
    {
        using Set_t = D;
        using Setter_t = ExternalAccessorSetterType;

        constexpr Setter(void (*setter)(T&, D)) : m_setter(setter) {}

        void set(T& obj, D&& data) const { m_setter(obj, std::forward(data)); }
        void set(T& obj, const D& data) const { m_setter(obj, data); }

        AccessToken<void (*)(T&, D)> set(T& obj) const { return AccessToken<void (*)(T&, D)>(obj, m_setter); }

        void (*m_setter)(T&, D);
    };

    template <class T, class D>
    struct Setter<D T::*>
    {
        using Set_t = D;
        using Setter_t = FieldSetterType;

        constexpr Setter(D T::*setter) : m_setter(setter) {}
        D& set(T& obj) const { return obj.*m_setter; }
        void set(T& obj, D&& data) const { obj.*m_setter = std::move(data); }
        void set(T& obj, const D& data) const { obj.*m_setter = data; }

        D T::*m_setter;
    };
}
