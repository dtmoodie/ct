#pragma once
#include <ct/reflect/member_pointer.hpp>

#include "Hash.hpp"
#include "Indexer.hpp"
#include "VariadicTypedef.hpp"
#include "macros.hpp"
#include "static_asserts.hpp"

#include <cstdint>
#include <ostream>
#include <utility>

namespace ct
{

    template <bool VAL, class U = void>
    using EnableIf = typename std::enable_if<VAL, U>::type;

    template <class T, class VISITED = VariadicTypedef<>, class ENABLE = void>
    struct Reflect
    {
        static const bool SPECIALIZED = false;
        using BaseTypes = VariadicTypedef<>;
        static constexpr const char* getName() { return ""; }
    };

    inline void printTypes(const ct::VariadicTypedef<>, std::ostream&) {}

    template <class T>
    void printTypes(const ct::VariadicTypedef<T>, std::ostream& os)
    {
        os << ct::Reflect<T>::getName();
    }

    template <class T, class... T1>
    void printTypes(const ct::VariadicTypedef<T, T1...>, std::ostream& os)
    {
        os << ct::Reflect<T>::getName() << ", ";
        printTypes(ct::VariadicTypedef<T1...>{}, os);
    }

    template <class T>
    struct ReflectImpl
    {
        constexpr static const bool SPECIALIZED = false;
    };

    struct EmptyImplementation
    {
        constexpr static const index_t NUM_FIELDS = 0;
        constexpr static const index_t START_INDEX = 0;
        constexpr static const index_t END_INDEX = 0;
        template <index_t I>
        constexpr static int getPtr(const Indexer<I>)
        {
            return 0;
        }
        static void printHierarchy(std::ostream&, const std::string& = "") {}
    };

    template <class T, class V>
    struct ReflectBasesImpl;

    // Recurse over the list of base types
    template <class T, class... BASES, class V>
    struct ReflectBasesImpl<VariadicTypedef<T, BASES...>, V>
    {
        using Impl = Reflect<T, V>;
        using Super =
            ReflectBasesImpl<VariadicTypedef<BASES...>, typename InsertUnique<typename Impl::VisitationList, V>::type>;

        using VisitationList =
            typename InsertUnique<typename Impl::VisitationList, typename Super::VisitationList>::type;
        using ClassInheritanceList =
            typename Append<typename Impl::ClassInheritanceList, typename Super::ClassInheritanceList>::type;

        constexpr static const index_t NUM_FIELDS = Reflect<T>::NUM_FIELDS + Super::NUM_FIELDS;
        constexpr static const index_t START_INDEX = Super::END_INDEX;
        constexpr static const index_t END_INDEX = START_INDEX + Reflect<T>::NUM_FIELDS;

        template <index_t I>
            constexpr static auto getPtr(const Indexer<I>) -> EnableIf < I
            >= START_INDEX&& I<END_INDEX, decltype(Impl::getPtr(Indexer<I - START_INDEX>{}))>
        {
            return Impl::getPtr(Indexer<I - START_INDEX>{});
        }

        template <index_t I>
            constexpr static auto getPtr(const Indexer<I> idx)
                -> EnableIf < I<START_INDEX, decltype(Super::getPtr(idx))>
        {
            return Super::getPtr(idx);
        }

        static void printHierarchy(std::ostream& os, const std::string& indent = "")
        {
            auto num_fields = NUM_FIELDS;
            auto start_index = START_INDEX;
            auto end_index = END_INDEX;
            Impl::printHierarchy(os, indent);
            Super::printHierarchy(os, indent);
        }
    };

    // End recursion
    template <class T, class V>
    struct ReflectBasesImpl<VariadicTypedef<T>, V>
    {
        using Impl = Reflect<T, V>;
        using VisitationListTmp = typename InsertUnique<T, typename Impl::VisitationList>::type;
        using VisitationList = typename InsertUnique<VisitationListTmp, V>::type;

        constexpr static const index_t NUM_FIELDS = Impl::NUM_FIELDS;
        constexpr static const index_t START_INDEX = 0;
        constexpr static const index_t END_INDEX = NUM_FIELDS + START_INDEX;

        using ClassInheritanceList = typename Impl::ClassInheritanceList;

        template <index_t I>
            constexpr static auto getPtr(const Indexer<I> idx) -> EnableIf < I<END_INDEX, decltype(Impl::getPtr(idx))>
        {
            return Impl::getPtr(idx);
        }

        static void printHierarchy(std::ostream& os, const std::string& indent = "")
        {
            auto num_fields = NUM_FIELDS;
            auto start_index = START_INDEX;
            auto end_index = END_INDEX;
            Impl::printHierarchy(os, indent);
        }
    };

    template <class T, class V>
    struct ReflectBases : public ReflectBasesImpl<T, V>
    {
    };

    // No bases specialization
    template <class V>
    struct ReflectBases<VariadicTypedef<>, V> : public EmptyImplementation
    {
        using ClassInheritanceList = VariadicTypedef<>;
        using VisitationList = V;
    };

    // This disables an implementation if it already exists in the visitation list
    // This prevents multiply including a base class in diamond inheritance
    template <class T, class IMPL, class VISITED, class ENABLE = void>
    struct ImplementationFilter;

    // Include the implementation
    template <class T, class IMPL, class VISITED>
    struct ImplementationFilter<T, IMPL, VISITED, EnableIf<!ContainsType<T, VISITED>::value>>
    {
        using DataType = T;
        using BaseTypes = typename IMPL::BaseTypes;

        using Bases_t = ReflectBases<typename IMPL::BaseTypes, typename Append<T, VISITED>::type>;
        using ClassInheritanceList = typename Append<T, typename Bases_t::ClassInheritanceList>::type;
        using VisitationList = typename Bases_t::VisitationList;

        static const bool SPECIALIZED = true;
        constexpr static const index_t NUM_FIELDS = IMPL::REFLECTION_COUNT + Bases_t::NUM_FIELDS;
        constexpr static const index_t START_INDEX = Bases_t::END_INDEX;
        constexpr static const index_t END_INDEX = START_INDEX + IMPL::REFLECTION_COUNT;

        constexpr static const char* getName() { return IMPL::getName(); }

        template <index_t I>
            constexpr static auto getPtr(const Indexer<I>) -> EnableIf < I
            >= START_INDEX&& I<END_INDEX, decltype(IMPL::getPtr(Indexer<I - START_INDEX>{}))>
        {
            return IMPL::getPtr(Indexer<I - START_INDEX>{});
        }

        template <index_t I>
            constexpr static auto getPtr(const Indexer<I> idx)
                -> EnableIf < I<START_INDEX, decltype(Bases_t::getPtr(idx))>
        {
            return Bases_t::getPtr(idx);
        }

        static void printHierarchy(std::ostream& os, const std::string& indent = "")
        {
            auto num_fields = NUM_FIELDS;
            auto start_index = START_INDEX;
            auto end_index = END_INDEX;
            os << indent << "Reflect<" << IMPL::getName() << ", Visited: ";
            printTypes(VISITED{}, os);
            os << "> (" << START_INDEX << ":" << IMPL::REFLECTION_COUNT << ":" << END_INDEX << ')' << std::endl;
            Bases_t::printHierarchy(os, indent + "  ");
        }

        static constexpr ct::Indexer<NUM_FIELDS - 1> end() { return ct::Indexer<NUM_FIELDS - 1>{}; }
    };

    // We've already visited this class, so exclude the implementation
    template <class T, class IMPL, class VISITED>
    struct ImplementationFilter<T, IMPL, VISITED, EnableIf<ContainsType<T, VISITED>::value>>
        : public EmptyImplementation
    {
        using ClassInheritanceList = VariadicTypedef<>;
        using VisitationList = VariadicTypedef<>;
    };

    // Externally defined reflection
    template <class T, class VISITED>
    struct Reflect<T, VISITED, EnableIf<ReflectImpl<T>::SPECIALIZED>>
        : public ImplementationFilter<T, ReflectImpl<T>, VISITED>
    {
    };

    // Internally defined reflection
    template <class T, class VISITED>
    struct Reflect<T, VISITED, EnableIf<T::INTERNALLY_REFLECTED>> : public ImplementationFilter<T, T, VISITED>
    {
    };

    template <class T, class U = void>
    using EnableIfReflected = EnableIf<Reflect<T>::SPECIALIZED, U>;

    template <class T, class U = void>
    using DisableIfReflected = EnableIf<!Reflect<T>::SPECIALIZED, U>;

    template <class T, index_t I>
    using PtrType = decltype(ct::Reflect<T>::getPtr(Indexer<I>{}));

    template <class T, index_t I>
    struct IsMemberFunction
    {
        using Accessor_t = PtrType<T, I>;
        static constexpr const bool value = IsMemberFunctionPointers<Accessor_t>::value;
    };

    template <class T, index_t I>
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

    template <class T>
    constexpr index_t indexOfFieldImpl(const char* field_name, const Indexer<0>)
    {
        return (strcmp(getName<0, T>(), field_name) == 0) ? 0 : -1;
    }

    template <class T, index_t I>
    constexpr index_t indexOfFieldImpl(const char* field_name, const Indexer<I> idx)
    {
        return (strcmp(getName<I, T>(), field_name) == 0) ? I : indexOfFieldImpl<T>(field_name, --idx);
    }

    template <class T>
    constexpr index_t indexOfField(const char* field_name)
    {
        return indexOfFieldImpl<T>(field_name, ct::Reflect<T>::end());
    }

    template <class T>
    constexpr bool haveField(const char* field_name)
    {
        return indexOfField<T>(field_name) != -1;
    }

    template <class T, index_t I>
    struct IsReadable
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = getFlags<type>() & READABLE;
    };

    template <class T, index_t I>
    struct IsWritable
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = getFlags<type>() & WRITABLE;
    };

    template <class T, index_t I>
    struct ShouldSerialize
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = !(getFlags<type>() & DO_NOT_SERIALIZE);
    };

    template <class T, index_t I>
    struct FieldGetType
    {
        using Ptr_t = PtrType<T, I>;
        using type = typename GetType<Ptr_t>::type;
    };

    template <class T, index_t I>
    struct FieldSetType
    {
        using Ptr_t = PtrType<T, I>;
        using type = typename SetType<Ptr_t>::type;
    };

    template <index_t I, class T>
    auto set(T& obj) -> typename FieldSetType<T, I>::type
    {
        auto ptr = Reflect<T>::getPtr(Indexer<I>{});
        return set(ptr, obj);
    }

    template <class T, index_t I, class U = void>
    using EnableIfIsReadable = EnableIf<IsReadable<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using EnableIfIsWritable = EnableIf<IsWritable<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using EnableIfMemberFunction = EnableIf<IsMemberFunction<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using DisableIfMemberFunction = EnableIf<!IsMemberFunction<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using EnableIfIsMemberObject = EnableIf<IsMemberObject<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using DisableIfIsMemberObject = EnableIf<!IsMemberObject<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using DisableIfIsReadable = EnableIf<!IsReadable<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using DisableIfIsWritable = EnableIf<!IsWritable<T, I>::value, U>;

    template <class T, index_t I, class ENABLE = EnableIfIsMemberObject<T, I>>
    struct GlobMemberObjectsHelper
    {
        using Ptr_t = PtrType<T, I>;
        using type = typename std::decay<typename GetType<Ptr_t>::type>::type;
        using types = typename Append<typename GlobMemberObjectsHelper<T, I - 1, void>::types, type>::type;
    };

    template <class T, index_t I>
    struct GlobMemberObjectsHelper<T, I, DisableIfIsMemberObject<T, I>>
    {
        using types = typename GlobMemberObjectsHelper<T, I - 1, void>::types;
    };

    template <class T>
    struct GlobMemberObjectsHelper<T, 0, EnableIfIsMemberObject<T, 0>>
    {
        using Ptr_t = PtrType<T, 0>;
        using types = VariadicTypedef<typename std::decay<typename GetType<Ptr_t>::type>::type>;
    };

    template <class T>
    struct GlobMemberObjectsHelper<T, 0, DisableIfIsMemberObject<T, 0>>
    {
        using types = VariadicTypedef<void>;
    };

    template <class T>
    struct GlobMemberObjects
    {
        using types = typename GlobMemberObjectsHelper<T, Reflect<T>::NUM_FIELDS - 1, void>::types;
    };
} // namespace ct

#define CT_PP_OVERLOAD(prefix, ...) CT_PP_CAT(prefix, CT_PP_VARIADIC_SIZE(__VA_ARGS__))

#define REFLECT_BEGIN(TYPE)                                                                                            \
    template <>                                                                                                        \
    struct ReflectImpl<TYPE>                                                                                           \
    {                                                                                                                  \
        using DataType = TYPE;                                                                                         \
        using BaseTypes = VariadicTypedef<>;                                                                           \
        static constexpr const char* getName() { return #TYPE; }                                                       \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;

#define REFLECT_DERIVED(TYPE, ...)                                                                                     \
    template <>                                                                                                        \
    struct ReflectImpl<TYPE>                                                                                           \
    {                                                                                                                  \
        using DataType = TYPE;                                                                                         \
        using BaseTypes = VariadicTypedef<__VA_ARGS__>;                                                                \
        static constexpr const char* getName() { return #TYPE; }                                                       \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;

#define REFLECT_TEMPLATED_START(TYPE)                                                                                  \
    template <class... Args>                                                                                           \
    struct ReflectImpl<TYPE<Args...>>                                                                                  \
    {                                                                                                                  \
        using DataType = TYPE<Args...>;                                                                                \
        using BaseTypes = VariadicTypedef<>;                                                                           \
        static constexpr const char* getName() { return #TYPE; }                                                       \
        static constexpr const bool SPECIALIZED = true;                                                                \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;

#define REFLECT_INTERNAL_START(TYPE)                                                                                   \
    static constexpr const bool INTERNALLY_REFLECTED = 1;                                                              \
    static constexpr const char* getName() { return #TYPE; }                                                           \
    static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__;                                              \
    using DataType = TYPE;                                                                                             \
    using BaseTypes = ct::VariadicTypedef<>;

#define REFLECT_INTERNAL_DERIVED(TYPE, ...)                                                                            \
    static constexpr const bool INTERNALLY_REFLECTED = true;                                                           \
    static constexpr const char* getName() { return #TYPE; }                                                           \
    static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__;                                              \
    using DataType = TYPE;                                                                                             \
    using BaseTypes = ct::VariadicTypedef<__VA_ARGS__>;

#define PUBLIC_ACCESS(NAME)                                                                                            \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START - 1>)                             \
        ->decltype(ct::makeMemberObjectPointer(#NAME, &DataType::NAME))                                                \
    {                                                                                                                  \
        return ct::makeMemberObjectPointer(#NAME, &DataType::NAME);                                                    \
    }

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

#define PROPERTY(NAME, GETTER, SETTER)                                                                                 \
    static constexpr auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START - 1>)                             \
        ->decltype(ct::makeMemberPropertyPointer(#NAME, GETTER, SETTER))                                               \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer(#NAME, GETTER, SETTER);                                                   \
    }

#define PROPERTY_WITH_FLAG(FLAG, NAME, GETTER, SETTER)                                                                 \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START - 1>)                             \
        ->decltype(ct::makeMemberPropertyPointer<FLAG>(#NAME, GETTER, SETTER))                                         \
    {                                                                                                                  \
        return ct::makeMemberPropertyPointer<FLAG>(#NAME, GETTER, SETTER);                                             \
    }

#define MEMBER_FUNCTION(NAME, ...)                                                                                     \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START - 1>)                             \
        ->decltype(ct::makeMemberFunctionPointers(#NAME, __VA_ARGS__))                                                 \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers(#NAME, __VA_ARGS__);                                                     \
    }

#define MEMBER_FUNCTION_WITH_FLAG(FLAG, NAME, ...)                                                                     \
    constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START - 1>)                             \
        ->decltype(ct::makeMemberFunctionPointers<FLAG>(#NAME, __VA_ARGS__))                                           \
    {                                                                                                                  \
        return ct::makeMemberFunctionPointers<FLAG>(#NAME, __VA_ARGS__);                                               \
    }

#define REFLECT_END                                                                                                    \
    static constexpr const index_t REFLECT_COUNT_END = __COUNTER__;                                                    \
    static constexpr const index_t REFLECTION_COUNT = REFLECT_COUNT_END - REFLECT_COUNT_START - 1;                     \
    static constexpr const index_t NUM_FIELDS = REFLECTION_COUNT - 1;                                                  \
    static constexpr ct::Indexer<NUM_FIELDS> end() { return ct::Indexer<NUM_FIELDS>{}; }                               \
    }

#define REFLECT_INTERNAL_END                                                                                           \
    static constexpr const ct::index_t REFLECT_COUNT_END = __COUNTER__;                                                \
    static constexpr const ct::index_t REFLECTION_COUNT = REFLECT_COUNT_END - REFLECT_COUNT_START - 1;                 \
    static constexpr const ct::index_t NUM_FIELDS = REFLECTION_COUNT - 1;                                              \
    static constexpr ct::Indexer<NUM_FIELDS> end() { return ct::Indexer<NUM_FIELDS>{}; }
