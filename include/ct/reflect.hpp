#pragma once
#include <ct/Indexer.hpp>
#include <ct/Hash.hpp>
#include <utility>
#include <cstdint>
#include "reflect/accessor.hpp"
namespace ct
{
    template<class T>
    struct Reflect{static const bool SPECIALIZED = false;};

    template<class T, class U = void>
    using enable_if_reflected = typename std::enable_if<Reflect<T>::SPECIALIZED, U>::type;

    template<class T, class U = void>
    using enable_if_not_reflected = typename std::enable_if<!Reflect<T>::SPECIALIZED, U>::type;


    template<class T, index_t I>
    using AccessorType = decltype(ct::Reflect<T>::getAccessor(Indexer<I>{}));

    template<class T, index_t I>
    struct GetterType
    {
        using accessor_type = AccessorType<T, I>;
        using type = typename accessor_type::GetType;
    };

    template<class T, index_t I>
    struct SetterType
    {
        using accessor_type = AccessorType<T, I>;
        using type = typename accessor_type::SetType;
    };

    template<class T, index_t I>
    struct GetterTraits
    {
        using accessor_type = AccessorType<T, I>;
        using type = typename accessor_type::GetterTraits_t;
    };
    template<index_t I, class T>
    constexpr const char* getName()
    {
        return Reflect<T>::getName(ct::Indexer<I>{});
    }

    template<index_t I, class T>
    typename GetterType<T, I>::type get(const T& obj)
    {
        auto accessor = Reflect<T>::getAccessor(Indexer<I>{});
        return accessor.get(obj);
    }

    template<index_t I, class T>
    typename SetterType<T, I>::type set(T& obj)
    {
        auto accessor = Reflect<T>::getAccessor(Indexer<I>{});
        return accessor.set(obj);
    }
}

#define REFLECT_BEGIN(TYPE) \
    template<> struct Reflect<TYPE>{ \
        using DataType = TYPE; \
        static constexpr const char* getName(){return #TYPE;} \
        static constexpr const bool SPECIALIZED = true; \
        static constexpr const index_t I0 = 0; \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;

#define REFLECT_DERIVED(TYPE, BASE) \
    template<> struct Reflect<TYPE>: private Reflect<BASE>{ \
        using DataType = TYPE; \
        static constexpr const char* getName(){return #TYPE;} \
        static constexpr const bool SPECIALIZED = true; \
        static constexpr const ct::index_t I0 = Reflect<BASE>::REFLECTION_COUNT; \
        static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__; \
        template<ct::index_t I> \
        static constexpr auto getAccessor(const ct::Indexer<I> idx) -> typename std::enable_if<I >= 0 && I < Reflect<BASE>::REFLECTION_COUNT, decltype(Reflect<BASE>::getAccessor(idx))>::type \
            {return Reflect<BASE>::getAccessor(idx);} \
        template<ct::index_t I> \
        static constexpr auto getName(const ct::Indexer<I> idx) -> typename std::enable_if<I >= 0 && I < Reflect<BASE>::REFLECTION_COUNT, const char*>::type {return Reflect<BASE>::getName(idx);}

#define REFLECT_TEMPLATED_START(TYPE) \
    template<class...Args> struct Reflect<TYPE<Args...>>{ \
        using DataType = TYPE<Args...>; \
        static constexpr const char* getName(){return #TYPE;} \
        static constexpr const bool SPECIALIZED = true; \
        static constexpr const index_t I0 = 0; \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;


#define REFLECT_INTERNAL_START(TYPE) \
    static constexpr ct::index_t INTERNALLY_REFLECTED = 1; \
    static constexpr const char* getName(){return #TYPE;} \
    static constexpr const ct::index_t I0 = 0; \
    static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__; \
    using DataType = TYPE;

#define PUBLIC_ACCESS(NAME) PUBLIC_ACCESS_(NAME, __COUNTER__)

#define PUBLIC_ACCESS_(NAME, N) \
    static ct::Accessor<const decltype(DataType::NAME)&(*)(const DataType&), decltype(DataType::NAME)&(*)(DataType&)> \
        getAccessor(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>){\
            return {\
                [](const DataType& obj)-> const decltype(DataType::NAME)&{return obj.NAME;  }, \
                [](DataType& obj)-> decltype(DataType::NAME)&{ return obj.NAME; }};} \
    static constexpr const char* getName(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>){ return #NAME; }

#define REFLECT_INTERNAL_MEMBER(TYPE, NAME) \
    TYPE NAME; \
    public: PUBLIC_ACCESS(NAME)

#define ACCESSOR(NAME, GETTER, SETTER) ACCESSOR_(NAME, GETTER, SETTER, __COUNTER__)

#define ACCESSOR_(NAME, GETTER, SETTER, N) \
    static constexpr auto getAccessor(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>) -> decltype(ct::makeAccessor(GETTER, SETTER)) { return ct::makeAccessor(GETTER, SETTER); } \
    static constexpr const char* getName(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>) { return #NAME; }

#define MEMBER_FUNCTION(NAME, FPTR) \
    static auto getAccessor(const ct::Indexer<I0 + __COUNTER__ - REFLECT_COUNT_START - 1>) -> decltype(ct::makeAccessor<CalculatedValue>(FPTR)) { return ct::makeAccessor<CalculatedValue>(FPTR); }

#define REFLECT_END \
    static constexpr const index_t REFLECT_COUNT_END = __COUNTER__; \
    static constexpr const index_t REFLECTION_COUNT = I0 + REFLECT_COUNT_END - REFLECT_COUNT_START - 1; \
    static constexpr const index_t N = REFLECTION_COUNT - 1; \
    static constexpr ct::Indexer<N> end(){return ct::Indexer<N>{};} \
    }

#define REFLECT_INTERNAL_END  \
    static constexpr const ct::index_t REFLECT_COUNT_END = __COUNTER__; \
    static constexpr const ct::index_t REFLECTION_COUNT = I0 + REFLECT_COUNT_END - REFLECT_COUNT_START - 1; \
    static constexpr const index_t N = REFLECTION_COUNT - 1; \
    static constexpr ct::Indexer<N> end() { return ct::Indexer<N>{}; }

