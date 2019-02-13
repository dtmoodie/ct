#ifndef CT_REFLECT_MACROS_HPP
#define CT_REFLECT_MACROS_HPP
#include "macros.hpp"

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

#ifndef __NVCC__

#define REFLECT_BEGIN(TYPE)                                                                                            \
    template <>                                                                                                        \
    struct ReflectImpl<TYPE>                                                                                           \
    {                                                                                                                  \
        using DataType = TYPE;                                                                                         \
        using BaseTypes = VariadicTypedef<>;                                                                           \
        static constexpr ct::StringView getName() { return #TYPE; }                                                    \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__ + 1;

#define REFLECT_DERIVED(TYPE, ...)                                                                                     \
    template <>                                                                                                        \
    struct ReflectImpl<TYPE>                                                                                           \
    {                                                                                                                  \
        using DataType = TYPE;                                                                                         \
        using BaseTypes = VariadicTypedef<__VA_ARGS__>;                                                                \
        static constexpr ct::StringView getName() { return #TYPE; }                                                    \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__ + 1;

#define REFLECT_TEMPLATED_START(TYPE)                                                                                  \
    template <class... Args>                                                                                           \
    struct ReflectImpl<TYPE<Args...>>                                                                                  \
    {                                                                                                                  \
        using DataType = TYPE<Args...>;                                                                                \
        using BaseTypes = VariadicTypedef<>;                                                                           \
        static constexpr ct::StringView getName() { return #TYPE; }                                                    \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__ + 1;

#define REFLECT_INTERNAL_START                                                                                         \
    static constexpr const bool INTERNALLY_REFLECTED = true;                                                           \
    static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__ + 1;                                          \
    static constexpr auto getTypeHelper()->typename std::remove_reference<decltype(*this)>::type;                      \
    using DataType = decltype(getTypeHelper());                                                                        \
    static CT_CONSTEXPR_NAME ct::StringView getName() { return ct::GetName<DataType>::getName(); }                     \
    using BaseTypes = ct::VariadicTypedef<>;

#define REFLECT_INTERNAL_DERIVED(...)                                                                                  \
    static constexpr const bool INTERNALLY_REFLECTED = true;                                                           \
    static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__ + 1;                                          \
    static constexpr auto getTypeHelper()->typename std::remove_reference<decltype(*this)>::type;                      \
    using DataType = decltype(getTypeHelper());                                                                        \
    static CT_CONSTEXPR_NAME ct::StringView getName() { return ct::GetName<DataType>::getName(); }                     \
    using BaseTypes = ct::VariadicTypedef<__VA_ARGS__>;

#define PUBLIC_ACCESS(NAME)                                                                                            \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(#NAME, &DataType::NAME);                                                    \
    }

#define PROPERTY(NAME, GETTER, SETTER)                                                                                 \
    static constexpr auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer(#NAME, GETTER, SETTER);                                                   \
    }

#define PROPERTY_WITH_FLAG(FLAG, NAME, GETTER, SETTER)                                                                 \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer<FLAG>(#NAME, GETTER, SETTER);                                             \
    }

#define MEMBER_FUNCTION(NAME, ...)                                                                                     \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers<DataType>(#NAME, __VA_ARGS__);                                           \
    }

#define MEMBER_FUNCTION_WITH_FLAG(FLAG, NAME, ...)                                                                     \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers<DataType, FLAG>(#NAME, __VA_ARGS__);                                     \
    }

#define STATIC_FUNCTION(NAME, ...)                                                                                     \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeStaticFunctionPointers<DataType>(#NAME, __VA_ARGS__);                                           \
    }

#define STATIC_FUNCTION_WITH_FLAG(FLAG, NAME, ...)                                                                     \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeStaticFunctionPointers<DataType, FLAG>(#NAME, __VA_ARGS__);                                     \
    }

#define REFLECT_END                                                                                                    \
    static constexpr const index_t REFLECT_COUNT_END = __COUNTER__;                                                    \
    static constexpr const index_t REFLECTION_COUNT = REFLECT_COUNT_END - REFLECT_COUNT_START;                         \
    static constexpr const index_t NUM_FIELDS = REFLECTION_COUNT;                                                      \
    }

#define REFLECT_INTERNAL_END                                                                                           \
    static constexpr const ct::index_t REFLECT_COUNT_END = __COUNTER__;                                                \
    static constexpr const ct::index_t REFLECTION_COUNT = REFLECT_COUNT_END - REFLECT_COUNT_START;                     \
    static constexpr const ct::index_t NUM_FIELDS = REFLECTION_COUNT;

#else

#define REFLECT_BEGIN(TYPE)
#define REFLECT_DERIVED(TYPE, ...)
#define REFLECT_TEMPLATED_START(TYPE)
#define REFLECT_INTERNAL_START
#define REFLECT_INTERNAL_DERIVED(...)
#define PUBLIC_ACCESS(NAME)
#define PROPERTY(NAME, GETTER, SETTER)
#define PROPERTY_WITH_FLAG(FLAG, NAME, GETTER, SETTER)
#define MEMBER_FUNCTION(NAME, ...)
#define MEMBER_FUNCTION_WITH_FLAG(FLAG, NAME, ...)
#define REFLECT_INTERNAL_END
#define REFLECT_END

#endif
#endif // CT_REFLECT_MACROS_HPP
