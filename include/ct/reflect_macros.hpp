#ifndef CT_REFLECT_MACROS_HPP
#define CT_REFLECT_MACROS_HPP
#include "macros.hpp"
/*#include "reflect/MemberFunctionPointer.hpp"
#include "reflect/MemberObjectPointer.hpp"
#include "reflect/MemberPropertyPointer.hpp"
#include "reflect/metadata.hpp"*/

#define REFLECT_INTERNAL_MEMBER_2(TYPE, NAME)                                                                          \
    TYPE NAME;                                                                                                         \
                                                                                                                       \
  public:                                                                                                              \
    constexpr static ct::MemberObjectPointer<TYPE DataType::*, ct::Flags::READABLE | ct::Flags::WRITABLE> getPtr(      \
        const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                                          \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(#NAME, &DataType::NAME);                                                    \
    }

#define REFLECT_INTERNAL_MEMBER_3(TYPE, NAME, INIT)                                                                    \
    TYPE NAME = INIT;                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    static inline TYPE init_##NAME() { return INIT; }                                                                  \
                                                                                                                       \
  public:                                                                                                              \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(                                                                            \
            #NAME, &DataType::NAME, ct::makeInitializer(&DataType::init_##NAME, #INIT));                               \
    }

#define REFLECT_INTERNAL_MEMBER_4(TYPE, NAME, INIT, METADATA)                                                          \
    TYPE NAME = INIT;                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    static inline TYPE init_##NAME() { return INIT; }                                                                  \
                                                                                                                       \
  public:                                                                                                              \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(                                                                            \
            #NAME, &DataType::NAME, ct::makePack(ct::makeInitializer(&DataType::init_##NAME, #INIT), METADATA));       \
    }

#ifdef _MSC_VER
#define REFLECT_INTERNAL_MEMBER(...)                                                                                   \
    CT_PP_CAT(CT_PP_OVERLOAD(REFLECT_INTERNAL_MEMBER_, __VA_ARGS__)(__VA_ARGS__), CT_PP_EMPTY())
#else
#define REFLECT_INTERNAL_MEMBER(...) CT_PP_OVERLOAD(REFLECT_INTERNAL_MEMBER_, __VA_ARGS__)(__VA_ARGS__)
#endif

#define REFLECT_STUB static constexpr const ct::index_t REFLECT_COUNT_BEGIN = __COUNTER__ + 1;

#define REFLECT_BEGIN(TYPE)                                                                                            \
    template <>                                                                                                        \
    struct ReflectImpl<TYPE>                                                                                           \
    {                                                                                                                  \
        using DataType = TYPE;                                                                                         \
        static constexpr ct::StringView getName() { return #TYPE; }                                                    \
        REFLECT_STUB

#define REFLECT_DERIVED(TYPE, ...)                                                                                     \
    template <>                                                                                                        \
    struct ReflectImpl<TYPE>                                                                                           \
    {                                                                                                                  \
        using DataType = TYPE;                                                                                         \
        using BaseTypes = ct::VariadicTypedef<__VA_ARGS__>;                                                            \
        static constexpr ct::StringView getName() { return #TYPE; }                                                    \
        REFLECT_STUB

#define REFLECT_TEMPLATED_BEGIN(TYPE)                                                                                  \
    template <class... Args>                                                                                           \
    struct ReflectImpl<TYPE<Args...>>                                                                                  \
    {                                                                                                                  \
        using DataType = TYPE<Args...>;                                                                                \
        using TemplateParameters = ct::VariadicTypedef<Args...>;                                                       \
        REFLECT_STUB

#define REFLECT_INTERNAL_BEGIN(TYPE)                                                                                   \
    REFLECT_STUB                                                                                                       \
        using DataType = TYPE;                                                                                         \
        static constexpr ct::StringView getName() { return #TYPE; }

#define INFER_THIS_TYPE                                                                                                \
    static constexpr auto getTypeHelper()->decltype(this);                                                             \
    using DataType = typename std::remove_pointer<decltype(getTypeHelper())>::type;

#define REFLECT_INTERNAL_DERIVED(TYPE, ...)                                                                            \
    REFLECT_STUB                                                                                                       \
        using DataType = TYPE;                                                                                         \
        using BaseTypes = ct::VariadicTypedef<__VA_ARGS__>;

#define PUBLIC_ACCESS(NAME)                                                                                            \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
        ->decltype(ct::makeMemberObjectPointer(#NAME, &DataType::NAME))                                                \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(#NAME, &DataType::NAME);                                                    \
    }

#define PUBLIC_ACCESS_WITH_METADATA(NAME, METADATA)                                                                    \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(#NAME, &DataType::NAME, METADATA);                                          \
    }

#define PROPERTY_1(NAME)                                                                                               \
    static constexpr auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer(#NAME, &DataType::NAME, nullptr);                                         \
    }

#define PROPERTY_2(NAME, GETTER)                                                                                       \
    static constexpr auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer(#NAME, GETTER, nullptr);                                                  \
    }

#define PROPERTY_3(NAME, GETTER, SETTER)                                                                               \
    static constexpr auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer(#NAME, GETTER, SETTER);                                                   \
    }

#ifdef _MSC_VER
#define PROPERTY(...) CT_PP_CAT(CT_PP_OVERLOAD(PROPERTY_, __VA_ARGS__)(__VA_ARGS__), CT_PP_EMPTY())
#else
#define PROPERTY(...) CT_PP_OVERLOAD(PROPERTY_, __VA_ARGS__)(__VA_ARGS__)
#endif

#define PROPERTY_WITH_FLAG_1(FLAG, NAME)                                                                               \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer<FLAG>(#NAME, &DataType::NAME, nullptr);                                   \
    }

#define PROPERTY_WITH_FLAG_2(FLAG, NAME, GETTER)                                                                       \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer<FLAG>(#NAME, GETTER, nullptr);                                            \
    }

#define PROPERTY_WITH_FLAG_3(FLAG, NAME, GETTER, SETTER)                                                               \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer<FLAG>(#NAME, GETTER, SETTER);                                             \
    }

#ifdef _MSC_VER
#define PROPERTY_WITH_FLAG(FLAG, ...)                                                                                  \
    CT_PP_CAT(CT_PP_OVERLOAD(PROPERTY_WITH_FLAG_, __VA_ARGS__)(uint64_t(FLAG), __VA_ARGS__), CT_PP_EMPTY())
#else
#define PROPERTY_WITH_FLAG(FLAG, ...) CT_PP_OVERLOAD(PROPERTY_WITH_FLAG_, __VA_ARGS__)(FLAG, __VA_ARGS__)

#endif

#define MEMBER_FUNCTION_1(NAME)                                                                                        \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers<DataType>(#NAME, &DataType::NAME);                                       \
    }

#define MEMBER_FUNCTION_N(NAME, ...)                                                                                   \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers<DataType>(#NAME, __VA_ARGS__);                                           \
    }

#define MEMBER_FUNCTION_2(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__)
#define MEMBER_FUNCTION_3(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__)
#define MEMBER_FUNCTION_4(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__)
#define MEMBER_FUNCTION_5(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__)
#define MEMBER_FUNCTION_6(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__)
#define MEMBER_FUNCTION_7(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__)
#define MEMBER_FUNCTION_8(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__)
#define MEMBER_FUNCTION_9(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__)

#ifdef _MSC_VER
#define MEMBER_FUNCTION(...) CT_PP_CAT(CT_PP_OVERLOAD(MEMBER_FUNCTION_, __VA_ARGS__)(__VA_ARGS__), CT_PP_EMPTY())
#else
#define MEMBER_FUNCTION(...) CT_PP_OVERLOAD(MEMBER_FUNCTION_, __VA_ARGS__)(__VA_ARGS__)
#endif

#define MEMBER_FUNCTION_WITH_FLAG(FLAG, NAME, ...)                                                                     \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers<DataType, FLAG>(#NAME, __VA_ARGS__);                                     \
    }

#define STATIC_FUNCTION(NAME, ...)                                                                                     \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeStaticFunctionPointers<DataType>(#NAME, __VA_ARGS__);                                           \
    }

#define STATIC_FUNCTION_WITH_FLAG(FLAG, NAME, ...)                                                                     \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)                                 \
    {                                                                                                                  \
        return ct::makeStaticFunctionPointers<DataType, FLAG>(#NAME, __VA_ARGS__);                                     \
    }

#define REFLECT_INTERNAL_END static constexpr const ct::index_t NUM_FIELDS = __COUNTER__ - REFLECT_COUNT_BEGIN

#define REFLECT_END                                                                                                    \
    REFLECT_INTERNAL_END                                                                                               \
    ;                                                                                                                  \
    }

#ifndef __NVCC__

#define ENUM_BEGIN(NAME, TYPE)                                                                                         \
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

#define ENUM_VALUE_(NAME, VALUE, N)                                                                                    \
    CT_INLINE_VAR                                                                                                      \
    ct::EnumValue<EnumType, EnumValueType, VALUE, N> NAME = {};                                                        \
    static constexpr ct::EnumField<ct::EnumValue<EnumType, EnumValueType, VALUE, N>> getPtr(ct::Indexer<N>)            \
    {                                                                                                                  \
        return ct::makeEnumField<ct::EnumValue<EnumType, EnumValueType, VALUE, N>>(#NAME);                             \
    }

#define ENUM_VALUE(NAME, VALUE) ENUM_VALUE_(NAME, VALUE, static_cast<uint16_t>(__COUNTER__ - REFLECT_COUNT_BEGIN))

#define ENUM_(NAME, N)                                                                                                 \
    static constexpr ct::EnumField<ct::EnumValue<DataType, decltype(DataType::NAME), DataType::NAME, N>> getPtr(       \
        ct::Indexer<N>)                                                                                                \
    {                                                                                                                  \
        return ct::makeEnumField<ct::EnumValue<DataType, decltype(DataType::NAME), DataType::NAME, N>>(#NAME);         \
    }

#define ENUM(NAME) ENUM_(NAME, __COUNTER__ - REFLECT_COUNT_BEGIN)

#define ENUM_END                                                                                                       \
    static constexpr const ct::index_t NUM_FIELDS = __COUNTER__ - REFLECT_COUNT_BEGIN;                                 \
    }

#else // defined(__NVCC__)

#define ENUM_BEGIN(NAME, TYPE)                                                                                         \
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
    static constexpr const ct::index_t NUM_FIELDS = __COUNTER__ - REFLECT_COUNT_BEGIN;                                 \
    }
#endif

#ifndef __NVCC__

#define BITSET_BEGIN(NAME)                                                                                             \
    struct NAME : ct::EnumBitset<NAME>                                                                                 \
    {                                                                                                                  \
        using EnumValueType = uint64_t;                                                                                \
        using EnumType = NAME;                                                                                         \
        template <uint16_t V, uint16_t I>                                                                              \
        using EnumValue = ct::BitsetIndex<NAME, V, I>;                                                                 \
        constexpr NAME(uint64_t v = 0) : ct::EnumBitset<NAME>(v) {}                                                    \
        template <uint16_t V, uint16_t I>                                                                              \
        constexpr NAME(ct::BitsetIndex<NAME, V, I> v) : ct::EnumBitset<NAME>(v.toBitset())                             \
        {                                                                                                              \
        }                                                                                                              \
        template <uint16_t V, uint16_t I>                                                                              \
        NAME& operator=(ct::BitsetIndex<NAME, V, I> v)                                                                 \
        {                                                                                                              \
            ct::EnumBitset<NAME>::operator=(v);                                                                        \
            return *this;                                                                                              \
        }                                                                                                              \
        REFLECT_STUB

#define ENUM_BITVALUE(NAME, VALUE)                                                                                     \
    CT_INLINE_VAR ct::                                                                                                 \
        BitsetIndex<EnumType, static_cast<uint16_t>(VALUE), static_cast<uint16_t>(__COUNTER__ - REFLECT_COUNT_BEGIN)>  \
            NAME = {};                                                                                                 \
    static constexpr ct::EnumField<ct::BitsetIndex<EnumType, static_cast<uint16_t>(VALUE), NAME.index>> getPtr(        \
        ct::Indexer<NAME.index>)                                                                                       \
    {                                                                                                                  \
        return ct::makeEnumField<ct::BitsetIndex<EnumType, static_cast<uint16_t>(VALUE), NAME.index>>(#NAME);          \
    }

#else // ifndef __NVCC__

#define BITSET_BEGIN(NAME)                                                                                             \
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

#endif // CT_REFLECT_MACROS_HPP
