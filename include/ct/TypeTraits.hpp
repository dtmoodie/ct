#pragma once
#include <type_traits>
#include <vector>
#include <ostream>

namespace ct
{
    template <class T>
    struct StreamWritable {
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
        static const bool value = sizeof(check<T>(static_cast<std::ostream*>(nullptr), static_cast<T*>(nullptr))) == sizeof(uint32_t);
    };

    template< bool B, class T, class F >
    using conditional_t = typename std::conditional<B, T, F>::type;

    template<class T>
    using remove_reference_t = typename std::remove_reference<T>::type;

    template<class T>
    using decay_t = typename std::decay<T>::type;

    template<class T>
    using remove_cv_t = typename std::remove_cv<T>::type;

    template<class T>
    using is_array_t = typename std::is_array<T>::value;

    template<class T>
    using is_lvalue_reference_t = typename std::is_lvalue_reference<T>::value;

    template<class T>
    using is_rvalue_reference_t = typename std::is_rvalue_reference<T>::value;

    template<class T, class O = void>
    using enable_pod_or_indexable = typename std::enable_if<
        std::is_pod<remove_reference_t<typename remove_reference_t<T>::Type>>::value || remove_reference_t<T>::IsIndexable,
        O>::type;

    template<class T, class O = void>
    using enable_pod = typename std::enable_if<
        std::is_pod<remove_reference_t<typename remove_reference_t<T>::Type>>::value, O>::type;

    template<class T, class O = void>
    using enable_indexable = typename std::enable_if<remove_reference_t<T>::IsIndexable, O>::type;

    template<class T> 
    struct Indexable
    {
        enum
        {
            ConstSize = 0,
            Dims = 0,
            IsIndexable = 0
        };
        typedef T Elem_t;
    };

    template<class T> 
    struct Indexable<std::vector<T>>
    {
        enum
        {
            ConstSize = 0,
            Dims = 1,
            IsIndexable = 1
        };
        typedef T Elem_t;
    };

    template<typename T>
    class is_default_constructible {

        typedef char yes;
        typedef struct { char arr[2]; } no;

        template<typename U>
        static decltype(U(), yes()) test(int);

        template<typename>
        static no test(...);

    public:

        static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    };
}