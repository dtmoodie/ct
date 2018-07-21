#ifndef CT_REFLECT_DETAIL_REFLECT_DATA_HPP
#define CT_REFLECT_DETAIL_REFLECT_DATA_HPP
#include "ct/reflect/reflect_data.hpp"
#include <ct/detail/counter.hpp>

#include <ostream>
#include <type_traits>

namespace ct
{
    namespace reflect
    {
        template<bool Enable, typename DType = void>
        using enable_if_t = typename std::enable_if<Enable, DType>::type;

        template <class T, class Enable>
        struct ReflectData;
        template <class T, class T2 = void>
        using enable_if_reflected = enable_if_t<ReflectData<T, void>::IS_SPECIALIZED, T2>;

        template <class T, class T2 = void>
        using enable_if_not_reflected = typename std::enable_if<!ReflectData<T, void>::IS_SPECIALIZED, T2>::type;

        template <class T, class Enable = void>
        struct ReflectData;
		// MSVC doesn't like these to be forward declared
#ifndef _MSC_VER
        template <int I, class T>
        static constexpr inline auto get(T& data)
            -> decltype(ReflectData<typename std::remove_const<T>::type>::get(data, _counter_<I>()));

        template <int I, class T>
        static constexpr inline auto get(const T& data)
            -> decltype(ReflectData<typename std::remove_const<T>::type>::get(data, _counter_<I>()));
#endif
        template <int I, class T>
        static constexpr inline auto getValue(const T& data) -> typename std::decay<decltype(get<I, T>(data))>::type
        {
            return get<I, T>(data);
        }

        template <int I, class T>
        static inline void setValue(T& data, const typename std::decay<decltype(get<I, T>(data))>::type& value)
        {
            get<I, T>(data) = value;
        }

        template <int I, class T>
        static constexpr inline const char* getName();

        // Specialization for internally reflected data
        template <class T>
        struct ReflectData<T, decltype(T::get(std::declval<T>(), _counter_<0>()), void())>
        {
            static constexpr bool IS_SPECIALIZED = true;
            static constexpr int N = T::N;
            static constexpr const char* getName() { return T::getName(); }

            static constexpr auto get(const T& data, _counter_<0>) -> decltype(T::get(data, _counter_<0>()))
            {
                return T::get(data, _counter_<0>());
            }

            static constexpr auto get(T& data, _counter_<0>) -> decltype(T::get(data, _counter_<0>()))
            {
                return T::get(data, _counter_<0>());
            }

            template <int I>
            static constexpr auto get(const T& data, _counter_<I>) -> decltype(T::get(data, _counter_<I>()))
            {
                return T::get(data, _counter_<I>());
            }

            template <int I>
            static constexpr auto get(T& data, _counter_<I>) -> decltype(T::get(data, _counter_<I>()))
            {
                return T::get(data, _counter_<I>());
            }

            static constexpr const char* getName(_counter_<0> cnt) { return T::getName(cnt); }

            template <int I>
            static constexpr const char* getName(_counter_<I> cnt)
            {
                return T::getName(cnt);
            }
        };
    }
}
#endif