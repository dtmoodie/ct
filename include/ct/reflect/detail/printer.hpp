#pragma once
#include <ct/detail/counter.hpp>
#include <ct/reflect/reflect_data.hpp>
#include <ct/reflect/printer.hpp>
#include <vector>
#include <typeinfo>

namespace ct
{
    namespace reflect
    {
        namespace detail
        {
            template <class T>
            struct stream_serializable {
                template <class U>
                static constexpr auto check(std::ostream* os, U* val) -> decltype(*os << *val, size_t(0))
                {
                    return 0;
                }

                template <class U>
                static constexpr int check(...)
                {
                    return 0;
                }
                static const bool value = sizeof(check<T>(static_cast<std::ostream*>(nullptr), static_cast<T*>(nullptr))) == sizeof(size_t);
            };

            template<class T>
            static constexpr inline auto printDataHelper(std::ostream& os, const T& data) -> typename std::enable_if<stream_serializable<T>::value>::type
            {
                os << data;
            }

            template<class T>
            static constexpr inline auto printDataHelper(std::ostream& os, const T& data) -> typename std::enable_if<!stream_serializable<T>::value>::type
            {
                os << "No serializer available for " << typeid(T).name();
            }

            template <class T>
            static constexpr inline void printStructHelper(std::ostream& os, const T& data, _counter_<0>)
            {
                os << getName<0, T>() << ':';
                printDataHelper(os, get<0>(data));
            }

            template <int I, class T>
            static constexpr inline void printStructHelper(std::ostream& os, const T& data, _counter_<I>)
            {
                printStructHelper(os, data, _counter_<I - 1>());
                os << ' ' << getName<I, T>() << ':';
                printDataHelper(os, get<I>(data));
            }
        }
    }
}
