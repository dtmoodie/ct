#ifndef CT_REFLECT_HPP
#define CT_REFLECT_HPP

#include "reflect_forward.hpp"

#include "type_traits.hpp"

#include "Indexer.hpp"
#include "config.hpp"
#include "hash.hpp"
#include "macros.hpp"

#include "reflect/MemberFunctionPointer.hpp"
#include "reflect/MemberObjectPointer.hpp"
#include "reflect/MemberPropertyPointer.hpp"

#include "static_asserts.hpp"

#include "typename.hpp"

#include <cstdint>
#include <ostream>
#include <utility>

namespace ct
{
    // Reflect is responsible for selecting the source of reflection information
    // for class T.  The non specializaed version seen here means that the provided type T has no source of reflection
    // information.

    template <class T, class VISITED, class ENABLE>
    struct Reflect
    {
        using BaseTypes = VariadicTypedef<>;
        static CT_CONSTEXPR_NAME auto getTypeName() -> decltype(GetName<T>::getName()) { return GetName<T>::getName(); }
    };

    // RelfectImpl is specializaed for each type to contain reflection information for the provided type.
    template <class T, class E = void>
    struct ReflectImpl
    {
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

    // When reflecting over a type and base types, ImplementationFilter creates two getPtr functions
    // One that redirects to the implementation and one that redirects to the base types
    // If either one of these has zero getPtr implementations due to being an empty reflection implementation
    // then we will run into a compiler error.
    // These two classes are used to specialize based on the number of fields between the two implementations

    template <class IMPL, class BASES, index_t NUM_FIELDS, index_t START_INDEX, index_t END_INDEX, class Enable = void>
    struct EmptyReflectionHandler
    {
        // The generic case where both START_INDEX > 0, thus BASES contains reflection fields
        // and NUM_FIELDS > 0, and thus IMPL contains reflection fields
        template <index_t I>
        constexpr static auto getPtr(const Indexer<I>)
            -> EnableIf<(I >= START_INDEX) && (I < END_INDEX), decltype(IMPL::getPtr(Indexer<I - START_INDEX>()))>
        {
            return IMPL::getPtr(Indexer<I - START_INDEX>());
        }

        template <index_t I>
            constexpr static auto getPtr(const Indexer<I> idx)
                -> EnableIf < I<START_INDEX, decltype(BASES::getPtr(idx))>
        {
            return BASES::getPtr(idx);
        }
    };

    template <class IMPL, class BASES, index_t NUM_FIELDS, index_t START_INDEX>
    struct EmptyReflectionHandler<IMPL, BASES, NUM_FIELDS, START_INDEX, START_INDEX, EnableIf<NUM_FIELDS != 0>>
    {
        template <index_t I>
            constexpr static auto getPtr(const Indexer<I> idx)
                -> EnableIf < I<START_INDEX, decltype(BASES::getPtr(idx))>
        {
            return BASES::getPtr(idx);
        }
    };

    template <class IMPL, class BASES, index_t START_INDEX, index_t END_INDEX>
    struct EmptyReflectionHandler<IMPL, BASES, 0, START_INDEX, END_INDEX, void>
    {
        // The specialization where START_INDEX > 0, thus BASES contains reflection fields
        // but NUM_FIELDS == 0, and thus IMPL contains no reflection fields

        template <index_t I>
            constexpr static auto getPtr(const Indexer<I> idx)
                -> EnableIf < I<START_INDEX, decltype(BASES::getPtr(idx))>
        {
            return BASES::getPtr(idx);
        }
    };

    template <class IMPL, class BASES, index_t END_INDEX>
    struct EmptyReflectionHandler<IMPL, BASES, 0, 0, END_INDEX, void>
    {
        // The specialization where START_INDEX == 0, thus BASES does not contain reflection fields
        // and NUM_FIELDS == 0, and thus IMPL contains no reflection fields
    };

    template <class IMPL, class BASES, index_t NUM_FIELDS, index_t END_INDEX>
    struct EmptyReflectionHandler<IMPL, BASES, NUM_FIELDS, 0, END_INDEX, void>
    {
        // The specialization where START_INDEX == 0, thus BASES does not contain reflection fields
        // and NUM_FIELDS > 0, and thus IMPL contains reflection fields
        template <index_t I>
        constexpr static auto getPtr(const Indexer<I>)
            -> EnableIf<(I >= 0) && (I < END_INDEX), decltype(IMPL::getPtr(Indexer<I>()))>
        {
            return IMPL::getPtr(Indexer<I>());
        }
    };

    // This class just makes it easier to fill the template parameters of EmptyReflectionHandler
    template <class T, class IMPL, class VISITED>
    struct EmptyReflectionHandlerSelector
    {
        using BaseTypes = typename BaseSelector<IMPL>::BaseTypes_t;

        using Bases_t = ReflectBases<BaseTypes, typename Append<T, VISITED>::type>;
        using ClassInheritanceList = typename Append<T, typename Bases_t::ClassInheritanceList>::type;
        using VisitationList = typename Bases_t::VisitationList;

        constexpr static const index_t NUM_FIELDS = IMPL::NUM_FIELDS + Bases_t::NUM_FIELDS;
        constexpr static const index_t START_INDEX = Bases_t::END_INDEX;
        constexpr static const index_t END_INDEX = START_INDEX + IMPL::NUM_FIELDS;
        using type = EmptyReflectionHandler<IMPL, Bases_t, NUM_FIELDS, START_INDEX, END_INDEX>;
    };

    // These classes are used for reflecting about base classes of type T
    // Implementation details not necessary for most users

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
            Impl::printHierarchy(os, indent);
            Super::printHierarchy(os, indent);
        }
    };

    // End recursion
    template <class T, class V>
    struct ReflectBasesImpl<VariadicTypedef<T>, V>
        : EmptyReflectionHandler<Reflect<T, V>, void, Reflect<T, V>::NUM_FIELDS, 0, Reflect<T, V>::NUM_FIELDS>
    {
        using Impl = Reflect<T, V>;
        using VisitationListTmp = typename InsertUnique<T, typename Impl::VisitationList>::type;
        using VisitationList = typename InsertUnique<VisitationListTmp, V>::type;

        constexpr static const index_t NUM_FIELDS = Impl::NUM_FIELDS;
        constexpr static const index_t START_INDEX = 0;
        constexpr static const index_t END_INDEX = NUM_FIELDS + START_INDEX;

        using ClassInheritanceList = typename Impl::ClassInheritanceList;

        /*template <index_t I>
            constexpr static auto getPtr(const Indexer<I> idx) -> EnableIf < I<END_INDEX, decltype(Impl::getPtr(idx))>
        {
            return Impl::getPtr(idx);
        }*/

        static void printHierarchy(std::ostream& os, const std::string& indent = "")
        {
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

    template <class IMPL, class ENABLE>
    struct BaseSelector
    {
        using BaseTypes_t = ct::VariadicTypedef<>;
    };

    template <class IMPL>
    struct BaseSelector<IMPL, EnableIf<!std::is_same<typename IMPL::BaseTypes, ct::VariadicTypedef<>>::value>>
    {
        using BaseTypes_t = typename IMPL::BaseTypes;
    };

    template <class T, class IMPL, typename ENABLE = void>
    struct NameSelector
    {
        constexpr static StringView getTypeName() { return GetName<T>::getName(); }
    };

    template <class T, class IMPL>
    struct NameSelector<T, IMPL, EnableIf<Has_name<IMPL>::value>>
    {
        constexpr static StringView getTypeName() { return IMPL::getTypeName(); }
    };

    /* ImplementationFilter is used to filter out repeatedly visiting base classes while visiting an inheritance
     structure
     IE: in the diamond inheritance case of:
        A
       / \
      B   C
       \ /
        D
     When we reflect about the fields of D, we have two paths to visit A, the implementation filter detects
     if we've already visited A and thus prevents a second pass through A
    */
    template <class T, class IMPL, class VISITED>
    struct ImplementationFilter<T, IMPL, VISITED, EnableIf<!ContainsType<T, VISITED>::value>>
        : NameSelector<decay_t<T>, IMPL>, EmptyReflectionHandlerSelector<T, IMPL, VISITED>::type
    {
        using DataType = T;
        using BaseTypes = typename BaseSelector<IMPL>::BaseTypes_t;

        using Bases_t = ReflectBases<BaseTypes, typename Append<T, VISITED>::type>;
        using ClassInheritanceList = typename Append<T, typename Bases_t::ClassInheritanceList>::type;
        using VisitationList = typename Bases_t::VisitationList;

        constexpr static const index_t NUM_FIELDS = IMPL::NUM_FIELDS + Bases_t::NUM_FIELDS;
        constexpr static const index_t START_INDEX = Bases_t::END_INDEX;
        constexpr static const index_t END_INDEX = START_INDEX + IMPL::NUM_FIELDS;

        static void printHierarchy(std::ostream& os, const std::string& indent = "")
        {
            auto num_fields = IMPL::NUM_FIELDS;
            auto start_index = START_INDEX;
            auto end_index = END_INDEX;
            os << indent << "Reflect<" << NameSelector<T, IMPL>::getTypeName() << ", Visited: ";
            printTypes(VISITED{}, os);
            os << "> (" << start_index << ":" << num_fields << ":" << end_index << ')' << std::endl;
            Bases_t::printHierarchy(os, indent + "  ");
        }

        static constexpr ct::Indexer<END_INDEX - 1> end() { return ct::Indexer<END_INDEX - 1>(); }
    };

    // We've already visited this class, so exclude the implementation
    template <class T, class IMPL, class VISITED>
    struct ImplementationFilter<T, IMPL, VISITED, EnableIf<ContainsType<T, VISITED>::value>>
        : public EmptyImplementation
    {
        using ClassInheritanceList = VariadicTypedef<>;
        using VisitationList = VariadicTypedef<>;
    };

    // Specialization for when ReflectImpl is specialized, IE a type has external reflection information
    template <class T, class VISITED>
    struct Reflect<T, VISITED, EnableIfValid<decltype(ReflectImpl<T>::NUM_FIELDS)>>
        : public ImplementationFilter<T, ReflectImpl<T>, VISITED>
    {
        using ImplementationFilter_t = ImplementationFilter<T, ReflectImpl<T>, VISITED>;
    };

    // Specialization for when a type has the appropriate reflection information built into the type
    template <class T, class VISITED>
    struct Reflect<T, VISITED, EnableIf<T::NUM_FIELDS != -1>> : public ImplementationFilter<T, T, VISITED>
    {
        using ImplementationFilter_t = ImplementationFilter<T, T, VISITED>;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// These are helpers to get common traits of a reflected type
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <index_t I, class T>
    constexpr StringView getName()
    {
        return Reflect<T>::getPtr(ct::Indexer<I>{}).getName();
    }

    template <index_t I, class T>
    auto set(T& obj) -> decltype(Reflect<T>::getPtr(Indexer<I>{}).set(obj))
    {
        auto accessor = Reflect<T>::getPtr(Indexer<I>{});
        return accessor.set(obj);
    }

    template <class M, ct::index_t I, class T>
    M* getMetadata()
    {
        auto ptr = ct::Reflect<T>::getPtr(ct::Indexer<I>());
        auto mdata = ptr.getMetadata();
        return mdata.template getMetadata<M>();
    }

    inline void printTypes(const ct::VariadicTypedef<>, std::ostream&) {}

    template <class T>
    void printTypes(const ct::VariadicTypedef<T>, std::ostream& os)
    {
        os << ct::Reflect<T>::getTypeName();
    }

    template <class T, class... T1>
    void printTypes(const ct::VariadicTypedef<T, T1...>, std::ostream& os)
    {
        os << ct::Reflect<T>::getTypeName() << ", ";
        printTypes(ct::VariadicTypedef<T1...>{}, os);
    }
} // namespace ct

#include "reflect_macros.hpp"
#endif // CT_REFLECT_HPP
