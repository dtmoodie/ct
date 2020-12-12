#ifndef CT_ENUM_HPP
#define CT_ENUM_HPP
#include "Result.hpp"
#include "StringView.hpp"
#include "config.hpp"
#include "reflect_forward.hpp"
#include "type_traits.hpp"

#include <type_traits>

/* This file constains a c++ enum replacement with expanded reflection capabilities.
 * A ct enum supports ostream<< operators and a fromString method.
 * The only caveat to this for c++ < 2017 is that you cannot take a ct Enum by reference into a function, it must be
 * taken by value.  Find example usage in tests/enum
 */
namespace ct
{

    template <class T>
    T value(T val)
    {
        return val;
    }

    template <class E, class T, T VALUE, uint16_t I>
    struct EnumValue
    {
        static constexpr uint16_t index = I;
        constexpr EnumValue() = default;
        constexpr operator T() const { return VALUE; }

#if defined(_MSC_VER) || defined(__clang__)
        static constexpr T value = VALUE;
#else
        static constexpr E value = E(VALUE);
#endif
    };

    template <class E, class T, T V1, T V2, uint16_t I, uint16_t J>
    constexpr EnumValue<E, T, V1 | V2, static_cast<uint16_t>(-1)> operator|(EnumValue<E, T, V1, I>,
                                                                            EnumValue<E, T, V2, J>)
    {
        return {};
    }

    template <class E, class T, T V1, T V2, uint16_t I, uint16_t J>
    constexpr bool operator&(EnumValue<E, T, V1, I>, EnumValue<E, T, V2, J>)
    {
        return V1 & V2;
    }

    template <class E, class T, T V1, T V2, uint16_t I, uint16_t J>
    constexpr bool operator==(EnumValue<E, T, V1, I>, EnumValue<E, T, V2, J>)
    {
        return V1 == V2;
    }

    struct EnumTag
    {
    };

    struct BitsetTag
    {
    };

    template <class TAG, class TYPE>
    struct EnumBase : EnumTag
    {
        TYPE value;

        constexpr EnumBase(TYPE v) : value(v) {}
        constexpr EnumBase() : value{} {}

        template <TYPE V, uint16_t I>
        EnumBase& operator=(EnumValue<TAG, TYPE, V, I>)
        {
            value = V;
            return *this;
        }

        constexpr operator TYPE() const { return value; }

        EnumBase& operator=(TYPE v)
        {
            value = v;
            return *this;
        }

        template <TYPE V, uint16_t I>
        constexpr bool operator>(EnumValue<TAG, TYPE, V, I>) const
        {
            return value > V;
        }

        template <TYPE V, uint16_t I>
        constexpr bool operator<(EnumValue<TAG, TYPE, V, I>) const
        {
            return value < V;
        }

        template <TYPE V, uint16_t I>
        constexpr bool operator<=(EnumValue<TAG, TYPE, V, I>) const
        {
            return value <= V;
        }

        template <TYPE V, uint16_t I>
        constexpr bool operator>=(EnumValue<TAG, TYPE, V, I>) const
        {
            return value >= V;
        }
    };

    // For now these increment the value by 1, but in the future it should increment to the next enumeration
    // These are defined outside of the class such that we can use some introspection tools to find the next enum. later
    template <class E, class T>
    EnumBase<E, T>& operator++(EnumBase<E, T>& v)
    {
        ++v.value;
        return v;
    }

    template <class E, class T>
    constexpr EnumBase<E, T> operator++(EnumBase<E, T>& v, int)
    {
        return (v.value++);
    }

    //////////////////////////////////////////
    /// bitwiser operators
    template <class E, class T, T V1, uint16_t I>
    constexpr E operator|(T e, EnumValue<E, T, V1, I>)
    {
        return E(e | V1);
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr E operator|(EnumValue<E, T, V1, I>, T e)
    {
        return E(V1 | e);
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr E operator|(E e, EnumValue<E, T, V1, I>)
    {
        return E(e | V1);
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr E operator|(EnumValue<E, T, V1, I>, E e)
    {
        return E(V1 | e);
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr T operator&(E val, EnumValue<E, T, V1, I>)
    {
        return val.value & V1;
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr T operator&(EnumValue<E, T, V1, I>, E val)
    {
        return T(V1 & val);
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr T operator&(T val, EnumValue<E, T, V1, I>)
    {
        return val & V1;
    }

    /*template <class E, class T, T V1, uint16_t I>
    constexpr bool operator&(EnumValue<E, T, V1, I>, T val)
    {
        return V1 & val;
    }*/

    //////////////////////////////////////////
    /// ~
    template <class E>
    constexpr EnableIf<IsBase<Base<EnumTag>, Derived<E>>::value, E> operator~(E e)
    {
        return E(~e.value);
    }

    //////////////////////////////////////////
    /// ==
    template <class E, class T, T V1, uint16_t I>
    constexpr bool operator==(EnumValue<E, T, V1, I>, E val)
    {
        return val == V1;
    }

    /*template <class E, class T, T V1, uint16_t I>
    constexpr bool operator==(EnumValue<E, T, V1, I>, T val)
    {
        return val == V1;
    }*/

    template <class E, class T, T V1, uint16_t I>
    constexpr bool operator==(E val, EnumValue<E, T, V1, I>)
    {
        return val.value == V1;
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr bool operator==(T val, EnumValue<E, T, V1, I>)
    {
        return val == V1;
    }

    //////////////////////////////////////////
    /// !=
    template <class E, class T, T V1, uint16_t I>
    constexpr bool operator!=(EnumValue<E, T, V1, I>, E val)
    {
        return val.value != V1;
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr bool operator!=(EnumValue<E, T, V1, I>, T val)
    {
        return val != V1;
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr bool operator!=(E val, EnumValue<E, T, V1, I>)
    {
        return val.value != V1;
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr bool operator!=(T val, EnumValue<E, T, V1, I>)
    {
        return val != V1;
    }

    template <class T>
    struct EnumField
    {
        // Uhh so this needs to include flags.hpp but it needs to also not
        constexpr static const Flag_t DataFlags = (1 << 5); // COMPILE_TIME_CONSTANT

        constexpr T value() const { return T{}; }

        constexpr StringView getName() const { return name; }
        StringView name;
    };

    template <class T>
    constexpr EnumField<T> makeEnumField(StringView name)
    {
        return EnumField<T>{name};
    }

    template <class T>
    constexpr Result<T> fromString(StringView str, ct::Indexer<0> idx, bool case_sensitive)
    {
        return Reflect<T>::getPtr(idx).name.equal(str, case_sensitive) ? success<T>(T(Reflect<T>::getPtr(idx).value()))
                                                                       : error("Invaid string to enum conversion");
    }

    template <class T, index_t I>
    constexpr Result<T> fromString(StringView str, ct::Indexer<I> idx, bool case_sensitive)
    {
        return Reflect<T>::getPtr(idx).name.equal(str, case_sensitive) ? success<T>(T(Reflect<T>::getPtr(idx).value()))
                                                                       : fromString<T>(str, --idx, case_sensitive);
    }

    template <class T>
    constexpr Result<T> fromString(StringView str, bool case_sensitive = false)
    {
        return fromString<T>(str, ct::Reflect<T>::end(), case_sensitive);
    }
} // namespace ct
#endif // CT_ENUM_HPP
