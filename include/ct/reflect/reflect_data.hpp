#pragma once
#include "ct/detail/counter.hpp"
#include "ct/reflect/detail/reflect_data.hpp"
#include <stdint.h>

namespace ct
{
    namespace reflect
    {

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

        template <class T, class Enable>
        struct ReflectData
        {
            static constexpr bool IS_SPECIALIZED = false;
        };

        template <int I, class T>
        static constexpr inline auto get(T& data)
            -> decltype(ReflectData<typename std::remove_const<T>::type>::get(data, _counter_<I>()))
        {
            return ReflectData<typename std::remove_const<T>::type>::get(data, _counter_<I>());
        }

        template <int I, class T>
        static constexpr inline auto get(const T& data)
            -> decltype(ReflectData<typename std::remove_const<T>::type>::get(data, _counter_<I>()))
        {
            return ReflectData<typename std::remove_const<T>::type>::get(data, _counter_<I>());
        }

        template <int I, class T>
        static constexpr inline const char* getName()
        {
            return ReflectData<typename std::remove_const<T>::type>::getName(_counter_<I>());
        }

        template <class T>
        constexpr int len()
        {
            return ReflectData<typename std::remove_const<T>::type>::N;
        }

    } // namespace ct::reflect
} // namespace ct

#define CT_PP_CAT(a, b) CT_PP_CAT_I(a, b)
# if _MSC_VER
#    define CT_PP_CAT_I(a, b) a ## b
# else
#    define CT_PP_CAT_I(a, b) CT_PP_CAT_II(~, a ## b)
#    define CT_PP_CAT_II(p, res) res
# endif
# define CT_PP_EMPTY()

#define CT_PP_VARIADIC_SIZE_I(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15, e16, e17, e18, e19, e20, e21, e22, e23, e24, e25, e26, e27, e28, e29, e30, e31, e32, e33, e34, e35, e36, e37, e38, e39, e40, e41, e42, e43, e44, e45, e46, e47, e48, e49, e50, e51, e52, e53, e54, e55, e56, e57, e58, e59, e60, e61, e62, e63, size, ...) size

#if _MSC_VER
#    define CT_PP_VARIADIC_SIZE(...) CT_PP_CAT(CT_PP_VARIADIC_SIZE_I(__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,),)
#else
#    define CT_PP_VARIADIC_SIZE(...) CT_PP_VARIADIC_SIZE_I(__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,)
#endif

#define CT_PP_OVERLOAD(prefix, ...) CT_PP_CAT(prefix, CT_PP_VARIADIC_SIZE(__VA_ARGS__))

#define REFLECT_DATA_START(TYPE)                                                                                       \
    template <>                                                                                                        \
    struct ReflectData<TYPE, void>                                                                                     \
    {                                                                                                                  \
        static constexpr int START = __COUNTER__;                                                                      \
        static constexpr bool IS_SPECIALIZED = true;                                                                   \
        static constexpr int I0 = 0;                                                                                   \
        static constexpr const char* getName(){return #TYPE; }                                                         \
        typedef TYPE DType;

#define REFLECT_DATA_DERIVED(TYPE, BASE)                                                                               \
    template <>                                                                                                        \
    struct ReflectData<TYPE, void>                                                                                     \
    {                                                                                                                  \
        static constexpr int START = __COUNTER__;                                                                      \
        static constexpr bool IS_SPECIALIZED = true;                                                                   \
        static constexpr int I0 = ReflectData<BASE, void>::N;                                                          \
        static constexpr const char* getName(){return #TYPE; }                                                         \
        typedef TYPE DType;                                                                                            \
        template <int I>                                                                                               \
        static constexpr auto get(DType& data, ct::_counter_<I> cnt, enable_if_t < I >= 0 && I<I0> * = 0)->decltype(ReflectData<BASE, void>::get(data, cnt))&        \
        {                                                                                                              \
            return ReflectData<BASE, void>::get(data, cnt);                                                            \
        }                                                                                                              \
        template <int I>                                                                                               \
        static constexpr auto                                                                                   \
        get(const DType& data, ct::_counter_<I> cnt, enable_if_t < I >= 0 && I<I0> * = 0)->const decltype(ReflectData<BASE, void>::get(data, cnt))&                         \
        {                                                                                                              \
            return ReflectData<BASE, void>::get(data, cnt);                                                            \
        }                                                                                                              \
        template <int I>                                                                                               \
        static constexpr const char* getName(ct::_counter_<I> cnt, enable_if_t < I >= 0 && I<I0> * = 0)           \
        {                                                                                                              \
            return ReflectData<BASE, void>::getName(cnt);                                                              \
        }

#define REFLECT_DATA_MEMBER(NAME) REFLECT_DATA_MEMBER_(NAME, __COUNTER__)

#if __cplusplus > 201103
#define REFLECT_DATA_MEMBER_(NAME, N)                                                                                  \
    static constexpr auto& get(DType& data, ct::_counter_<N - START - 1 + I0>){ return data.NAME; }                   \
    static constexpr const auto& get(const DType& data, ct::_counter_<N - START - 1 + I0>){ return data.NAME; }       \
    static constexpr const char* getName(ct::_counter_<N - START - 1 + I0> /*dummy*/) { return #NAME; }
#else
#define REFLECT_DATA_MEMBER_(NAME, N)                                                                                  \
    static constexpr auto get(DType& data, ct::_counter_<N - START - 1 + I0>)-> decltype(data.NAME)& { return data.NAME; }                   \
    static constexpr auto get(const DType& data, ct::_counter_<N - START - 1 + I0>)-> const decltype(data.NAME)& { return data.NAME; }       \
    static constexpr const char* getName(ct::_counter_<N - START - 1 + I0> /*dummy*/) { return #NAME; }
#endif
#define REFLECT_DATA_END                                                                                               \
    static constexpr int N = __COUNTER__ - START - 1 + I0;                                                             \
    }

#define REFLECT_TEMPLATED_DATA_START(TYPE)                                                                             \
    template <class... T>                                                                                              \
    struct ReflectData<TYPE<T...>, void>                                                                               \
    {                                                                                                                  \
        static constexpr int START = __COUNTER__;                                                                      \
        static constexpr bool IS_SPECIALIZED = true;                                                                   \
        static constexpr int I0 = 0;                                                                                   \
        typedef TYPE<T...> DType;

#define REFLECT_TEMPLATED_DERIVED_DATA_START(TYPE, BASE) \
    template <class... T>                                                                                                        \
    struct ReflectData<TYPE<T...>, void>                                                                                     \
    {                                                                                                                  \
        static constexpr const int START = __COUNTER__;                                                                      \
        static constexpr const bool IS_SPECIALIZED = true;                                                                   \
        static constexpr const int I0 = ReflectData<BASE, void>::N;                                                          \
        static constexpr const char* getName(){return #TYPE; }                                                         \
        typedef TYPE<T...> DType;                                                                                            \
        template <int I>                                                                                               \
        static constexpr auto get(DType& data, ct::_counter_<I> cnt, enable_if_t < I >= 0 && I<I0> * = 0)-> typename std::decay<decltype(ReflectData<BASE, void>::get(data, cnt))>::type&        \
        {                                                                                                              \
            return ReflectData<BASE, void>::get(data, cnt);                                                            \
        }                                                                                                              \
        template <int I>                                                                                               \
        static constexpr auto                                                                                   \
        get(const DType& data, ct::_counter_<I> cnt, enable_if_t < I >= 0 && I<I0> * = 0)->const typename std::decay<decltype(ReflectData<BASE, void>::get(data, cnt))>::type&                         \
        {                                                                                                              \
            return ReflectData<BASE, void>::get(data, cnt);                                                            \
        }                                                                                                              \
        template <int I>                                                                                               \
        static constexpr const char* getName(ct::_counter_<I> cnt, enable_if_t < I >= 0 && I<I0> * = 0)           \
        {                                                                                                              \
            return ReflectData<BASE, void>::getName(cnt);                                                              \
        }

#define REFLECT_TEMPLATED_DERIVED_DATA_START_2(TYPE, BASE, TEMPLATE_TYPE) \
    template <TEMPLATE_TYPE... T>                                                                                                        \
    struct ReflectData<TYPE<T...>, void>                                                                                     \
    {                                                                                                                  \
        static constexpr const int START = __COUNTER__;                                                                      \
        static constexpr const bool IS_SPECIALIZED = true;                                                                   \
        static constexpr const int I0 = ReflectData<BASE, void>::N;                                                          \
        static constexpr const char* getName(){return #TYPE; }                                                         \
        typedef TYPE<T...> DType;                                                                                            \
        template <int I>                                                                                               \
        static constexpr auto get(DType& data, ct::_counter_<I> cnt, enable_if_t < I >= 0 && I<I0> * = 0)->typename std::decay<decltype(ReflectData<BASE, void>::get(data, cnt))>::type&        \
        {                                                                                                              \
            return ReflectData<BASE, void>::get(data, cnt);                                                            \
        }                                                                                                              \
        template <int I>                                                                                               \
        static constexpr auto                                                                                   \
        get(const DType& data, ct::_counter_<I> cnt, enable_if_t < I >= 0 && I<I0> * = 0)->const typename std::decay<decltype(ReflectData<BASE, void>::get(data, cnt))>::type& \
        {                                                                                                              \
            return ReflectData<BASE, void>::get(data, cnt);                                                            \
        }                                                                                                              \
        template <int I>                                                                                               \
        static constexpr const char* getName(ct::_counter_<I> cnt, enable_if_t < I >= 0 && I<I0> * = 0)           \
        {                                                                                                              \
            return ReflectData<BASE, void>::getName(cnt);                                                              \
        }

// Internally reflectable type, ie no external declaration of ReflectData
// Example usage:
// REFLECT_INTERNAL_START(InternallyReflected)
//    REFLECT_INTERNAL_MEMBER(float, x)
//    REFLECT_INTERNAL_MEMBER(float, y)
//    REFLECT_INTERNAL_MEMBER(float, z)
// REFLECT_INTERNAL_END;
#if __cplusplus > 201103
#define REFLECT_INTERNAL_MEMBER_2(TYPE, NAME) \
    TYPE NAME; \
    REFLECT_DATA_MEMBER(NAME)

#define REFLECT_INTERNAL_MEMBER_3(TYPE, NAME, INIT) \
    TYPE NAME = INIT; \
    REFLECT_DATA_MEMBER(NAME)

#ifdef _MSC_VER
#define REFLECT_INTERNAL_MEMBER(...) CT_PP_CAT(CT_PP_OVERLOAD( REFLECT_INTERNAL_MEMBER_, __VA_ARGS__ )(__VA_ARGS__), CT_PP_EMPTY())
#else
#define REFLECT_INTERNAL_MEMBER(...) CT_PP_OVERLOAD( REFLECT_INTERNAL_MEMBER_, __VA_ARGS__ )(__VA_ARGS__)
#endif

#define REFLECT_INTERNAL_START(TYPE)                                                                                   \
    static constexpr int START = __COUNTER__;                                                                          \
    typedef TYPE DType;                                                                                                \
    static constexpr int I0 = 0;                                                                                       \
    static constexpr const char* getName(){return #TYPE; }                                                             \
    typedef void INTERNALLY_REFLECTED;

#define REFLECT_INTERNAL_END static constexpr int N = __COUNTER__ - START - 1 + I0
#endif
