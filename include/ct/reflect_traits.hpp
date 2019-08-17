#ifndef CT_REFLECT_TRAITS_HPP
#define CT_REFLECT_TRAITS_HPP
#include "enum.hpp"
#include "reflect.hpp"

namespace ct
{

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

    template <class T, index_t I>
    struct IsReadable
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = getFlags<type>() & Flags::READABLE;
    };

    template <class T, index_t I>
    struct IsWritable
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = getFlags<type>() & Flags::WRITABLE;
    };

    template <class T, index_t I>
    struct ShouldSerialize
    {
        using type = PtrType<T, I>;
        constexpr static const bool value = !(getFlags<type>() & Flags::DO_NOT_SERIALIZE);
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
        static constexpr const bool value = std::is_base_of<EnumTag, T>::value;
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
        constexpr static const auto num = LenVariadicTypedef<types>::value;
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
        constexpr static const auto num = LenVariadicTypedef<types>::value;
    };

    template <index_t I, class T>
    auto set(T& obj) -> typename FieldSetType<T, I>::type
    {
        auto ptr = Reflect<T>::getPtr(Indexer<I>{});
        return set(ptr, obj);
    }
} // namespace ct

#endif // CT_REFLECT_TRAITS_HPP
