#ifndef CT_REFLECT_TRAITS_HPP
#define CT_REFLECT_TRAITS_HPP
#include "enum.hpp"
#include "reflect.hpp"
#include "type_traits.hpp"
namespace ct
{

    template <class DTYPE, class CTYPE, class METADATA, Flag_t FLAGS>
    struct GetType<MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>>
    {
        using type = const typename MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>::Data_t&;
    };

    template <class DTYPE, class CTYPE, class METADATA, Flag_t FLAGS>
    struct SetType<MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>>
    {
        using type = typename MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>::Data_t&;
    };

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS, class METADATA>
    struct GetType<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>>
    {
        using type = typename MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>::Data_t;
    };

    template <class T, Flag_t FLAGS, class METADATA, class... PTRS>
    struct GetType<MemberFunctionPointers<T, FLAGS, METADATA, PTRS...>>
    {
        using type = VariadicTypedef<typename InferPointerType<PTRS>::Data_t...>;
    };

    template <class T, Flag_t FLAGS, class METADATA, class... PTRS>
    struct GetType<StaticFunctions<T, FLAGS, METADATA, PTRS...>>
    {
        using type = VariadicTypedef<typename InferPointerType<PTRS>::Data_t...>;
    };

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS, class METADATA>
    struct SetType<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>>
    {
        using type = typename InferSetterType<SET_PTR>::type;
    };

    template <class T, class E = void>
    struct InternallyReflected : std::false_type
    {
    };

    template <class T, class E = void>
    struct HasFieldCounter : std::false_type
    {
    };
    template <class T>
    struct HasFieldCounter<T, Valid<decltype(T::NUM_FIELDS)>> : std::true_type
    {
    };

    template <class T>
    struct InternallyReflected<T, EnableIf<HasFieldCounter<T>::value>> : std::true_type
    {
    };

    template <class T, class E>
    struct IsReflected : InternallyReflected<T>
    {
    };

    template <class T>
    struct IsReflected<T, EnableIf<HasFieldCounter<ReflectImpl<T>>::value>> : std::true_type
    {
    };

    template <class T, class U = void>
    using EnableIfReflected = EnableIf<IsReflected<T>::value, U>;

    template <class T, class U = void>
    using DisableIfReflected = EnableIf<!IsReflected<T>::value, U>;

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
    constexpr index_t indexOfField(StringView field_name, EnableIfReflected<T, int32_t> = 0)
    {
        return indexOfFieldImpl<T>(field_name, Reflect<T>::end());
    }

    template <class T>
    constexpr index_t indexOfField(StringView, DisableIfReflected<T, int32_t> = 0)
    {
        return -1;
    }

    template <class T>
    constexpr bool haveField(const char* field_name)
    {
        return indexOfField<T>(field_name) != -1;
    }

    template <class T, index_t I>
    using PtrType = decltype(Reflect<typename std::decay<T>::type>::getPtr(Indexer<I>{}));

    template <class T>
    struct IsFunction
    {
        static constexpr const bool value = false;
    };

    template <class T, Flag_t FLAGS, class METADATA, class... PTRS>
    struct IsFunction<StaticFunctions<T, FLAGS, METADATA, PTRS...>>
    {
        static constexpr const bool value = true;
    };

    template <class T, Flag_t FLAGS, class METADATA, class... PTRS>
    struct IsFunction<MemberFunctionPointers<T, FLAGS, METADATA, PTRS...>>
    {
        static constexpr const bool value = true;
    };

    template <class T, index_t I>
    struct IsMemberFunction
    {
        using Accessor_t = PtrType<T, I>;
        static constexpr const bool value = IsFunction<Accessor_t>::value;
    };

    template <class T>
    struct IsMemberPropertyPointer
    {
        constexpr static const bool value = false;
        constexpr static const bool has_setter = false;
    };

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS>
    struct IsMemberPropertyPointer<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>>
    {
        constexpr static const bool value = true;
        constexpr static const bool has_setter = true;
    };

    template <class GET_PTR, Flag_t FLAGS>
    struct IsMemberPropertyPointer<MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS>>
    {
        constexpr static const bool value = true;
        constexpr static const bool has_setter = false;
    };

    template <class T>
    struct IsMemberObjectPointer
    {
        constexpr static const bool value = false;
    };

    template <class DTYPE, class CTYPE, class METADATA, Flag_t FLAGS>
    struct IsMemberObjectPointer<MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>>
    {
        constexpr static const bool value = true;
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

    template <class T, index_t I>
    struct IsReadable
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = flags<type>() & Flags::READABLE;
    };

    template <class T, index_t I>
    struct IsWritable
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = flags<type>() & Flags::WRITABLE;
    };

    template <class T, index_t I>
    struct ShouldSerialize
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = !(flags<type>() & Flags::DO_NOT_SERIALIZE);
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

    template <class T, class U>
    constexpr index_t indexOfMemberTypeImpl(Indexer<0>)
    {
        return std::is_same<decay_t<typename FieldSetType<T, 0>::type>, U>::value ? 0 : -1;
    }

    template <class T, class U, index_t I>
    constexpr index_t indexOfMemberTypeImpl(Indexer<I> idx)
    {
        return std::is_same<decay_t<typename FieldSetType<T, I>::type>, U>::value ? I
                                                                                  : indexOfMemberTypeImpl<T, U>(--idx);
    }

    template <class T, class U>
    constexpr index_t indexOfMemberType()
    {
        return indexOfMemberTypeImpl<T, U>(Reflect<T>::end());
    }

    template <class T, index_t I, index_t J = 0, class U = void>
    using EnableIfArgs = EnableIf<CountArgs<T, I, J>::NUM_ARGS >= 1, U>;

    template <class T, index_t I, index_t J = 0, class U = void>
    using EnableIfNoArgs = EnableIf<CountArgs<T, I, J>::NUM_ARGS == 0, U>;

    template <class T, index_t I, index_t J = 0>
    struct ConstFunction
    {
        using Ptr_t = PtrType<T, I>;
        using FunctionPtr_t = typename std::decay<decltype(std::get<J>(std::declval<Ptr_t>().m_ptrs))>::type;
        static constexpr const bool value = FunctionPtr_t::IS_CONST;
    };

    template <class T>
    struct EnumChecker
    {
        static constexpr const bool value =
            std::is_base_of<EnumTag, T>::value || std::is_base_of<BitsetTag, T>::value || std::is_enum<T>::value;
    };

    template <class T, class U = void>
    using EnableIfIsEnum = EnableIf<EnumChecker<T>::value, U>;

    template <class T>
    struct IsEnumField : std::false_type
    {
    };

    template <class T>
    struct IsEnumField<EnumField<T>> : std::true_type
    {
    };

    template <class T, index_t I, class U = void>
    using EnableIfIsReadable = EnableIf<IsReadable<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using DisableIfIsReadable = EnableIf<!IsReadable<T, I>::value, U>;

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
    using DisableIfIsMemberObject = EnableIf<!IsMemberObject<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using EnableIfIsMemberProperty = EnableIf<IsMemberProperty<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using DisableIfIsMemberProperty = EnableIf<!IsMemberProperty<T, I>::value, U>;

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
        constexpr static const auto num = types::size();
    };

    template <class T, index_t I, class ENABLE = EnableIfIsWritable<T, I>>
    struct GlobWritableHelper
    {
        using Ptr_t = PtrType<T, I>;
        using type = typename std::decay<typename SetType<Ptr_t>::type>::type;
        using types = typename Append<typename GlobWritableHelper<T, I - 1, void>::types, type>::type;
    };

    template <class T, index_t I>
    struct GlobWritableHelper<T, I, DisableIfIsWritable<T, I>>
    {
        using types = typename GlobWritableHelper<T, I - 1, void>::types;
    };

    template <class T>
    struct GlobWritableHelper<T, 0, EnableIfIsWritable<T, 0>>
    {
        using Ptr_t = PtrType<T, 0>;
        using types = VariadicTypedef<typename std::decay<typename GetType<Ptr_t>::type>::type>;
    };

    template <class T>
    struct GlobWritableHelper<T, 0, DisableIfIsWritable<T, 0>>
    {
        using types = VariadicTypedef<>;
    };

    template <class T>
    struct GlobWritable
    {
        using types = typename GlobWritableHelper<T, Reflect<T>::NUM_FIELDS - 1, void>::types;
        constexpr static const auto num = types::size();
    };

    template <index_t I, class T>
    auto set(T& obj) -> typename FieldSetType<T, I>::type
    {
        auto ptr = Reflect<T>::getPtr(Indexer<I>{});
        return set(ptr, obj);
    }
} // namespace ct

#endif // CT_REFLECT_TRAITS_HPP
