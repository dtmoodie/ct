#pragma once
#include <ct/VariadicTypedef.hpp>

#include <ostream>
#include <type_traits>
#include <vector>

namespace ct
{
    template <class T>
    struct ReferenceType
    {
        using Type = T&;
        using ConstType = const T&;
    };

    template <class T>
    struct is_reference_type
    {
        static const bool value = std::is_same<typename ReferenceType<T>::Type, T>::value;
    };

    template <class T>
    struct StreamWritable
    {
        template <class U>
        static constexpr auto check(std::ostream* os, U* val) -> decltype(*os << *val, uint32_t())
        {
            return 0;
        }

        template <class U>
        static constexpr uint8_t check(...)
        {
            return 0;
        }
        static const bool value =
            sizeof(check<T>(static_cast<std::ostream*>(nullptr), static_cast<T*>(nullptr))) == sizeof(uint32_t);
    };

    template<class T>
    struct StreamWritableHelper;

    template<class T>
    struct StreamWritableHelper<VariadicTypedef<T>>
    {
        constexpr static const bool value = StreamWritable<T>::value;
    };

    template<class T, class ... TYPES>
    struct StreamWritableHelper<VariadicTypedef<T, TYPES...>>
    {
        constexpr static const bool value = StreamWritableHelper<VariadicTypedef<TYPES...>>::value && StreamWritable<T>::value;
    };

    template<class ... TYPES>
    struct StreamWritable<VariadicTypedef<TYPES...>>
    {
        constexpr static const bool value = StreamWritableHelper<VariadicTypedef<TYPES...>>::value;
    };

    template <typename T, typename _ = void>
    struct is_container : std::false_type
    {
    };

    template <typename... Ts>
    struct is_container_helper
    {
    };

    template <typename T>
    struct is_container<T,
                        typename std::conditional<false,
                                                  is_container_helper<typename T::value_type,
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
    class is_default_constructible
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
}
