#ifndef CT_ENUM_HPP
#define CT_ENUM_HPP
#include "Result.hpp"
#include "StringView.hpp"
#include "config.hpp"
#include "reflect_forward.hpp"
#include "reflect_macros.hpp"
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

    template <class E, class T, T VALUE, uint16_t I, bool BITSET = false>
    struct EnumValue
    {
        static constexpr uint16_t index = I;
        constexpr EnumValue() = default;
        constexpr operator T() const { return VALUE; }
// constexpr operator E() const { return E(VALUE); }

#ifdef _MSC_VER
        static constexpr T value = VALUE;
#else
        static constexpr E value = E(VALUE);
#endif

        constexpr T operator()() const { return VALUE; }
    };

    template <class E, class T, T V1, T V2, uint16_t I, uint16_t J>
    constexpr EnumValue<E, T, V1 | V2, static_cast<uint16_t>(-1), false> operator|(EnumValue<E, T, V1, I, false>,
                                                                                   EnumValue<E, T, V2, J, false>)
    {
        return {};
    }

    template <class E, class T, T V1, T V2, uint16_t I, uint16_t J, bool BITSET>
    constexpr bool operator&(EnumValue<E, T, V1, I, BITSET>, EnumValue<E, T, V2, J, BITSET>)
    {
        return V1 & V2;
    }

    template <class E, class T, T V1, T V2, uint16_t I, uint16_t J, bool BITSET>
    constexpr bool operator==(EnumValue<E, T, V1, I, BITSET>, EnumValue<E, T, V2, J, BITSET>)
    {
        return V1 == V2;
    }

    struct EnumTag
    {
    };

    template <class TAG, class TYPE>
    struct EnumBase : EnumTag
    {
        TYPE value;

        constexpr EnumBase(TYPE v) : value(v) {}
        constexpr EnumBase() : value{} {}

        template <TYPE V, uint16_t I, bool BITSET>
        EnumBase& operator=(EnumValue<TAG, TYPE, V, I, BITSET>)
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

        template <TYPE V, uint16_t I, bool BITSET>
        constexpr bool operator>(EnumValue<TAG, TYPE, V, I, BITSET>) const
        {
            return value > V;
        }

        template <TYPE V, uint16_t I, bool BITSET>
        constexpr bool operator<(EnumValue<TAG, TYPE, V, I, BITSET>) const
        {
            return value < V;
        }

        template <TYPE V, uint16_t I, bool BITSET>
        constexpr bool operator<=(EnumValue<TAG, TYPE, V, I, BITSET>) const
        {
            return value <= V;
        }

        template <TYPE V, uint16_t I, bool BITSET>
        constexpr bool operator>=(EnumValue<TAG, TYPE, V, I, BITSET>) const
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
    constexpr E operator|(T e, EnumValue<E, T, V1, I, false>)
    {
        return E(e | V1);
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr E operator|(EnumValue<E, T, V1, I, false>, T e)
    {
        return E(V1 | e);
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr E operator|(E e, EnumValue<E, T, V1, I, false>)
    {
        return E(e.value | V1);
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr E operator|(EnumValue<E, T, V1, I, false>, E e)
    {
        return E(V1 | e.value);
    }

    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator&(E val, EnumValue<E, T, V1, I, BITSET>)
    {
        return val.value & V1;
    }

    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator&(EnumValue<E, T, V1, I, BITSET>, E val)
    {
        return V1 & val.value;
    }

    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator&(T val, EnumValue<E, T, V1, I, BITSET>)
    {
        return val & V1;
    }

    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator&(EnumValue<E, T, V1, I, BITSET>, T val)
    {
        return V1 & val;
    }

    //////////////////////////////////////////
    /// ~
    template <class E>
    constexpr EnableIf<std::is_base_of<EnumTag, E>::value, E> operator~(E e)
    {
        return E(~e.value);
    }

    //////////////////////////////////////////
    /// ==
    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator==(EnumValue<E, T, V1, I, BITSET>, E val)
    {
        return val.value == V1;
    }

    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator==(EnumValue<E, T, V1, I, BITSET>, T val)
    {
        return val == V1;
    }

    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator==(E val, EnumValue<E, T, V1, I, BITSET>)
    {
        return val.value == V1;
    }

    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator==(T val, EnumValue<E, T, V1, I, BITSET>)
    {
        return val == V1;
    }

    //////////////////////////////////////////
    /// !=
    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator!=(EnumValue<E, T, V1, I, BITSET>, E val)
    {
        return val.value != V1;
    }

    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator!=(EnumValue<E, T, V1, I, BITSET>, T val)
    {
        return val != V1;
    }

    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator!=(E val, EnumValue<E, T, V1, I, BITSET>)
    {
        return val.value != V1;
    }

    template <class E, class T, T V1, uint16_t I, bool BITSET>
    constexpr bool operator!=(T val, EnumValue<E, T, V1, I, BITSET>)
    {
        return val != V1;
    }

    template <class T>
    struct EnumField
    {
        constexpr auto value() const -> decltype(T::value) { return T::value; }
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
        return Reflect<T>::getPtr(idx).name.equal(str, case_sensitive) ? success<T>(Reflect<T>::getPtr(idx).value())
                                                                       : error("Invaid string to enum conversion");
    }

    template <class T, index_t I>
    constexpr Result<T> fromString(StringView str, ct::Indexer<I> idx, bool case_sensitive)
    {
        return Reflect<T>::getPtr(idx).name.equal(str, case_sensitive) ? success<T>(Reflect<T>::getPtr(idx).value())
                                                                       : fromString<T>(str, --idx, case_sensitive);
    }

    template <class T>
    constexpr Result<T> fromString(StringView str, bool case_sensitive = false)
    {
        return fromString<T>(str, ct::Reflect<T>::end(), case_sensitive);
    }
}

#ifndef __NVCC__

#define ENUM_START_(NAME, TYPE, BITSET_)                                                                               \
    struct NAME : ct::EnumBase<NAME, TYPE>                                                                             \
    {                                                                                                                  \
        using EnumValueType = TYPE;                                                                                    \
        using EnumType = NAME;                                                                                         \
        constexpr static const bool BITSET = BITSET_;                                                                  \
        constexpr NAME() {}                                                                                            \
        constexpr NAME(TYPE v) : EnumBase<NAME, TYPE>(v) {}                                                            \
        template <TYPE V, uint16_t I>                                                                                  \
        constexpr NAME(ct::EnumValue<NAME, TYPE, V, I, BITSET>) : EnumBase<NAME, TYPE>(V)                              \
        {                                                                                                              \
        }                                                                                                              \
        REFLECT_STUB

#define BITSET_START(NAME, TYPE) ENUM_START_(NAME, TYPE, true)

#define ENUM_START(NAME, TYPE) ENUM_START_(NAME, TYPE, false)

#define ENUM_VALUE(NAME, VALUE)                                                                                        \
    CT_INLINE_VAR ct::EnumValue<EnumType, EnumValueType, VALUE, __COUNTER__ - REFLECT_COUNT_START, BITSET> NAME = {};  \
    static constexpr auto getPtr(ct::Indexer<NAME.index>)                                                              \
    {                                                                                                                  \
        return ct::makeEnumField<ct::EnumValue<EnumType, EnumValueType, VALUE, NAME.index>>(#NAME);                    \
    }

#define ENUM(NAME)                                                                                                     \
    static constexpr auto getPtr(ct::Indexer<__COUNTER__ - REFLECT_COUNT_START> idx)                                   \
    {                                                                                                                  \
        return ct::makeEnumField<ct::EnumValue<DataType, decltype(DataType::NAME), DataType::NAME, idx.index>>(#NAME); \
    }

#define ENUM_END                                                                                                       \
    static constexpr const ct::index_t NUM_FIELDS = __COUNTER__ - REFLECT_COUNT_START;                                 \
    }

#else // defined(__NVCC__)

#define ENUM_START(NAME, TYPE)                                                                                         \
    struct NAME : ct::EnumBase<NAME, TYPE>                                                                             \
    {                                                                                                                  \
        using EnumValueType = TYPE;                                                                                    \
        using EnumType = NAME;                                                                                         \
        constexpr NAME() {}                                                                                            \
        constexpr NAME(TYPE v) : EnumBase<NAME, TYPE>(v) {}                                                            \
        template <TYPE V, uint16_t I>                                                                                  \
        constexpr NAME(ct::EnumValue<NAME, TYPE, V, I>) : EnumBase<NAME, TYPE>(V)                                      \
        {                                                                                                              \
        }                                                                                                              \
        REFLECT_STUB

#define ENUM_VALUE(NAME, VALUE) static constexpr const EnumValueType NAME = VALUE;

#define ENUM(NAME)

#define ENUM_END                                                                                                       \
    static constexpr const ct::index_t NUM_FIELDS = __COUNTER__ - REFLECT_COUNT_START;                                 \
    }
#endif
#endif // CT_ENUM_HPP
