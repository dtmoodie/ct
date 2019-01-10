#pragma once
#include "print.hpp"
#include <cstdint>
#include <ct/TypeTraits.hpp>
#include <ct/reflect.hpp>
#include <memory>

namespace std
{
    template <class T>
    auto operator<<(ostream& os, const T& obj) -> ct::enable_if_reflected<T, ostream&>
    {
        ct::printStruct(os, obj);
        return os;
    }

    template <class T>
    auto operator<<(ostream& os, const vector<T>& vec) ->
        typename std::enable_if<ct::StreamWritable<T>::value, ostream&>::type
    {
        if (vec.empty())
        {
            os << "[empty]";
        }
        else
        {
            os << "[";
            for (size_t i = 0; i < vec.size(); ++i)
            {
                if (i != 0)
                    os << ' ';
                os << vec[i];
            }
            os << ']';
        }
        return os;
    }
}

namespace ct
{

    template <class T, int I>
    struct CanWrite
    {
        using DType = typename GetterType<T, I>::type;
        enum
        {
            value = StreamWritable<DType>::value
        };
    };

    template <class T, class O, int I>
    struct ShouldWrite
    {
        using DType = typename GetterType<T, I>::type;
        using GetterTraits = typename ct::GetterTraits<T, I>::type;
        enum
        {
            is_calculated = std::is_same<GetterTraits, CalculatedValue>::value,
            is_writable = CanWrite<T, I>::value,
            value = is_writable && ((is_calculated && O::print_calculated_values == true) || is_calculated == false)
        };
        static_assert(is_writable == true || O::error_on_nonprintable == false,
                      "Either need to be able to write this data type or need to disable it in options");
    };

    template <int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& os, const T& obj) ->
        typename std::enable_if<ShouldWrite<T, Options, I>::value && IsMemberFunction<T, I>::value == false>::type
    {
        // If you get a "no type named 'type' in struct std::enable_if<false, void> here, it's because the type is not
        // stream writable and your print options do not allow that via the error_on_nonprintable flag

        auto accessor = Reflect<T>::getAccessor(ct::Indexer<I>{});

        if (Options::print_name)
        {
            os << Reflect<T>::getName(ct::Indexer<I>{}) << Options::name_separator;
        }

        os << accessor.get(obj);
    }

    template <int I, class Options, class T>
    auto printMemberFunctionResult(std::ostream&, const T&) ->
        typename std::enable_if<!std::is_same<typename AccessorType<T, I>::Args_t, VariadicTypedef<>>::value ||
                                AccessorType<T, I>::IS_CONST == false>::type
    {
    }

    template <int I, class Options, class T>
    auto printMemberFunctionResult(std::ostream& os, const T& obj) ->
        typename std::enable_if<std::is_same<typename AccessorType<T, I>::Args_t, VariadicTypedef<>>::value &&
                                AccessorType<T, I>::IS_CONST == true>::type
    {
        auto accessor = Reflect<T>::getAccessor(ct::Indexer<I>{});
        if (Options::print_name)
        {
            os << Reflect<T>::getName(ct::Indexer<I>{}) << Options::name_separator;
        }

        os << accessor.invoke(obj);
    }

    template <int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& os, const T& obj) ->
        typename std::enable_if<ShouldWrite<T, Options, I>::value && IsMemberFunction<T, I>::value == true>::type
    {
        // If you get a "no type named 'type' in struct std::enable_if<false, void> here, it's because the type is not
        // stream writable and your print options do not allow that via the error_on_nonprintable flag
        printMemberFunctionResult<I, Options, T>(os, obj);
    }

    template <int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& os, const T& data) ->
        typename std::enable_if<!ShouldWrite<T, Options, I>::value>::type
    {
        if (ShouldWrite<T, Options, I>::is_writable == false && Options::error_on_nonprintable == false)
        {
            auto accessor = Reflect<T>::getAccessor(ct::Indexer<I>{});
            Options::onUnprintable(os, Reflect<T>::getName(ct::Indexer<I>{}), accessor.invoke(data));
        }
    }

    template <class Options = PrintOptions, class T>
    void printStructHelper(std::ostream& os, const T& obj, const ct::Indexer<0U>)
    {
        printValue<0, Options>(os, obj);
    }

    template <class Options = PrintOptions, index_t I, class T>
    void printStructHelper(std::ostream& os, const T& obj, const ct::Indexer<I> idx)
    {
        printStructHelper<Options>(os, obj, --idx);
        os << Options::value_separator;
        printValue<I, Options>(os, obj);
    }

    template <class Options, class T>
    auto printStruct(std::ostream& os, const T& obj) -> ct::enable_if_reflected<T>
    {
        os << Options::object_begin;
        printStructHelper<Options>(os, obj, Reflect<T>::end());
        os << Options::object_end;
    }

    template <class Options = PrintOptions, class T>
    auto printStruct(std::ostream& os, const std::shared_ptr<T>& obj) -> ct::enable_if_reflected<T>
    {
        if (obj)
        {

            os << Options::object_begin;
            printStructHelper<Options>(os, *obj, Reflect<T>::end());
            os << Options::object_end;
        }
    }
}
