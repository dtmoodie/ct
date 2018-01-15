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
                            std::conditional_t<false,
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
                                               void>> : public std::true_type
        {
        };

        template <class T, class Enable>
        struct ReflectData
        {
            static constexpr bool IS_SPECIALIZED = false;
        };

        template <int I, class T>
        static constexpr inline auto get(T& data)
            -> decltype(ReflectData<std::remove_const_t<T>>::get(data, _counter_<I>()))
        {
            return ReflectData<std::remove_const_t<T>>::get(data, _counter_<I>());
        }

        template <int I, class T>
        static constexpr inline auto get(const T& data)
            -> decltype(ReflectData<std::remove_const_t<T>>::get(data, _counter_<I>()))
        {
            return ReflectData<std::remove_const_t<T>>::get(data, _counter_<I>());
        }

        template <int I, class T>
        static constexpr inline const char* getName()
        {
            return ReflectData<std::remove_const_t<T>>::getName(_counter_<I>());
        }

        template <class T>
        constexpr int len()
        {
            return ReflectData<std::remove_const_t<T>>::N;
        }

        template <class T, class T2 = void>
        using enable_if_reflected = std::enable_if_t<ReflectData<T>::IS_SPECIALIZED, T2>;

        template <class T, class T2 = void>
        using enable_if_not_reflected = typename std::enable_if<!ReflectData<T>::IS_SPECIALIZED, T2>::type;

    } // namespace ct::reflect
} // namespace ct

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
        static constexpr auto& get(DType& data, ct::_counter_<I> cnt, std::enable_if_t < I >= 0 && I<I0> * = 0)        \
        {                                                                                                              \
            return ReflectData<BASE, void>::get(data, cnt);                                                            \
        }                                                                                                              \
        template <int I>                                                                                               \
        static constexpr const auto&                                                                                   \
        get(const DType& data, ct::_counter_<I> cnt, std::enable_if_t < I >= 0 && I<I0> * = 0)                         \
        {                                                                                                              \
            return ReflectData<BASE, void>::get(data, cnt);                                                            \
        }                                                                                                              \
        template <int I>                                                                                               \
        static constexpr const char* getName(ct::_counter_<I> cnt, std::enable_if_t < I >= 0 && I<I0> * = 0)           \
        {                                                                                                              \
            return ReflectData<BASE, void>::getName(cnt);                                                              \
        }

#define REFLECT_DATA_MEMBER(NAME) REFLECT_DATA_MEMBER_(NAME, __COUNTER__)

#define REFLECT_DATA_MEMBER_(NAME, N)                                                                                  \
    static constexpr auto& get(DType& data, ct::_counter_<N - START - 1 + I0>) { return data.NAME; }                   \
    static constexpr const auto& get(const DType& data, ct::_counter_<N - START - 1 + I0>) { return data.NAME; }       \
    static constexpr const char* getName(ct::_counter_<N - START - 1 + I0> /*dummy*/) { return #NAME; }

#define REFLECT_DATA_END                                                                                               \
    static constexpr int N = __COUNTER__ - START - 1 + I0;                                                             \
    }

#define REFLECTED_EXPORT_DECL MO_EXPORTS

#define REFLECT_TEMPLATED_DATA_START(TYPE)                                                                             \
    template <class... T>                                                                                              \
    struct ReflectData<TYPE<T...>, void>                                                                               \
    {                                                                                                                  \
        static constexpr int START = __COUNTER__;                                                                      \
        static constexpr bool IS_SPECIALIZED = true;                                                                   \
        static constexpr int I0 = 0;                                                                                   \
        typedef TYPE<T...> DType;

// Internally reflectable type, ie no external declaration of ReflectData
// Example usage:
// REFLECT_INTERNAL_START(InternallyReflected)
//    REFLECT_INTERNAL_MEMBER(float, x)
//    REFLECT_INTERNAL_MEMBER(float, y)
//    REFLECT_INTERNAL_MEMBER(float, z)
// REFLECT_INTERNAL_END;

#define REFLECT_INTERNAL_MEMBER(TYPE, NAME)                                                                            \
    TYPE NAME;                                                                                                         \
    REFLECT_DATA_MEMBER(NAME)

#define REFLECT_INTERNAL_START(TYPE)                                                                                   \
    static constexpr int START = __COUNTER__;                                                                          \
    typedef TYPE DType;                                                                                                \
    static constexpr int I0 = 0;                                                                                       \
    typedef void INTERNALLY_REFLECTED;

#define REFLECT_INTERNAL_END static constexpr int N = __COUNTER__ - START - 1 + I0
