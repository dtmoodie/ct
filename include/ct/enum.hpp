#ifndef CT_ENUM_HPP
#define CT_ENUM_HPP
#include "reflect.hpp"
#include "reflect_macros.hpp"

#include "StringView.hpp"
#include <type_traits>

namespace ct
{

    template <class E, class T, T VALUE, uint16_t I>
    struct EnumValue
    {
        static constexpr uint16_t index = I;
        constexpr operator T() const { return VALUE; }
        static constexpr E value = E(VALUE);
        constexpr T operator()() const { return VALUE; }
    };

    template <class TAG, class TYPE>
    struct EnumBase
    {
        TYPE value;

        constexpr EnumBase(TYPE v) : value(v) {}

        template <TYPE V, uint16_t I>
        constexpr EnumBase& operator=(EnumValue<TAG, TYPE, V, I>)
        {
            value = V;
            return *this;
        }

        constexpr operator TYPE() const { return value; }

        constexpr EnumBase& operator=(TYPE v)
        {
            value = v;
            return *this;
        }

        template <TYPE V, uint16_t I>
        constexpr bool operator!=(EnumValue<TAG, TYPE, V, I>)
        {
            return value != V;
        }

        template <TYPE V, uint16_t I>
        constexpr bool operator==(ct::EnumValue<TAG, TYPE, V, I>)
        {
            return value == V;
        }

        template <TYPE V, uint16_t I>
        constexpr bool operator>(EnumValue<TAG, TYPE, V, I>)
        {
            return value > V;
        }

        template <TYPE V, uint16_t I>
        constexpr bool operator<(EnumValue<TAG, TYPE, V, I>)
        {
            return value < V;
        }

        template <TYPE V, uint16_t I>
        constexpr bool operator>=(EnumValue<TAG, TYPE, V, I>)
        {
            return value >= V;
        }
    };

    template <class T>
    struct EnumField
    {
        constexpr auto value() { return T::value; }
        StringView name;
    };

    template <class T>
    constexpr EnumField<T> makeEnumField(StringView name)
    {
        return EnumField<T>{name};
    }

    template <class T>
    struct IsEnumField : std::false_type
    {
    };

    template <class T>
    struct IsEnumField<EnumField<T>> : std::true_type
    {
    };

    template <class T, class U = void>
    using EnableIfIsEnum = ct::EnableIf<IsEnumField<typename ct::PtrType<T, 0>>::value, U>;

    template <class T>
    void printEnums(std::ostream& os, ct::Indexer<0> idx)
    {
        auto mdata = ct::Reflect<T>::getPtr(idx);
        os << mdata << '\n';
    }

    template <class T, ct::index_t I>
    void printEnums(std::ostream& os, ct::Indexer<I> idx)
    {
        printEnums<T>(os, --idx);
        auto mdata = ct::Reflect<T>::getPtr(idx);
        os << mdata << '\n';
    }

    template <class T>
    void printEnums(std::ostream& ios)
    {
        printEnums<T>(ios, ct::Reflect<T>::end());
    }

    template <class T>
    constexpr T fromString(StringView str, ct::Indexer<0> idx)
    {
        return Reflect<T>::getPtr(idx).name == str ? Reflect<T>::getPtr(idx).value() : throw std::runtime_error("Invaid string to enum conversion");
    }

    template <class T, index_t I>
    constexpr T fromString(StringView str, ct::Indexer<I> idx)
    {
        return Reflect<T>::getPtr(idx).name == str ? Reflect<T>::getPtr(idx).value() : fromString<T>(str, --idx);
    }

    template <class T>
    constexpr T fromString(StringView str)
    {
        return fromString<T>(str, ct::Reflect<T>::end());
    }
}

namespace std
{
    template <class E, class T, T V, uint16_t I>
    ostream& operator<<(ostream& os, ct::EnumValue<E, T, V, I>)
    {
        os << ct::Reflect<E>::getPtr(ct::Indexer<I>()).name << " ";
        os << V;
        return os;
    }

    template <class T>
    ostream& operator<<(ostream& os, ct::EnumField<T> v)
    {
        if (std::is_same<decltype(v.value()), uint8_t>::value)
        {
            os << v.name << " " << static_cast<int32_t>(v.value());
        }
        else
        {
            // os << v.name << " " << v.value();
            os << v.value();
        }

        return os;
    }

    template <class T>
    void printEnumHelper(ostream& os, T v, ct::Indexer<0> idx)
    {
        if (ct::Reflect<T>::getPtr(idx).value() == v)
        {
            os << ct::Reflect<T>::getPtr(idx).name;
        }
        else
        {
            os << "Invalid value " << v;
        }
    }

    template <class T, ct::index_t I>
    void printEnumHelper(ostream& os, T v, ct::Indexer<I> idx)
    {
        if (ct::Reflect<T>::getPtr(idx).value() == v)
        {
            os << ct::Reflect<T>::getPtr(idx).name;
        }
        else
        {
            printEnumHelper(os, v, --idx);
        }
    }

    template <class T>
    ct::EnableIfIsEnum<T, ostream&> operator<<(ostream& os, T v)
    {
        printEnumHelper(os, v, ct::Reflect<T>::end());
        return os;
    }
}

#define ENUM_START(NAME, TYPE)                                                                                                                                                     \
    struct NAME : ct::EnumBase<NAME, TYPE>                                                                                                                                         \
    {                                                                                                                                                                              \
        using EnumValueType = TYPE;                                                                                                                                                \
        using EnumType = NAME;                                                                                                                                                     \
        constexpr NAME(TYPE v) : EnumBase<NAME, TYPE>(v) {}                                                                                                                        \
        template <TYPE V, uint16_t I>                                                                                                                                              \
        constexpr NAME(ct::EnumValue<NAME, TYPE, V, I>) : EnumBase<NAME, TYPE>(V)                                                                                                  \
        {                                                                                                                                                                          \
        }                                                                                                                                                                          \
        REFLECT_STUB

#define ENUM_VALUE(NAME, VALUE)                                                                                                                                                    \
    static constexpr const ct::EnumValue<EnumType, EnumValueType, VALUE, __COUNTER__ - REFLECT_COUNT_START> NAME = {};                                                             \
    static constexpr auto getPtr(ct::Indexer<NAME.index>) { return ct::makeEnumField<ct::EnumValue<EnumType, EnumValueType, VALUE, NAME.index>>(#NAME); }

#define ENUM(NAME)                                                                                                                                                                 \
    static constexpr auto getPtr(ct::Indexer<__COUNTER__ - REFLECT_COUNT_START> idx)                                                                                               \
    {                                                                                                                                                                              \
        return ct::makeEnumField<ct::EnumValue<DataType, decltype(DataType::NAME), DataType::NAME, idx.index>>(#NAME);                                                             \
    }

#define ENUM_END                                                                                                                                                                   \
    static constexpr const ct::index_t NUM_FIELDS = __COUNTER__ - REFLECT_COUNT_START;                                                                                             \
    }

#endif // CT_ENUM_HPP