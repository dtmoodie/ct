#ifndef CT_REFLECT_MACROS_HPP
#define CT_REFLECT_MACROS_HPP
#include "macros.hpp"
#include "reflect/metadata.hpp"

#define REFLECT_INTERNAL_MEMBER_2(TYPE, NAME)                                                                          \
    TYPE NAME;                                                                                                         \
                                                                                                                       \
  public:                                                                                                              \
    PUBLIC_ACCESS(NAME)

#define REFLECT_INTERNAL_MEMBER_3(TYPE, NAME, INIT)                                                                    \
    TYPE NAME = INIT;                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    static inline TYPE init_##NAME() { return INIT; }                                                                  \
  public:                                                                                                              \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(                                                                            \
            #NAME, &DataType::NAME, ct::metadata::makeInitializer(&DataType::init_##NAME, #INIT));                     \
    }

#define REFLECT_INTERNAL_MEMBER_4(TYPE, NAME, INIT, METADATA)                                                          \
    TYPE NAME = INIT;                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    static inline TYPE init_##NAME() { return INIT; }                                                                  \
  public:                                                                                                              \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(                                                                            \
            #NAME,                                                                                                     \
            &DataType::NAME,                                                                                           \
            ct::metadata::makePack(ct::metadata::makeInitializer(&DataType::init_##NAME, #INIT), METADATA));           \
    }

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
        static constexpr const bool SPECIALIZED = true;                                                                \
        REFLECT_STUB

#define REFLECT_DERIVED(TYPE, ...)                                                                                     \
    template <>                                                                                                        \
    struct ReflectImpl<TYPE>                                                                                           \
    {                                                                                                                  \
        using DataType = TYPE;                                                                                         \
        using BaseTypes = ct::VariadicTypedef<__VA_ARGS__>;                                                            \
        static constexpr const bool SPECIALIZED = true;                                                                \
        REFLECT_STUB

#define REFLECT_TEMPLATED_START(TYPE)                                                                                  \
    template <class... Args>                                                                                           \
    struct ReflectImpl<TYPE<Args...>>                                                                                  \
    {                                                                                                                  \
        using DataType = TYPE<Args...>;                                                                                \
        using TemplateParameters = ct::VariadicTypedef<Args...>;                                                       \
        static constexpr const bool SPECIALIZED = true;                                                                \
        REFLECT_STUB

#define REFLECT_STUB static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__ + 1;

#define REFLECT_INTERNAL_START                                                                                         \
    REFLECT_STUB                                                                                                       \
        static constexpr auto getTypeHelper()->decltype(this);                                                         \
        using DataType = typename std::remove_pointer<decltype(getTypeHelper())>::type;

#define REFLECT_INTERNAL_DERIVED(...)                                                                                  \
    REFLECT_STUB                                                                                                       \
        static constexpr auto getTypeHelper()->decltype(this);                                                         \
        using DataType = typename std::remove_pointer<decltype(getTypeHelper())>::type;                                \
        using BaseTypes = ct::VariadicTypedef<__VA_ARGS__>;

#define PUBLIC_ACCESS(NAME)                                                                                            \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(#NAME, &DataType::NAME);                                                    \
    }

#define PUBLIC_ACCESS_WITH_METADATA(NAME, METADATA)                                                                    \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(#NAME, &DataType::NAME, METADATA);                                          \
    }

#define PROPERTY_1(NAME)                                                                                               \
    static constexpr auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer(#NAME, &DataType::NAME, nullptr);                                         \
    }

#define PROPERTY_2(NAME, GETTER)                                                                                       \
    static constexpr auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer(#NAME, GETTER, nullptr);                                                  \
    }

#define PROPERTY_3(NAME, GETTER, SETTER)                                                                               \
    static constexpr auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer(#NAME, GETTER, SETTER);                                                   \
    }

#ifdef _MSC_VER
#define PROPERTY(...) CT_PP_CAT(CT_PP_OVERLOAD(PROPERTY_, __VA_ARGS__)(__VA_ARGS__), CT_PP_EMPTY())
#else
#define PROPERTY(...) CT_PP_OVERLOAD(PROPERTY_, __VA_ARGS__)(__VA_ARGS__)
#endif

#define PROPERTY_WITH_FLAG_1(FLAG, NAME)                                                                               \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer<FLAG>(#NAME, &DataType::NAME, nullptr);                                   \
    }

#define PROPERTY_WITH_FLAG_2(FLAG, NAME, GETTER)                                                                       \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer<FLAG>(#NAME, GETTER, nullptr);                                            \
    }

#define PROPERTY_WITH_FLAG_3(FLAG, NAME, GETTER, SETTER)                                                               \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer<FLAG>(#NAME, GETTER, SETTER);                                             \
    }

#ifdef _MSC_VER
#define PROPERTY_WITH_FLAG(FLAG, ...)                                                                                  \
    CT_PP_CAT(CT_PP_OVERLOAD(PROPERTY_WITH_FLAG_, __VA_ARGS__)(FLAG, __VA_ARGS__), CT_PP_EMPTY())
#else
#define PROPERTY_WITH_FLAG(FLAG, ...) CT_PP_OVERLOAD(PROPERTY_WITH_FLAG_, __VA_ARGS__)(FLAG, __VA_ARGS__)

#endif

#define MEMBER_FUNCTION_1(NAME)                                                                                        \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers<DataType>(#NAME, &DataType::NAME);                                       \
    }

#define MEMBER_FUNCTION_N(NAME, ...)                                                                                   \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)                                 \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers<DataType>(#NAME, __VA_ARGS__);                                           \
    }

#define MEMBER_FUNCTION_2(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__);
#define MEMBER_FUNCTION_3(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__);
#define MEMBER_FUNCTION_4(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__);
#define MEMBER_FUNCTION_5(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__);
#define MEMBER_FUNCTION_6(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__);
#define MEMBER_FUNCTION_7(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__);
#define MEMBER_FUNCTION_8(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__);
#define MEMBER_FUNCTION_9(NAME, ...) MEMBER_FUNCTION_N(NAME, __VA_ARGS__);

#ifdef _MSC_VER
#define MEMBER_FUNCTION(...) CT_PP_CAT(CT_PP_OVERLOAD(MEMBER_FUNCTION_, __VA_ARGS__)(__VA_ARGS__), CT_PP_EMPTY())
#else
#define MEMBER_FUNCTION(...) CT_PP_OVERLOAD(MEMBER_FUNCTION_, __VA_ARGS__)(__VA_ARGS__)
#endif

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

#define REFLECT_INTERNAL_END                                                                                           \
    static constexpr const ct::index_t REFLECT_COUNT_END = __COUNTER__;                                                \
    static constexpr const ct::index_t REFLECTION_COUNT = REFLECT_COUNT_END - REFLECT_COUNT_START;                     \
    static constexpr const ct::index_t NUM_FIELDS = REFLECTION_COUNT

#define REFLECT_END                                                                                                    \
    REFLECT_INTERNAL_END                                                                                               \
    ;                                                                                                                  \
    }

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
#define REFLECT_INTERNAL_END ;
#define REFLECT_END

#endif
#endif // CT_REFLECT_MACROS_HPP
