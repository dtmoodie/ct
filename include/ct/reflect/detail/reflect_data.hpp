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
        static constexpr inline auto getValue(const T& data) -> std::decay_t<decltype(get<I, T>(data))>
        {
            return get<I, T>(data);
        }

        template <int I, class T>
        static constexpr inline void setValue(T& data, const std::decay_t<decltype(get<I, T>(data))>& value)
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
