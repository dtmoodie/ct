#pragma once
#include "print.hpp"
#include <cstdint>
#include <ct/TypeTraits.hpp>
#include <ct/reflect.hpp>
#include <memory>

namespace std
{
    template <class T>
    auto operator<<(ostream& os, const T& obj) -> ct::EnableIfReflected<T, ostream&>
    {
        thread_local bool recursion_block = false;
        if(recursion_block)
        {
            return os;
        }
        recursion_block = true;
        ct::printStruct(os, obj);
        recursion_block = false;
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
        using DType = typename FieldGetType<T, I>::type;
        enum
        {
            value = StreamWritable<typename std::decay<DType>::type>::value
        };
    };

    template <class T, class O, int I>
    struct ShouldWrite
    {
        using Ptr_t = PtrType<T, I>;
        using DType = typename GetType<Ptr_t>::type;
        constexpr static const bool is_calculated = IsMemberFunctionPointers<Ptr_t>::value;
        constexpr static const bool is_writable = CanWrite<T, I>::value;
        constexpr static const bool value = is_writable && ((is_calculated && O::print_calculated_values == true) || is_calculated == false);

        static_assert(is_writable == true || O::error_on_nonprintable == false,
                      "Either need to be able to write this data type or need to disable it in options");
    };

    template <int I, class Options = PrintOptions, class T>
    auto printParam(std::ostream& os, const T& obj) ->
        typename std::enable_if<ShouldWrite<T, Options, I>::value && IsMemberFunction<T, I>::value == false>::type
    {
        // If you get a "no type named 'type' in struct std::enable_if<false, void> here, it's because the type is not
        // stream writable and your print options do not allow that via the error_on_nonprintable flag

        auto accessor = Reflect<T>::getPtr(ct::Indexer<I>{});

        if (Options::print_name)
        {
            os << accessor.m_name << Options::name_separator;
        }

        os << get(accessor, obj);
    }

    template<class T, index_t I, index_t J = 0>
    struct CountArgs
    {
        using Ptr_t = PtrType<T, I>;
        using FunctionPtr_t = typename std::decay<decltype(std::get<J>(std::declval<Ptr_t>().m_ptrs))>::type;
        constexpr static const index_t NUM_ARGS = InferPointerType<FunctionPtr_t>::NUM_ARGS;
    };

    template<class T, index_t I, class U = void>
    using EnableIfNoArgs = typename std::enable_if<CountArgs<T, I>::NUM_ARGS == 0, U>::type;

    template<class T, index_t I, class U = void>
    using EnableIfArgs = typename std::enable_if<CountArgs<T, I>::NUM_ARGS >= 1, U>::type;

    template <int I, class Options, class T>
    auto printMemberFunctionResult(std::ostream& os, const T& obj) -> EnableIfNoArgs<T, I>
    {
        auto accessor = Reflect<T>::getPtr(ct::Indexer<I>{});
        if (Options::print_name)
        {
            os << accessor.m_name << Options::name_separator;
        }

        os << invoke<0>(accessor, obj);
    }

    template <int I, class Options, class T>
    auto printMemberFunctionResult(std::ostream& , const T& ) -> EnableIfArgs<T, I>
    {
    }

    template <int I, class Options = PrintOptions, class T>
    auto printParam(std::ostream& os, const T& obj) ->
        typename std::enable_if<ShouldWrite<T, Options, I>::value && IsMemberFunction<T, I>::value == true>::type
    {
        // If you get a "no type named 'type' in struct std::enable_if<false, void> here, it's because the type is not
        // stream writable and your print options do not allow that via the error_on_nonprintable flag
        printMemberFunctionResult<I, Options, T>(os, obj);
    }

    template <int I, class Options = PrintOptions, class T>
    auto printParam(std::ostream& os, const T& data) ->
        typename std::enable_if<!ShouldWrite<T, Options, I>::value>::type
    {
        if (ShouldWrite<T, Options, I>::is_writable == false && Options::error_on_nonprintable == false)
        {
            auto accessor = Reflect<T>::getPtr(ct::Indexer<I>{});
            Options::onUnprintable(os, Reflect<T>::getName(ct::Indexer<I>{}), accessor.invoke(data));
        }
    }

    template<int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& os, const T& obj)
        -> typename std::enable_if<!IsMemberFunction<T, I>::value>::type
    {
        printParam<I, Options>(os, obj);
    }

    template<int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& os, const T& obj)
        -> typename std::enable_if<IsMemberFunction<T, I>::value>::type
    {
        printMemberFunctionResult<I, Options>(os, obj);
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
    auto printStruct(std::ostream& os, const T& obj) -> ct::EnableIfReflected<T>
    {
        os << Options::object_begin;
        printStructHelper<Options>(os, obj, Reflect<T>::end());
        os << Options::object_end;
    }

    template <class Options = PrintOptions, class T>
    auto printStruct(std::ostream& os, const std::shared_ptr<T>& obj) -> ct::EnableIfReflected<T>
    {
        if (obj)
        {

            os << Options::object_begin;
            printStructHelper<Options>(os, *obj, Reflect<T>::end());
            os << Options::object_end;
        }
    }
}
