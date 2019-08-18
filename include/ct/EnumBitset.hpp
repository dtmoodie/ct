#ifndef CT_ENUM_BITSET_HPP
#define CT_ENUM_BITSET_HPP
#include "enum.hpp"
#include "reflect_traits.hpp"

#include <bitset>

namespace ct
{
    template <class T, class E = void>
    struct UnderlyingType
    {
        using type = typename std::underlying_type<T>::type;
    };

    template <class T>
    struct EnumTraits;

    template <class T, class E = void>
    struct EnumMax
    {
        constexpr static const uint32_t value = EnumTraits<T>::max;
    };
    template <class T>
    struct UnderlyingType<T, EnableIfIsEnum<T>>
    {
        using type = typename T::EnumValueType;
    };

    template <class T>
    constexpr T max(T v0, T v1)
    {
        return v0 > v1 ? v0 : v1;
    }

    template <class T>
    constexpr typename T::EnumValueType enumMax(Indexer<0> idx)
    {
        return Reflect<T>::getPtr(idx).value();
    }

    template <class T, index_t I>
    constexpr typename T::EnumValueType enumMax(Indexer<I> idx)
    {
        return max(static_cast<typename T::EnumValueType>(Reflect<T>::getPtr(idx).value()), enumMax<T>(--idx));
    }

    template <class T>
    struct EnumMax<T, EnableIfIsEnum<T>>
    {
        constexpr static typename T::EnumValueType value = enumMax<T>(Reflect<T>::end()) + 1;
    };

    template <typename T>
    class EnumBitset
    {
      public:
        using UnderlyingType = typename UnderlyingType<T>::type;

        constexpr EnumBitset() : c() {}
        constexpr EnumBitset(UnderlyingType v) : c(v) {}

        constexpr bool test(T pos) const { return c.test(get_value(pos)); }

        template <UnderlyingType V, index_t I>
        constexpr EnumBitset(EnumValue<T, UnderlyingType, V, I, true>) : c(V)
        {
        }

        constexpr EnumBitset(EnumBase<T, UnderlyingType> v) : c(v.value) {}
        constexpr EnumBitset(T v) : c(v.value) {}

        template <UnderlyingType V>
        static constexpr EnumBitset fromBitValue()
        {
            return EnumBitset(T(V));
        }

        constexpr EnumBitset& reset(T pos)
        {
            c.reset(get_value(pos));
            return *this;
        }

        constexpr EnumBitset& set(T pos)
        {
            c.set(get_value(pos));
            return *this;
        }

        constexpr EnumBitset& flip(T pos)
        {
            c.flip(get_value(pos));
            return *this;
        }

        constexpr operator unsigned long long() const { return c.to_ullong(); }

      private:
        constexpr void multiSet(T v) { c.set(v); }
        template <class... ARGS>
        constexpr void multiSet(T v, ARGS... args)
        {
            c.set(v);
            multiSet(args...);
        }
        std::bitset<static_cast<UnderlyingType>(EnumMax<T>::value)> c;
        UnderlyingType get_value(T v) const { return static_cast<UnderlyingType>(v); }
    };

    // Bitset overloads return a bitset

    template <class E, class T, T V1, T V2, uint16_t I, uint16_t J>
    constexpr EnumBase<E, T> operator|(EnumValue<E, T, V1, I, true>, EnumValue<E, T, V2, J, true>)
    {
        return EnumBase<E, T>((T{} | (1 << V1)) | (1 << V2));
    }

    template <class E, class T, T V1, uint16_t I>
    constexpr EnumBitset<E> operator|(EnumValue<E, T, V1, I, true>, E e)
    {
        return EnumBitset<E>(V1, e.value);
    }

    template <class T, ct::index_t I>
    void bitsetPrint(std::ostream& out, EnumBitset<T> type, ct::Indexer<I> idx, bool& empty)
    {
        auto ptr = T::getPtr(idx);
        if (type.test(ptr.value()))
        {
            if (empty)
            {
                out << ptr.name;
                empty = false;
            }
            else
            {
                out << "|" << ptr.name;
            }
        }
    }

    template <class T>
    void bitsetRecurse(std::ostream& out, EnumBitset<T> type, ct::Indexer<0> idx, bool& empty)
    {
        bitsetPrint(out, type, idx, empty);
    }

    template <class T, ct::index_t I>
    void bitsetRecurse(std::ostream& out, EnumBitset<T> type, ct::Indexer<I> idx, bool& empty)
    {
        bitsetRecurse(out, type, --idx, empty);
        bitsetPrint(out, type, idx, empty);
    }

    template <class T>
    EnumBitset<T> bitsetFromString(StringView str, char deliminator = '|')
    {
        StringView rest = str;
        EnumBitset<T> output;
        auto pos = rest.find(deliminator);
        while (pos != std::string::npos)
        {
            auto substr = rest.substr(0, pos);
            rest = rest.substr(pos + 1);
            auto flag = ct::fromString<T>(substr);
            if (flag.success())
            {
                output.flip(flag.value());
            }
            pos = rest.find(deliminator);
        }
        auto flag = ct::fromString<T>(rest);
        if (flag.success())
        {
            output.flip(flag.value());
        }
        return output;
    }
}

namespace std
{
    template <class T>
    ostream& operator<<(ostream& os, ct::EnumBitset<T> v)
    {
        bool empty = true;
        ct::bitsetRecurse(os, v, ct::Reflect<T>::end(), empty);
        return os;
    }
}

#endif // CT_ENUM_BITSET_HPP
