#ifndef CT_ENUM_BITSET_HPP
#define CT_ENUM_BITSET_HPP
#include "enum.hpp"

#include <bitset>

namespace ct
{
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

    template <class E, uint8_t VALUE, uint16_t I>
    struct BitsetIndex : public BitsetTag
    {
        static constexpr uint16_t index = I;
        constexpr BitsetIndex() = default;
        constexpr uint64_t toBitset() const { return uint64_t(1) << VALUE; }
        constexpr operator uint64_t() const { return uint64_t(1) << VALUE; }

#ifdef _MSC_VER
        static constexpr T value = 1 << VALUE;
#else
        static constexpr E value = E(uint64_t(1) << VALUE);
#endif
    };

    template <class T, class E, uint8_t V, uint16_t I>
    constexpr bool operator==(T v, BitsetIndex<E, V, I>)
    {
        return v == V;
    }

    template <class T, class E, uint8_t V, uint16_t I>
    constexpr bool operator!=(T v, BitsetIndex<E, V, I>)
    {
        return v != V;
    }

    template <typename T, typename STORAGE = uint64_t>
    struct EnumBitset : BitsetTag
    {
        static constexpr uint8_t MAX_BIT = sizeof(STORAGE) * 8;

        constexpr EnumBitset(STORAGE v = 0) : m_data{v} {}

        template <class U, uint8_t V, uint16_t I>
        constexpr bool test(BitsetIndex<U, V, I> idx) const
        {
            return m_data & indexToBit(idx);
        }

        bool test(STORAGE bitset) const
        {
            for (int i = 0; i < 8 * sizeof(STORAGE); ++i)
            {
                const STORAGE bit = (static_cast<STORAGE>(1) << i);
                if (bitset & bit && !(m_data & bit))
                {
                    return false;
                }
            }
            return true;
        }

        void reset(STORAGE bitset) { m_data = m_data & (~bitset); }

        template <class U, uint8_t V, uint16_t I>
        void reset(BitsetIndex<U, V, I> idx)
        {
            reset(idx.toBitset());
        }

        template <class U, uint8_t V, uint16_t I>
        void set(BitsetIndex<U, V, I> idx)
        {
            m_data = m_data | indexToBit(idx);
        }

        void set(STORAGE bitset) { m_data = m_data | bitset; }

        // TODO implement flip
        template <class U, uint8_t V, uint16_t I>
        void flip(BitsetIndex<U, V, I>)
        {
        }

        void flip(STORAGE v) { m_data = m_data ^ v; }

        constexpr operator STORAGE() const { return m_data; }

        template <uint8_t V, uint16_t I>
        EnumBitset& operator=(BitsetIndex<T, V, I> v)
        {
            m_data = v;
            return *this;
        }

        // This should only be used when this EnumBitset represents a single set bit, otherwise it doesn't make sense
        template <uint8_t V, uint16_t I>
        constexpr bool operator>(BitsetIndex<T, V, I> v) const
        {
            return m_data > v.toBitset();
        }

        template <uint8_t V, uint16_t I>
        constexpr bool operator<(BitsetIndex<T, V, I> v) const
        {
            return m_data < v.toBitset();
        }

        template <uint8_t V, uint16_t I>
        constexpr bool operator<=(BitsetIndex<T, V, I> v) const
        {
            return m_data <= v.toBitset();
        }

        template <uint8_t V, uint16_t I>
        constexpr bool operator>=(BitsetIndex<T, V, I> v) const
        {
            return m_data >= v.toBitset();
        }

      private:
        template <class U, uint8_t V, uint16_t I>
        constexpr STORAGE indexToBit(BitsetIndex<U, V, I>) const
        {
            static_assert(V < MAX_BIT, "Can only do bitsets up to 64 bits for now :/");
            return static_cast<STORAGE>(1) << V;
        }

        STORAGE m_data;
    };

    template <class E, class T>
    EnumBitset<E, T>& operator++(EnumBitset<E, T>& v)
    {
        T val = v;
        val = val << 1;
        v = EnumBitset<E, T>(val);
        return v;
    }

    template <class E, class T>
    EnumBitset<E, T> operator++(EnumBitset<E, T>& v, int)
    {
        T val = v;
        val = val << 1;
        v = EnumBitset<E, T>(val);
        return v;
    }

    template <class E, uint8_t V1, uint16_t I>
    constexpr E operator|(E e, BitsetIndex<E, V1, I>)
    {
        return E(uint64_t(e) | (1 << V1));
    }

    template <class E, uint8_t V1, uint8_t V2, uint16_t I1, uint16_t I2>
    constexpr E operator|(BitsetIndex<E, V1, I1> b0, BitsetIndex<E, V2, I2> b1)
    {
        return E(b0.toBitset() | b1.toBitset());
    }

    template <class E, class T, T V1, uint8_t V2, uint16_t I1, uint16_t I2>
    constexpr E operator|(EnumValue<E, T, V1, I1> b0, BitsetIndex<E, V2, I2> b1)
    {
        return E(b0 | b1.toBitset());
    }

    template <class E, class T, T V1, uint8_t V2, uint16_t I1, uint16_t I2>
    constexpr E operator|(BitsetIndex<E, V2, I2> b1, EnumValue<E, T, V1, I1> b0)
    {
        return E(b1.toBitset() | b0);
    }

    template <class E, uint8_t V1, uint16_t I>
    constexpr E operator|(BitsetIndex<E, V1, I> idx, E e)
    {
        return E(idx | uint64_t(e));
    }

    template <class E, uint8_t V1, uint16_t I>
    constexpr bool operator&(E val, BitsetIndex<E, V1, I>)
    {
        return val & (1 << V1);
    }

    template <class E, uint8_t V1, uint16_t I>
    constexpr bool operator&(BitsetIndex<E, V1, I>, E val)
    {
        return (1 << V1) & val;
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
    T bitsetFromString(StringView str, char deliminator = '|')
    {
        StringView rest = str;
        T output;
        auto pos = rest.find(deliminator);
        while (pos != std::string::npos)
        {
            auto substr = rest.substr(0, pos);
            rest = rest.substr(pos + 1);
            auto flag = ct::fromString<T>(substr);
            if (flag.success())
            {
                output.set(flag.value());
            }
            pos = rest.find(deliminator);
        }
        auto flag = ct::fromString<T>(rest);
        if (flag.success())
        {
            output.set(flag.value());
        }
        return output;
    }
}

#ifndef __NVCC__

#define BITSET_START(NAME)                                                                                             \
    struct NAME : ct::EnumBitset<NAME>                                                                                 \
    {                                                                                                                  \
        using EnumValueType = uint64_t;                                                                                \
        using EnumType = NAME;                                                                                         \
        template <uint8_t V, uint16_t I>                                                                               \
        using EnumValue = ct::BitsetIndex<NAME, V, I>;                                                                 \
        constexpr NAME(uint64_t v = 0) : ct::EnumBitset<NAME>(v) {}                                                    \
        template <uint8_t V, uint16_t I>                                                                               \
        constexpr NAME(ct::BitsetIndex<NAME, V, I> v) : ct::EnumBitset<NAME>(v.toBitset())                             \
        {                                                                                                              \
        }                                                                                                              \
        REFLECT_STUB

#define ENUM_BITVALUE(NAME, VALUE)                                                                                     \
    CT_INLINE_VAR ct::BitsetIndex<EnumType, static_cast<uint8_t>(VALUE), static_cast<uint16_t>(__COUNTER__ - REFLECT_COUNT_START)> NAME =    \
        {};                                                                                                            \
    static constexpr auto getPtr(ct::Indexer<NAME.index>)                                                              \
    {                                                                                                                  \
        return ct::makeEnumField<ct::BitsetIndex<EnumType, static_cast<uint8_t>(VALUE), NAME.index>>(#NAME);                                 \
    }

#else // ifndef __NVCC__

#define BITSET_START(NAME)                                                                                             \
    struct NAME : ct::EnumBitset<NAME>                                                                                 \
    {                                                                                                                  \
        using EnumValueType = uint64_t;                                                                                \
        using EnumType = NAME;                                                                                         \
        template <uint8_t V, uint16_t I>                                                                               \
        using EnumValue = ct::BitsetIndex<NAME, V, I>;                                                                 \
        constexpr NAME(uint64_t v = 0) : ct::EnumBitset<NAME>(v) {}                                                    \
        template <uint8_t V, uint16_t I>                                                                               \
        constexpr NAME(ct::BitsetIndex<NAME, V, I> v) : ct::EnumBitset<NAME>(v.toBitset())                             \
        {                                                                                                              \
        }                                                                                                              \
        REFLECT_STUB

#define ENUM_BITVALUE(NAME, VALUE) static constexpr const EnumValueType NAME = static_cast<uint64_t>(1) << VALUE;
#endif // ifndef __NVCC__

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