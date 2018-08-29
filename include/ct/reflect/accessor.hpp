#pragma once
#include "getter.hpp"
#include "setter.hpp"

namespace ct
{
    template<class T0, class T1, class GetterTraits = DefaultGetterTraits, class SetterTraits = DefaultSetterTraits>
    struct Accessor: public Getter<T0>, public Setter<T1>
    {
        using GetterTraits_t = GetterTraits;
        using SetterTraits_t = SetterTraits;

        constexpr Accessor(T0 getter, T1 setter):
            Getter<T0>(getter), Setter<T1>(setter){}
    };

    template<class T0, class T, class T1, class GetterTraits, class SetterTraits>
    struct Accessor<T0, void(T::*)(T1), GetterTraits,SetterTraits> : public Getter<T0>
    {
        using GetterTraits_t = GetterTraits;
        using SetterTraits_t = SetterTraits;
        using SetType = T1;

        constexpr Accessor(T0 getter, void(T::*setter)(T1)):
            Getter<T0>(getter), m_setter(setter){}



        void set(T& obj, T1&& data) const
        {
            (obj.*m_setter)(std::forward(data));
        }

        AccessToken<void(T::*)(T1)> set(T& obj) const
        {
            return {obj, m_setter, Getter<T0>::get(obj)};
        }

    private:
        void(T::*m_setter)(T1);
    };

    template<class T0, class GetterTraits>
    struct Accessor<T0, void, GetterTraits, void>: public Getter<T0>
    {
        using GetterTraits_t = GetterTraits;
        using SetterTraits_t = void;
        using SetType = void;

        constexpr Accessor(T0 getter):
            Getter<T0>(getter){}
    };

    template<class GetterTraits = DefaultGetterTraits, class SetterTraits = DefaultSetterTraits, class T0, class T1>
    constexpr Accessor<T0, T1, GetterTraits, SetterTraits> makeAccessor(T0 getter, T1 setter)
    {
        return Accessor<T0, T1, GetterTraits, SetterTraits>(getter, setter);
    }

    template<class GetterTraits = DefaultGetterTraits, class T0>
    constexpr Accessor<T0, void, GetterTraits, void> makeAccessor(T0 getter)
    {
        return Accessor<T0, void, GetterTraits, void>(getter);
    }
}
