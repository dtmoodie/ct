#ifndef CT_REFLECT_HPP
#define CT_REFLECT_HPP
#include "Indexer.hpp"
#include "VariadicTypedef.hpp"
#include "config.hpp"
#include "hash.hpp"
#include "macros.hpp"
#include "reflect/MemberFunctionPointer.hpp"
#include "reflect/MemberObjectPointer.hpp"
#include "reflect/MemberPropertyPointer.hpp"
#include "static_asserts.hpp"
#include "type_traits.hpp"
#include "typename.hpp"

#include <cstdint>
#include <ostream>
#include <utility>

namespace ct
{
    // The concrete implementation of Reflect is responsible for selecting the source of reflection information
    // for class T.  The non specializaed version seen here means that the provided type T has no source of reflection
    // information.

    template <class T, class VISITED = VariadicTypedef<>, class ENABLE = void>
    struct Reflect
    {
        static const bool SPECIALIZED = false;
        using BaseTypes = VariadicTypedef<>;
        static constexpr StringView getName() { return ""; }
    };

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

    // These classes are used for reflecting about base classes of type T
    // Implementation details not necessary for most users
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

    template <class IMPL, class ENABLE = void>
    struct BaseSelector
    {
        using BaseTypes_t = ct::VariadicTypedef<>;
    };

    template <class IMPL>
    struct BaseSelector<IMPL, EnableIf<!std::is_same<typename IMPL::BaseTypes, ct::VariadicTypedef<>>::value>>
    {
        using BaseTypes_t = typename IMPL::BaseTypes;
    };

    // ImplementationFilter is used to filter out repeatedly visiting base classes while visiting an inheritance
    // structure
    // IE: in the diamond inheritance case of:
    //    A
    //   / \
    //  B   C
    //   \ /
    //    D
    // When we reflect about the fields of D, we have two paths to visit A, the implementation filter detects
    // if we've already visited A and thus prevents a second pass through A
    template <class T, class IMPL, class VISITED>
    struct ImplementationFilter<T, IMPL, VISITED, EnableIf<!ContainsType<T, VISITED>::value>>
    {
        using DataType = T;
        using BaseTypes = typename BaseSelector<IMPL>::BaseTypes_t;

        using Bases_t = ReflectBases<BaseTypes, typename Append<T, VISITED>::type>;
        using ClassInheritanceList = typename Append<T, typename Bases_t::ClassInheritanceList>::type;
        using VisitationList = typename Bases_t::VisitationList;

        static const bool SPECIALIZED = true;
        constexpr static const index_t NUM_FIELDS = IMPL::NUM_FIELDS + Bases_t::NUM_FIELDS;
        constexpr static const index_t START_INDEX = Bases_t::END_INDEX;
        constexpr static const index_t END_INDEX = START_INDEX + IMPL::NUM_FIELDS;

        // constexpr static StringView getName() { return IMPL::getName(); }
        CT_CONSTEXPR_NAME static StringView getName() { return getNameImpl<IMPL>(); }

        template <index_t I>
        constexpr static auto getPtr(const Indexer<I>)
            -> EnableIf<(I >= START_INDEX) && (I < END_INDEX), decltype(IMPL::getPtr(Indexer<I - START_INDEX>()))>
        {
            return IMPL::getPtr(Indexer<I - START_INDEX>());
        }

        template <index_t I>
            constexpr static auto getPtr(const Indexer<I> idx)
                -> EnableIf < I<START_INDEX, decltype(Bases_t::getPtr(idx))>
        {
            return Bases_t::getPtr(idx);
        }

        static void printHierarchy(std::ostream& os, const std::string& indent = "")
        {
            auto num_fields = IMPL::NUM_FIELDS;
            auto start_index = START_INDEX;
            auto end_index = END_INDEX;
            os << indent << "Reflect<" << getName() << ", Visited: ";
            printTypes(VISITED{}, os);
            os << "> (" << start_index << ":" << num_fields << ":" << end_index << ')' << std::endl;
            Bases_t::printHierarchy(os, indent + "  ");
        }

        static constexpr ct::Indexer<END_INDEX - 1> end() { return ct::Indexer<END_INDEX - 1>(); }
      private:
        template <class U = T>
        static CT_CONSTEXPR_NAME auto getNameImpl() -> EnableIf<Has_getName<U>::value, StringView>
        {
            return U::getName();
        }

        template <class U = T>
        static CT_CONSTEXPR_NAME auto getNameImpl() -> EnableIf<!Has_getName<U>::value, StringView>
        {
            return GetName<T>::getName();
        }
    };

    // We've already visited this class, so exclude the implementation
    template <class T, class IMPL, class VISITED>
    struct ImplementationFilter<T, IMPL, VISITED, EnableIf<ContainsType<T, VISITED>::value>>
        : public EmptyImplementation
    {
        using ClassInheritanceList = VariadicTypedef<>;
        using VisitationList = VariadicTypedef<>;
    };

    template <class T, class VISITED>
    struct Reflect<T, VISITED, EnableIf<ReflectImpl<T>::SPECIALIZED>>
        : public ImplementationFilter<T, ReflectImpl<T>, VISITED>
    {
        using ImplementationFilter_t = ImplementationFilter<T, ReflectImpl<T>, VISITED>;
    };

    // Internally defined reflection
    template <class T, class VISITED>
    struct Reflect<T, VISITED, EnableIf<T::NUM_FIELDS != -1>> : public ImplementationFilter<T, T, VISITED>
    {
        using ImplementationFilter_t = ImplementationFilter<T, T, VISITED>;
    };

    template <class T>
    struct IsReflected
    {
        constexpr static const bool value = Reflect<T>::SPECIALIZED;
    };

    template <class T, class U = void>
    using EnableIfReflected = EnableIf<IsReflected<T>::value, U>;

    template <class T, class U = void>
    using DisableIfReflected = EnableIf<!IsReflected<T>::value, U>;

    template <class T, index_t I>
    using PtrType = decltype(ct::Reflect<T>::getPtr(Indexer<I>{}));

    template <class T, index_t I>
    struct IsMemberFunction
    {
        using Accessor_t = PtrType<T, I>;
        static constexpr const bool value = IsFunction<Accessor_t>::value;
    };

    template <class T, index_t I>
    struct IsMemberObject
    {
        using Accesosr_t = PtrType<T, I>;
        static constexpr const bool value = IsMemberObjectPointer<Accesosr_t>::value;
    };

    template <class T, index_t I>
    struct IsMemberProperty
    {
        using Accessor_t = PtrType<T, I>;
        static constexpr const bool value = IsMemberPropertyPointer<Accessor_t>::value;
    };

    template <index_t I, class T>
    constexpr StringView getName()
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
    constexpr index_t indexOfFieldImpl(StringView field_name, const Indexer<0>)
    {
        return getName<0, T>() == field_name ? 0 : -1;
    }

    template <class T, index_t I>
    constexpr index_t indexOfFieldImpl(StringView field_name, const Indexer<I> idx)
    {
        return getName<I, T>() == field_name ? I : indexOfFieldImpl<T>(field_name, --idx);
    }

    template <class T>
    constexpr index_t indexOfField(StringView field_name)
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

    template <class T, index_t I, index_t J = 0>
    struct CountArgs
    {
        using Ptr_t = PtrType<T, I>;
        using FunctionPtr_t = typename std::decay<decltype(std::get<J>(std::declval<Ptr_t>().m_ptrs))>::type;
        constexpr static const index_t NUM_ARGS = FunctionPtr_t::NUM_ARGS;
    };

    template <class T, index_t I, class U = void>
    using EnableIfArgs = EnableIf<CountArgs<T, I>::NUM_ARGS >= 1, U>;

    template <class T, index_t I, class U = void>
    using EnableIfNoArgs = EnableIf<CountArgs<T, I>::NUM_ARGS == 0, U>;

    template <class T, index_t I, index_t J = 0>
    struct ConstFunction
    {
        using Ptr_t = PtrType<T, I>;
        using FunctionPtr_t = typename std::decay<decltype(std::get<J>(std::declval<Ptr_t>().m_ptrs))>::type;
        static constexpr const bool value = FunctionPtr_t::IS_CONST;
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
    using DisableIfIsWritable = EnableIf<!IsWritable<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using EnableIfMemberFunction = EnableIf<IsMemberFunction<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using DisableIfMemberFunction = EnableIf<!IsMemberFunction<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using EnableIfIsMemberObject = EnableIf<IsMemberObject<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using EnableIfIsMemberProperty = EnableIf<IsMemberProperty<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using DisableIfIsMemberObject = EnableIf<!IsMemberObject<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using DisableIfIsReadable = EnableIf<!IsReadable<T, I>::value, U>;

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
        using types = VariadicTypedef<>;
    };

    template <class T>
    struct GlobMemberObjects
    {
        using types = typename GlobMemberObjectsHelper<T, Reflect<T>::NUM_FIELDS - 1, void>::types;
        constexpr static const auto num = LenVariadicTypedef<types>::value;
    };

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
        os << ct::Reflect<T>::getName();
    }

    template <class T, class... T1>
    void printTypes(const ct::VariadicTypedef<T, T1...>, std::ostream& os)
    {
        os << ct::Reflect<T>::getName() << ", ";
        printTypes(ct::VariadicTypedef<T1...>{}, os);
    }
} // namespace ct

#include "reflect_macros.hpp"
#endif // CT_REFLECT_HPP
