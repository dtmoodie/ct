#pragma once
#include "reflect/accessor.hpp"

#include <ct/Hash.hpp>
#include <ct/Indexer.hpp>

#include <cstdint>
#include <utility>
namespace ct
{
    template <class T>
    struct Reflect
    {
        static const bool SPECIALIZED = false;
        static constexpr const char* getName() { return ""; }
    };

    template <class T, class U = void>
    using enable_if_reflected = typename std::enable_if<Reflect<T>::SPECIALIZED, U>::type;

    template <class T, class U = void>
    using enable_if_not_reflected = typename std::enable_if<!Reflect<T>::SPECIALIZED, U>::type;

    template <class T, index_t I>
    using AccessorType = decltype(ct::Reflect<T>::getAccessor(Indexer<I>{}));

    template <class T, index_t I>
    struct GetterType
    {
        using accessor_type = AccessorType<T, I>;
        using type = typename accessor_type::GetType;
    };

    template <class T, index_t I>
    struct SetterType
    {
        using accessor_type = AccessorType<T, I>;
        using type = typename accessor_type::SetType;
    };

    template <class T, index_t I>
    struct GetterTraits
    {
        using accessor_type = AccessorType<T, I>;
        using type = typename accessor_type::GetterTraits_t;
    };

    template <class T, index_t I>
    struct SetterTraits
    {
        using accessor_type = AccessorType<T, I>;
        using type = typename accessor_type::SetterTraits_t;
    };

    template <index_t I, class T>
    constexpr const char* getName()
    {
        return Reflect<T>::getName(ct::Indexer<I>{});
    }

    template <index_t I, class T>
    typename GetterType<T, I>::type get(const T& obj)
    {
        auto accessor = Reflect<T>::getAccessor(Indexer<I>{});
        return accessor.get(obj);
    }

    template <index_t I, class T>
    auto set(T& obj) -> decltype(Reflect<T>::getAccessor(Indexer<I>{}).set(obj))
    {
        auto accessor = Reflect<T>::getAccessor(Indexer<I>{});
        return accessor.set(obj);
    }

    template <class T, index_t I, class U = void>
    using enable_if_member_getter =
        typename std::enable_if<std::is_same<typename GetterTraits<T, I>::type, DefaultGetterTraits>::value, U>::type;

    template <class T, index_t I, class U = void>
    using enable_if_member_setter =
        typename std::enable_if<std::is_same<typename SetterTraits<T, I>::type, DefaultGetterTraits>::value, U>::type;

    template <class T, index_t I, class U = void>
    using disable_if_member_getter =
        typename std::enable_if<!std::is_same<typename GetterTraits<T, I>::type, DefaultGetterTraits>::value, U>::type;

    template <class T, index_t I, class U = void>
    using disable_if_member_setter =
        typename std::enable_if<!std::is_same<typename SetterTraits<T, I>::type, DefaultGetterTraits>::value, U>::type;
}
#define CT_PP_CAT(a, b) CT_PP_CAT_I(a, b)
#if _MSC_VER
#define CT_PP_CAT_I(a, b) a##b
#else
#define CT_PP_CAT_I(a, b) CT_PP_CAT_II(~, a##b)
#define CT_PP_CAT_II(p, res) res
#endif
#define CT_PP_EMPTY()

#define CT_PP_VARIADIC_SIZE_I(e0,                                                                                      \
                              e1,                                                                                      \
                              e2,                                                                                      \
                              e3,                                                                                      \
                              e4,                                                                                      \
                              e5,                                                                                      \
                              e6,                                                                                      \
                              e7,                                                                                      \
                              e8,                                                                                      \
                              e9,                                                                                      \
                              e10,                                                                                     \
                              e11,                                                                                     \
                              e12,                                                                                     \
                              e13,                                                                                     \
                              e14,                                                                                     \
                              e15,                                                                                     \
                              e16,                                                                                     \
                              e17,                                                                                     \
                              e18,                                                                                     \
                              e19,                                                                                     \
                              e20,                                                                                     \
                              e21,                                                                                     \
                              e22,                                                                                     \
                              e23,                                                                                     \
                              e24,                                                                                     \
                              e25,                                                                                     \
                              e26,                                                                                     \
                              e27,                                                                                     \
                              e28,                                                                                     \
                              e29,                                                                                     \
                              e30,                                                                                     \
                              e31,                                                                                     \
                              e32,                                                                                     \
                              e33,                                                                                     \
                              e34,                                                                                     \
                              e35,                                                                                     \
                              e36,                                                                                     \
                              e37,                                                                                     \
                              e38,                                                                                     \
                              e39,                                                                                     \
                              e40,                                                                                     \
                              e41,                                                                                     \
                              e42,                                                                                     \
                              e43,                                                                                     \
                              e44,                                                                                     \
                              e45,                                                                                     \
                              e46,                                                                                     \
                              e47,                                                                                     \
                              e48,                                                                                     \
                              e49,                                                                                     \
                              e50,                                                                                     \
                              e51,                                                                                     \
                              e52,                                                                                     \
                              e53,                                                                                     \
                              e54,                                                                                     \
                              e55,                                                                                     \
                              e56,                                                                                     \
                              e57,                                                                                     \
                              e58,                                                                                     \
                              e59,                                                                                     \
                              e60,                                                                                     \
                              e61,                                                                                     \
                              e62,                                                                                     \
                              e63,                                                                                     \
                              size,                                                                                    \
                              ...)                                                                                     \
    size

#if _MSC_VER
#define CT_PP_VARIADIC_SIZE(...)                                                                                       \
    CT_PP_CAT(CT_PP_VARIADIC_SIZE_I(__VA_ARGS__,                                                                       \
                                    64,                                                                                \
                                    63,                                                                                \
                                    62,                                                                                \
                                    61,                                                                                \
                                    60,                                                                                \
                                    59,                                                                                \
                                    58,                                                                                \
                                    57,                                                                                \
                                    56,                                                                                \
                                    55,                                                                                \
                                    54,                                                                                \
                                    53,                                                                                \
                                    52,                                                                                \
                                    51,                                                                                \
                                    50,                                                                                \
                                    49,                                                                                \
                                    48,                                                                                \
                                    47,                                                                                \
                                    46,                                                                                \
                                    45,                                                                                \
                                    44,                                                                                \
                                    43,                                                                                \
                                    42,                                                                                \
                                    41,                                                                                \
                                    40,                                                                                \
                                    39,                                                                                \
                                    38,                                                                                \
                                    37,                                                                                \
                                    36,                                                                                \
                                    35,                                                                                \
                                    34,                                                                                \
                                    33,                                                                                \
                                    32,                                                                                \
                                    31,                                                                                \
                                    30,                                                                                \
                                    29,                                                                                \
                                    28,                                                                                \
                                    27,                                                                                \
                                    26,                                                                                \
                                    25,                                                                                \
                                    24,                                                                                \
                                    23,                                                                                \
                                    22,                                                                                \
                                    21,                                                                                \
                                    20,                                                                                \
                                    19,                                                                                \
                                    18,                                                                                \
                                    17,                                                                                \
                                    16,                                                                                \
                                    15,                                                                                \
                                    14,                                                                                \
                                    13,                                                                                \
                                    12,                                                                                \
                                    11,                                                                                \
                                    10,                                                                                \
                                    9,                                                                                 \
                                    8,                                                                                 \
                                    7,                                                                                 \
                                    6,                                                                                 \
                                    5,                                                                                 \
                                    4,                                                                                 \
                                    3,                                                                                 \
                                    2,                                                                                 \
                                    1, ), )
#else
#define CT_PP_VARIADIC_SIZE(...)                                                                                       \
    CT_PP_VARIADIC_SIZE_I(__VA_ARGS__,                                                                                 \
                          64,                                                                                          \
                          63,                                                                                          \
                          62,                                                                                          \
                          61,                                                                                          \
                          60,                                                                                          \
                          59,                                                                                          \
                          58,                                                                                          \
                          57,                                                                                          \
                          56,                                                                                          \
                          55,                                                                                          \
                          54,                                                                                          \
                          53,                                                                                          \
                          52,                                                                                          \
                          51,                                                                                          \
                          50,                                                                                          \
                          49,                                                                                          \
                          48,                                                                                          \
                          47,                                                                                          \
                          46,                                                                                          \
                          45,                                                                                          \
                          44,                                                                                          \
                          43,                                                                                          \
                          42,                                                                                          \
                          41,                                                                                          \
                          40,                                                                                          \
                          39,                                                                                          \
                          38,                                                                                          \
                          37,                                                                                          \
                          36,                                                                                          \
                          35,                                                                                          \
                          34,                                                                                          \
                          33,                                                                                          \
                          32,                                                                                          \
                          31,                                                                                          \
                          30,                                                                                          \
                          29,                                                                                          \
                          28,                                                                                          \
                          27,                                                                                          \
                          26,                                                                                          \
                          25,                                                                                          \
                          24,                                                                                          \
                          23,                                                                                          \
                          22,                                                                                          \
                          21,                                                                                          \
                          20,                                                                                          \
                          19,                                                                                          \
                          18,                                                                                          \
                          17,                                                                                          \
                          16,                                                                                          \
                          15,                                                                                          \
                          14,                                                                                          \
                          13,                                                                                          \
                          12,                                                                                          \
                          11,                                                                                          \
                          10,                                                                                          \
                          9,                                                                                           \
                          8,                                                                                           \
                          7,                                                                                           \
                          6,                                                                                           \
                          5,                                                                                           \
                          4,                                                                                           \
                          3,                                                                                           \
                          2,                                                                                           \
                          1, )
#endif

#define CT_PP_OVERLOAD(prefix, ...) CT_PP_CAT(prefix, CT_PP_VARIADIC_SIZE(__VA_ARGS__))

#define REFLECT_BEGIN(TYPE)                                                                                            \
    template <>                                                                                                        \
    struct Reflect<TYPE>                                                                                               \
    {                                                                                                                  \
        using DataType = TYPE;                                                                                         \
        static constexpr const char* getName() { return #TYPE; }                                                       \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const index_t I0 = 0;                                                                         \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;

#define REFLECT_DERIVED(TYPE, BASE)                                                                                    \
    template <>                                                                                                        \
    struct Reflect<TYPE> : private Reflect<BASE>                                                                       \
    {                                                                                                                  \
        using DataType = TYPE;                                                                                         \
        static constexpr const char* getName() { return #TYPE; }                                                       \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const ct::index_t I0 = Reflect<BASE>::REFLECTION_COUNT;                                       \
        static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__;                                          \
        template <ct::index_t I>                                                                                       \
                static constexpr auto getAccessor(const ct::Indexer<I> idx) -> typename std::enable_if < I             \
                >= 0 &&                                                                                                \
            I<Reflect<BASE>::REFLECTION_COUNT, decltype(Reflect<BASE>::getAccessor(idx))>::type                        \
        {                                                                                                              \
            return Reflect<BASE>::getAccessor(idx);                                                                    \
        }                                                                                                              \
        template <ct::index_t I>                                                                                       \
                static constexpr auto getName(const ct::Indexer<I> idx) -> typename std::enable_if < I                 \
                >= 0 &&                                                                                                \
            I<Reflect<BASE>::REFLECTION_COUNT, const char*>::type                                                      \
        {                                                                                                              \
            return Reflect<BASE>::getName(idx);                                                                        \
        }

#define REFLECT_TEMPLATED_START(TYPE)                                                                                  \
    template <class... Args>                                                                                           \
    struct Reflect<TYPE<Args...>>                                                                                      \
    {                                                                                                                  \
        using DataType = TYPE<Args...>;                                                                                \
        static constexpr const char* getName() { return #TYPE; }                                                       \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const index_t I0 = 0;                                                                         \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;

#define REFLECT_INTERNAL_START(TYPE)                                                                                   \
    static constexpr ct::index_t INTERNALLY_REFLECTED = 1;                                                             \
    static constexpr const char* getName() { return #TYPE; }                                                           \
    static constexpr const ct::index_t I0 = 0;                                                                         \
    static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__;                                              \
    using DataType = TYPE;

#define PUBLIC_ACCESS(NAME) PUBLIC_ACCESS_(NAME, __COUNTER__)

#define PUBLIC_ACCESS_(NAME, N)                                                                                        \
    static ct::Accessor<const decltype(DataType::NAME)& (*)(const DataType&),                                          \
                        decltype(DataType::NAME)& (*)(DataType&)>                                                      \
    getAccessor(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>)                                                   \
    {                                                                                                                  \
        return {[](const DataType& obj) -> const decltype(DataType::NAME)& { return obj.NAME; },                       \
                [](DataType& obj) -> decltype(DataType::NAME)& { return obj.NAME; }};                                  \
    }                                                                                                                  \
    static constexpr const char* getName(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>) { return #NAME; }

#define REFLECT_INTERNAL_MEMBER_2(TYPE, NAME)                                                                          \
    TYPE NAME;                                                                                                         \
                                                                                                                       \
  public:                                                                                                              \
    PUBLIC_ACCESS(NAME)

#define REFLECT_INTERNAL_MEMBER_3(TYPE, NAME, INIT)                                                                    \
    TYPE NAME = INIT;                                                                                                  \
                                                                                                                       \
  public:                                                                                                              \
    PUBLIC_ACCESS(NAME)

#ifdef _MSC_VER
#define REFLECT_INTERNAL_MEMBER(...)                                                                                   \
    CT_PP_CAT(CT_PP_OVERLOAD(REFLECT_INTERNAL_MEMBER_, __VA_ARGS__)(__VA_ARGS__), CT_PP_EMPTY())
#else
#define REFLECT_INTERNAL_MEMBER(...) CT_PP_OVERLOAD(REFLECT_INTERNAL_MEMBER_, __VA_ARGS__)(__VA_ARGS__)
#endif

#define ACCESSOR(NAME, GETTER, SETTER) ACCESSOR_(NAME, GETTER, SETTER, __COUNTER__)

#define ACCESSOR_(NAME, GETTER, SETTER, N)                                                                             \
    static constexpr auto getAccessor(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>)                             \
        ->decltype(ct::makeAccessor(GETTER, SETTER))                                                                   \
    {                                                                                                                  \
        return ct::makeAccessor(GETTER, SETTER);                                                                       \
    }                                                                                                                  \
    static constexpr const char* getName(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>) { return #NAME; }

#define MEMBER_FUNCTION(NAME, FPTR)                                                                                    \
    static auto getAccessor(const ct::Indexer<I0 + __COUNTER__ - REFLECT_COUNT_START - 1>)                             \
        ->decltype(ct::makeAccessor<CalculatedValue>(FPTR))                                                            \
    {                                                                                                                  \
        return ct::makeAccessor<CalculatedValue>(FPTR);                                                                \
    }

#define REFLECT_END                                                                                                    \
    static constexpr const index_t REFLECT_COUNT_END = __COUNTER__;                                                    \
    static constexpr const index_t REFLECTION_COUNT = I0 + REFLECT_COUNT_END - REFLECT_COUNT_START - 1;                \
    static constexpr const index_t N = REFLECTION_COUNT - 1;                                                           \
    static constexpr ct::Indexer<N> end() { return ct::Indexer<N>{}; }                                                 \
    }

#define REFLECT_INTERNAL_END                                                                                           \
    static constexpr const ct::index_t REFLECT_COUNT_END = __COUNTER__;                                                \
    static constexpr const ct::index_t REFLECTION_COUNT = I0 + REFLECT_COUNT_END - REFLECT_COUNT_START - 1;            \
    static constexpr const ct::index_t N = REFLECTION_COUNT - 1;                                                       \
    static constexpr ct::Indexer<N> end() { return ct::Indexer<N>{}; }
