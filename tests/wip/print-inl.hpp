#pragma once
#include "print.hpp"
#include "reflect.hpp"

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
    template<int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& os, const T& obj)-> typename std::enable_if<
        !std::is_same<typename decltype(Reflect<T>::getAccessor(ct::_counter_<I>{}))::GetterTraits_t, CalculatedValue>::value ||
        Options::print_calculated_values == true>::type
    {
        auto accessor = Reflect<T>::getAccessor(ct::_counter_<I>{});
        if(Options::print_name)
        {
            os << accessor.getName() << Options::name_separator;
        }
        os << accessor.get(obj);
    }

    template<int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& , const T& )-> typename std::enable_if<
        std::is_same<typename decltype(Reflect<T>::getAccessor(ct::_counter_<I>{}))::GetterTraits_t, CalculatedValue>::value &&
        Options::print_calculated_values == false>::type
    {

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

    template<class Options = PrintOptions, class T>
    void printStruct(std::ostream& os, const T& obj)
    {
        os << Options::object_begin;
        printStructHelper<Options>(os, obj, Reflect<T>::end());
        os << Options::object_end;
    }

}
