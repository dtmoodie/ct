#pragma once
#include <ct/reflect/member_pointer.hpp>

#include "Hash.hpp"
#include "Indexer.hpp"
#include "VariadicTypedef.hpp"

#include <cstdint>
#include <utility>

namespace ct
{
    template <class T, class ENABLE = void>
    struct Reflect
    {
        static const bool SPECIALIZED = false;
        using Base = ct::VariadicTypedef<void>;
        static constexpr const char* getName() { return ""; }
    };

    template<class T>
    struct Reflect<T, typename std::enable_if<T::INTERNALLY_REFLECTED>::type>
    {
        static const bool SPECIALIZED = true;
        static constexpr const char* getName() {return T::getName(); }
        static constexpr const ct::index_t I0 = T::I0;
        static constexpr const ct::index_t REFLECT_COUNT_START = T::REFLECT_COUNT_START;
        using DataType = T;

        template<index_t I>
        static auto getPtr(const Indexer<I> idx) -> decltype(T::getPtr(idx)){return T::getPtr(idx);}

        template<index_t I>
        static auto getName(const Indexer<I> idx) -> decltype(T::getName(idx)){return T::getName(idx);}

        static constexpr const ct::index_t REFLECT_COUNT_END = T::REFLECT_COUNT_END;
        static constexpr const ct::index_t REFLECTION_COUNT = T::REFLECTION_COUNT;
        static constexpr const ct::index_t N = T::N;
        static constexpr ct::Indexer<N> end() { return ct::Indexer<N>{}; }
    };

    template <class T, class U = void>
    using EnableIfReflected = typename std::enable_if<Reflect<T>::SPECIALIZED, U>::type;

    template <class T, class U = void>
    using DisableIfReflected = typename std::enable_if<!Reflect<T>::SPECIALIZED, U>::type;

    template <class T, index_t I>
    using PtrType = decltype(ct::Reflect<T>::getPtr(Indexer<I>{}));

    template <class T, index_t I>
    struct IsMemberFunction
    {
        using Accessor_t = PtrType<T, I>;
        static constexpr const bool value = IsMemberFunctionPointers<Accessor_t>::value;
    };

    template<class T, index_t I>
    struct IsMemberObject
    {
        using Accesosr_t = PtrType<T, I>;
        static constexpr const bool value = IsMemberObjectPointer<Accesosr_t>::value;
    };

    template <index_t I, class T>
    constexpr const char* getName()
    {
        return Reflect<T>::getName(ct::Indexer<I>{});
    }

    template <index_t I, class T>
    auto set(T& obj) -> decltype(Reflect<T>::getPtr(Indexer<I>{}).set(obj))
    {
        auto accessor = Reflect<T>::getPtr(Indexer<I>{});
        return accessor.set(obj);
    }

    template<class T, index_t I>
    struct IsReadable
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = getFlags<type>() & READABLE;
    };

    template<class T, index_t I>
    struct IsWritable
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = getFlags<type>() & WRITABLE;
    };

    template<class T, index_t I>
    struct ShouldSerialize
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = !(getFlags<type>() & DO_NOT_SERIALIZE);
    };

    template<class T, index_t I>
    struct FieldGetType
    {
        using Ptr_t = PtrType<T, I>;
        using type = typename GetType<Ptr_t>::type;
    };

    template <class T, index_t I, class U = void>
    using EnableIfIsReadable = typename std::enable_if<IsReadable<T, I>::value, U>::type;

    template <class T, index_t I, class U = void>
    using EnableIfIsWritable = typename std::enable_if<IsWritable<T, I>::value, U>::type;

    template<class T, index_t I, class U = void>
    using EnableIfIsMemberObject = typename std::enable_if<IsMemberObject<T, I>::value, U>::type;

    template<class T, index_t I, class U = void>
    using DisableIfIsMemberObject = typename std::enable_if<!IsMemberObject<T, I>::value, U>::type;

    template <class T, index_t I, class U = void>
    using DisableIfIsReadable = typename std::enable_if<!IsReadable<T, I>::value, U>::type;

    template <class T, index_t I, class U = void>
    using DisableIfIsWritable = typename std::enable_if<!IsWritable<T, I>::value, U>::type;

    template<class T, index_t I, class ENABLE = EnableIfIsMemberObject<T, I>>
    struct GlobMemberObjectsHelper
    {
        using Ptr_t = PtrType<T, I>;
        using type = typename std::decay<typename GetType<Ptr_t>::type>::type;
        using types = typename Append<typename GlobMemberObjectsHelper<T, I-1, void>::types, type>::type;
    };

    template<class T, index_t I>
    struct GlobMemberObjectsHelper<T, I, DisableIfIsMemberObject<T, I>>
    {
        using types = typename GlobMemberObjectsHelper<T, I-1, void>::types;
    };

    template<class T>
    struct GlobMemberObjectsHelper<T, 0, EnableIfIsMemberObject<T, 0>>
    {
        using Ptr_t = PtrType<T, 0>;
        using types = VariadicTypedef<typename std::decay<typename GetType<Ptr_t>::type>::type>;
    };

    template<class T>
    struct GlobMemberObjectsHelper<T, 0, DisableIfIsMemberObject<T, 0>>
    {
        using types = VariadicTypedef<void>;
    };

    template<class T>
    struct GlobMemberObjects
    {
        using types = typename GlobMemberObjectsHelper<T, Reflect<T>::N, void>::types;
    };
} // namespace ct

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

#ifdef _MSC_VER
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
        using Base = VariadicTypedef<void>;                                                                            \
        static constexpr const char* getName() { return #TYPE; }                                                       \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const index_t I0 = 0;                                                                         \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;

#define REFLECT_DERIVED(TYPE, BASE)                                                                                    \
    template <>                                                                                                        \
    struct Reflect<TYPE> : private Reflect<BASE>                                                                       \
    {                                                                                                                  \
        using DataType = TYPE;                                                                                         \
        using Base = ct::VariadicTypedef<BASE>;                                                                        \
        static constexpr const char* getName() { return #TYPE; }                                                       \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const ct::index_t I0 = Reflect<BASE>::REFLECTION_COUNT;                                       \
        static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__;                                          \
        template <ct::index_t I>                                                                                       \
                static constexpr auto getPtr(const ct::Indexer<I> idx) -> typename std::enable_if < I             \
                >= 0 &&                                                                                                \
            I<Reflect<BASE>::REFLECTION_COUNT, decltype(Reflect<BASE>::getPtr(idx))>::type                        \
        {                                                                                                              \
            return Reflect<BASE>::getPtr(idx);                                                                    \
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
        using Base = ct::VariadicTypedef<void>;                                                                        \
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
    constexpr static auto getPtr(const ct::Indexer<I0 + (N) - REFLECT_COUNT_START - 1>)                           \
      -> decltype(ct::makeMemberObjectPointer(&DataType::NAME))                                                            \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(&DataType::NAME);                                                     \
    }                                                                                                                  \
    static constexpr const char* getName(const ct::Indexer<I0 + (N) - REFLECT_COUNT_START - 1>) { return #NAME; }

#define REFLECT_INTERNAL_MEMBER_2(TYPE, NAME)                                                                          \
    TYPE NAME;                                                                                                         \
  public:                                                                                                              \
    PUBLIC_ACCESS(NAME)

#define REFLECT_INTERNAL_MEMBER_3(TYPE, NAME, INIT)                                                                    \
    TYPE NAME = INIT;                                                                                                  \
  public:                                                                                                              \
    PUBLIC_ACCESS(NAME)

#ifdef _MSC_VER
#define REFLECT_INTERNAL_MEMBER(...)                                                                                   \
    CT_PP_CAT(CT_PP_OVERLOAD(REFLECT_INTERNAL_MEMBER_, __VA_ARGS__)(__VA_ARGS__), CT_PP_EMPTY())
#else
#define REFLECT_INTERNAL_MEMBER(...)                                                                                   \
    CT_PP_OVERLOAD(REFLECT_INTERNAL_MEMBER_, __VA_ARGS__)(__VA_ARGS__)
#endif

#define PROPERTY(NAME, GETTER, SETTER) PROPERTY_(NAME, GETTER, SETTER, __COUNTER__)

#define PROPERTY_(NAME, GETTER, SETTER, N)                                                                             \
    static constexpr auto getPtr(const ct::Indexer<I0 + (N) - REFLECT_COUNT_START - 1>)                           \
        ->decltype(ct::makeMemberPropertyPointer(GETTER, SETTER))                                                                   \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer(GETTER, SETTER);                                                                       \
    }                                                                                                                  \
    static constexpr const char* getName(const ct::Indexer<I0 + (N) - REFLECT_COUNT_START - 1>) { return #NAME; }

#define MEMBER_FUNCTION_IMPL(NAME, FPTR, N)                                                                            \
    static auto getPtr(const ct::Indexer<I0 + (N) - REFLECT_COUNT_START - 1>)                                     \
        ->decltype(ct::makeMemberFunctionPointers(FPTR))                                                            \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers(FPTR);                                                                \
    }                                                                                                                  \
    static constexpr const char* getName(const ct::Indexer<I0 + (N) - REFLECT_COUNT_START - 1>) { return #NAME; }

#define MEMBER_FUNCTION_2(NAME, FPTR) MEMBER_FUNCTION_IMPL(NAME, FPTR, __COUNTER__)

#define MEMBER_FUNCTION_3(NAME, FPTR1, FPTR2)                                                                          \
    MEMBER_FUNCTION_IMPL(NAME, FPTR1, __COUNTER__)                                                                     \
    MEMBER_FUNCTION_IMPL(NAME, FPTR2, __COUNTER__)

#define MEMBER_FUNCTION_1(NAME) MEMBER_FUNCTION_IMPL(NAME, &DataType::NAME, __COUNTER__)

#define MEMBER_FUNCTION(...) CT_PP_CAT(CT_PP_OVERLOAD(MEMBER_FUNCTION_, __VA_ARGS__)(__VA_ARGS__), CT_PP_EMPTY())

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
