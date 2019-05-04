#ifndef CT_ENUM_HPP
#define CT_ENUM_HPP
#include "reflect.hpp"
#include "reflect_macros.hpp"

#include "StringView.hpp"
#include <type_traits>

namespace ct
{

    template <class E, class T, T VALUE>
    struct EnumTag
    {
        constexpr operator T() { return VALUE; }
        static constexpr T value = VALUE;
    };
    template <class T>
    struct EnumValue
    {
        constexpr auto value() { return T::value; }
        StringView name;
    };

    template <class T>
    constexpr EnumValue<T> makeEnumValue(StringView name)
    {
        return EnumValue<T>{name};
    }

    template <class T>
    struct IsEnumValue : std::false_type
    {
    };

    template <class T>
    struct IsEnumValue<EnumValue<T>> : std::true_type
    {
    };

    template <class T, class U = void>
    using EnableIfIsEnum = ct::EnableIf<IsEnumValue<typename ct::PtrType<T, 0>>::value, U>;

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
}

namespace std
{
    template <class E, class T, T V>
    ostream& operator<<(ostream& os, ct::EnumTag<E, T, V>)
    {
        os << V;
        return os;
    }

    template <class T>
    ostream& operator<<(ostream& os, ct::EnumValue<T> v)
    {
        os << v.name << " " << v.value();
        return os;
    }

    template <class T>
    void printEnumHelper(ostream& os, T v, ct::Indexer<0> idx)
    {
        if (ct::Reflect<T>::getPtr(idx).value() == v.value)
        {
            os << ct::Reflect<T>::getPtr(idx).name;
        }
        else
        {
            os << "Invalid value " << v.value;
        }
    }

    template <class T, ct::index_t I>
    void printEnumHelper(ostream& os, T v, ct::Indexer<I> idx)
    {
        if (ct::Reflect<T>::getPtr(idx).value() == v.value)
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

#define ENUM_START(NAME, TYPE)                                                                                         \
    struct NAME                                                                                                        \
    {                                                                                                                  \
        using EnumValueType = TYPE;                                                                                    \
        using EnumType = NAME;                                                                                         \
        EnumValueType value;                                                                                           \
        NAME(EnumValueType v) : value(v) {}                                                                            \
        NAME& operator=(EnumValueType v) { value = v; }                                                                \
        REFLECT_STUB

#define ENUM_VALUE(NAME, VALUE)                                                                                        \
    using NAME = ct::EnumTag<EnumType, EnumValueType, VALUE>;                                                          \
    static constexpr auto getPtr(ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                       \
    {                                                                                                                  \
        return ct::makeEnumValue<NAME>(#NAME);                                                                         \
    }

#define ENUM_END                                                                                                       \
    static constexpr const ct::index_t NUM_FIELDS = __COUNTER__ - REFLECT_COUNT_START;                                 \
    }

#endif // CT_ENUM_HPP
