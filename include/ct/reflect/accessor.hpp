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

        using GetterType_t = typename Getter<T0>::GetterType;
        using SetterType_t = typename Setter<T1>::SetterType;

        constexpr Accessor(T0 getter, T1 setter):
            Getter<T0>(getter), Setter<T1>(setter){}
    };

    template<class T0, class GetterTraits>
    struct Accessor<T0, void, GetterTraits, void>: public Getter<T0>
    {
        using GetterTraits_t = GetterTraits;
        using SetterTraits_t = void;
        using SetType = void;
        using GetterType_t = typename Getter<T0>::GetterType;
        using SetterType_t = void;

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
