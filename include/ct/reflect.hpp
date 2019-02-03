#pragma once
#include <ct/reflect/member_pointer.hpp>

#include "macros.hpp"
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
        return Reflect<T>::getPtr(ct::Indexer<I>{}).m_name;
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

#define PUBLIC_ACCESS(NAME)                                                                                            \
    constexpr static auto getPtr(const ct::Indexer<I0 + __COUNTER__ - REFLECT_COUNT_START - 1>)                        \
      -> decltype(ct::makeMemberObjectPointer(#NAME, &DataType::NAME))                                                 \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(#NAME, &DataType::NAME);                                                    \
    }

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

#define PROPERTY(NAME, GETTER, SETTER)                                                                             \
    static constexpr auto getPtr(const ct::Indexer<I0 + __COUNTER__ - REFLECT_COUNT_START - 1>)                           \
        ->decltype(ct::makeMemberPropertyPointer(#NAME, GETTER, SETTER))                                                                   \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer(#NAME, GETTER, SETTER);                                                                       \
    }

#define MEMBER_FUNCTION(NAME, ...)                                                                                     \
    static auto getPtr(const ct::Indexer<I0 + __COUNTER__ - REFLECT_COUNT_START - 1>)                                  \
        ->decltype(ct::makeMemberFunctionPointers(#NAME, __VA_ARGS__))                                                 \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers(#NAME, __VA_ARGS__);                                                     \
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
