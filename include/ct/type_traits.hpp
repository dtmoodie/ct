#ifndef CT_TYPE_TRAITS_HPP
#define CT_TYPE_TRAITS_HPP
#include "StringView.hpp"
#include "VariadicTypedef.hpp"

#include <ostream>
#include <type_traits>
#include <vector>

/*!
    This macro is used for determining of a class has a desired static function.
    It can be used as follows:
    DEFINE_HAS_STATIC_FUNCTION(HasFoo, foo, void(*)(int));
    struct Has
    {
        static void foo(int);
    };
    struct DoesNotHave
    {
        static void foo(int, int);
    };
    HasFoo<Has>::value == 1;
    HasFoo<DoesNotHave>::value == 0;
*/

#define DEFINE_HAS_STATIC_FUNCTION(TRAITS_NAME, FUNC_NAME, RET, ...)                                                   \
    template <typename U, class ENABLE = void, class ARGS = ct::VariadicTypedef<__VA_ARGS__>>                          \
    struct TRAITS_NAME : std::false_type                                                                               \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename U, class... ARGS>                                                                               \
    struct TRAITS_NAME<U, ct::Valid<decltype(U::FUNC_NAME(std::declval<ARGS>()...))>, ct::VariadicTypedef<ARGS...>>    \
        : std::is_same<RET, decltype(U::FUNC_NAME(std::declval<ARGS>()...))>                                           \
    {                                                                                                                  \
    }

#define DEFINE_HAS_MEMBER_FUNCTION(TRAITS_NAME, FUNC_NAME, RET, ...)                                                   \
    template <typename U, class ENABLE = void, class ARGS = ct::VariadicTypedef<__VA_ARGS__>>                          \
    struct TRAITS_NAME : std::false_type                                                                               \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename U, class... ARGS>                                                                               \
    struct TRAITS_NAME<U,                                                                                              \
                       ct::Valid<decltype(std::declval<U>().FUNC_NAME(std::declval<ARGS>()...))>,                      \
                       ct::VariadicTypedef<ARGS...>>                                                                   \
        : std::is_same<RET, decltype(std::declval<U>().FUNC_NAME(std::declval<ARGS>()...))>                            \
    {                                                                                                                  \
    }

namespace ct
{
    template <bool VAL, class U = void>
    using EnableIf = typename std::enable_if<VAL, U>::type;

    template <bool VAL, class U = void>
    using DisableIf = typename std::enable_if<!VAL, U>::type;

    template <class T>
    struct ReferenceType
    {
        using Type = T&;
        using ConstType = const T&;
    };

    template <class T>
    struct ReferenceType<T*>
    {
        using Type = T*;
        using ConstType = const T*;
    };

    template <class T>
    struct IsReferenceType
    {
        static const bool value = std::is_same<typename ReferenceType<T>::Type, T>::value;
    };

    template <class T>
    struct TypeIs
    {
        using type = T;
    };

    template <class T>
    struct Base : TypeIs<T>
    {
    };

    template <class T>
    struct Derived : TypeIs<T>
    {
    };

    template <class T, class U>
    struct IsBase;

    template <class T, class U>
    struct IsBase<Base<T>, Derived<U>> : std::is_base_of<T, U>
    {
    };

    template <class... T>
    struct Validator : TypeIs<void>
    {
    };

    template <class... T>
    using Valid = typename Validator<T...>::type;

    template <class... T>
    using EnableIfValid = EnableIf<std::is_same<Valid<T...>, void>::value>;

    template <class... T>
    using DisableIfValid = EnableIf<!std::is_same<Valid<T...>, void>::value>;

    DEFINE_HAS_STATIC_FUNCTION(Has_name, getName, ct::StringView);

    template <class T, class E = void>
    struct StreamWritable : std::false_type
    {
    };

    template <class T>
    struct StreamWritable<T, Valid<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type
    {
    };

    template <class T>
    struct StreamWritableHelper;

    template <class T>
    struct StreamWritableHelper<VariadicTypedef<T>>
    {
        constexpr static const bool value = StreamWritable<T>::value;
    };

    template <class T, class... TYPES>
    struct StreamWritableHelper<VariadicTypedef<T, TYPES...>>
    {
        constexpr static const bool value =
            StreamWritableHelper<VariadicTypedef<TYPES...>>::value && StreamWritable<T>::value;
    };

    template <class... TYPES>
    struct StreamWritable<VariadicTypedef<TYPES...>>
    {
        constexpr static const bool value = StreamWritableHelper<VariadicTypedef<TYPES...>>::value;
    };

    template <typename T, typename _ = void>
    struct IsContainer : std::false_type
    {
    };

    template <typename... Ts>
    struct IsContainerHelper
    {
    };

    template <typename T>
    struct IsContainer<T,
                       typename std::conditional<false,
                                                 IsContainerHelper<typename T::value_type,
                                                                   typename T::size_type,
                                                                   typename T::allocator_type,
                                                                   typename T::iterator,
                                                                   typename T::const_iterator,
                                                                   decltype(std::declval<T>().size()),
                                                                   decltype(std::declval<T>().begin()),
                                                                   decltype(std::declval<T>().end()),
                                                                   decltype(std::declval<T>().cbegin()),
                                                                   decltype(std::declval<T>().cend())>,
                                                 void>::type> : public std::true_type
    {
    };

    template <bool B, class T, class F>
    using conditional_t = typename std::conditional<B, T, F>::type;

    template <class T>
    using remove_reference_t = typename std::remove_reference<T>::type;

    template <class T>
    using decay_t = typename std::decay<T>::type;
#ifndef NVCC
    template <class T>
    using remove_reference_t = typename std::remove_reference<T>::type;
#endif

    template <class T>
    using remove_cv_t = typename std::remove_cv<T>::type;

    template <class T>
    using is_array_t = typename std::is_array<T>::value;

    template <class T>
    using is_lvalue_reference_t = typename std::is_lvalue_reference<T>::value;

    template <class T>
    using is_rvalue_reference_t = typename std::is_rvalue_reference<T>::value;

    template <class T, class O = void>
    using enable_pod_or_indexable =
        typename std::enable_if<std::is_pod<remove_reference_t<typename remove_reference_t<T>::Type>>::value ||
                                    remove_reference_t<T>::IsIndexable,
                                O>::type;

    template <class T, class O = void>
    using enable_pod =
        typename std::enable_if<std::is_pod<remove_reference_t<typename remove_reference_t<T>::Type>>::value, O>::type;

    template <typename T>
    class IsDefaultConstructible
    {

        typedef char yes;
        typedef struct
        {
            char arr[2];
        } no;

        template <typename U>
        static decltype(U(), yes()) test(int);

        template <typename>
        static no test(...);

      public:
        static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    };
} // namespace ct
#endif // CT_TYPE_TRAITS_HPP
