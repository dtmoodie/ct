#pragma once
#include "print.hpp"
#include "reflect.hpp"
#include <cstdint>

namespace std
{
    template<class T>
    auto operator<<(ostream& os, const T& obj) -> ct::enable_if_reflected<T, ostream&>
    {
        ct::printStruct(os, obj);
        return os;
    }
}

namespace ct
{
    namespace detail
    {
        template <class T>
        struct stream_writable {
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
    }

    template<class T, int I>
    struct CanWrite
    {
        using DType = typename decltype(Reflect<T>::getAccessor(ct::_counter_<I>{}))::GetType;
        enum{value = detail::stream_writable<DType>::value};
    };

    template<class T, class O, int I>
    struct ShouldWrite
    {
        using DType = typename decltype(Reflect<T>::getAccessor(ct::_counter_<I>{}))::GetType;
        using GetterTraits = typename decltype(Reflect<T>::getAccessor(ct::_counter_<I>{}))::GetterTraits_t;
        enum{
            is_calculated = std::is_same<GetterTraits, CalculatedValue>::value,
            is_writable   = CanWrite<T, I>::value,
            value = is_writable && ((is_calculated && O::print_calculated_values == true) || is_calculated == false)
        };
        static_assert(is_writable == true || O::error_on_nonprintable == false, "Either need to be able to write this data type or need to disable it in options");
    };

    template<int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& os, const T& obj)->
        typename std::enable_if<ShouldWrite<T, Options, I>::value>::type
    {
        // If you get a "no type named 'type' in struct std::enable_if<false, void> here, it's because the type is not
        // stream writable and your print options do not allow that via the error_on_nonprintable flag

        auto accessor = Reflect<T>::getAccessor(ct::_counter_<I>{});

        if(Options::print_name)
        {
            os << accessor.getName() << Options::name_separator;
        }

        os << accessor.get(obj);
    }

    template<int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& os, const T& data)->
        typename std::enable_if<!ShouldWrite<T, Options, I>::value>::type
    {
        if(ShouldWrite<T, Options, I>::is_writable == false && Options::error_on_nonprintable == false)
        {
            auto accessor = Reflect<T>::getAccessor(ct::_counter_<I>{});
            Options::onUnprintable(os, accessor.getName(), accessor.get(data));
        }
    }

    template<class Options = PrintOptions, class T>
    void printStructHelper(std::ostream& os, const T& obj, const ct::_counter_<0>)
    {
        printValue<0, Options>(os, obj);
    }

    template<class Options = PrintOptions, int I, class T>
    void printStructHelper(std::ostream& os, const T& obj, const ct::_counter_<I> idx)
    {
        printStructHelper<Options>(os, obj, --idx);
        os << Options::value_separator;
        printValue<I, Options>(os, obj);
    }

    template<class Options, class T>
    void printStruct(std::ostream& os, const T& obj)
    {
        os << Options::object_begin;
        printStructHelper<Options>(os, obj, Reflect<T>::end());
        os << Options::object_end;
    }

}
