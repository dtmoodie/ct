#pragma once
#include <utility>

namespace ct
{
    template <typename... Args>
    struct variadic_typedef
    {
    };

    template <typename... Args>
    struct convert_in_tuple
    {
        // Leaving this empty will cause the compiler
        // to complain if you try to access a "type" member.
        // You may also be able to do something like:
        // static_assert(std::is_same<>::value, "blah")
        // if you know something about the types.
    };

    template <typename... Args>
    struct convert_in_tuple<variadic_typedef<Args...>>
    {
        // use Args normally
        typedef std::tuple<Args...> type;
    };

    template <typename T, class T2>
    struct append_to_tupple
    {
    };

    template <typename T, typename... Args>
    struct append_to_tupple<T, variadic_typedef<Args...>>
    {
        typedef variadic_typedef<T, Args...> type;
        typedef std::tuple<T, Args...> tuple_type;
    };

    template <typename... Args>
    struct append_to_tupple<void, variadic_typedef<Args...>>
    {
        typedef variadic_typedef<Args...> type;
        typedef std::tuple<Args...> tuple_type;
    };

    template <typename... Args>
    struct append_to_tupple<variadic_typedef<Args...>, void>
    {
        typedef variadic_typedef<Args...> type;
        typedef std::tuple<Args...> tuple_type;
    };

    template <typename Arg>
    struct append_to_tupple<Arg, variadic_typedef<void>>
    {
        typedef variadic_typedef<Arg> type;
        typedef std::tuple<Arg> tuple_type;
    };

    template <>
    struct append_to_tupple<void, variadic_typedef<void>>
    {
        typedef variadic_typedef<void> type;
        typedef std::tuple<void> tuple_type;
    };

    template <typename Arg>
    struct append_to_tupple<variadic_typedef<void>, Arg>
    {
        typedef variadic_typedef<Arg> type;
        typedef std::tuple<Arg> tuple_type;
    };

    template <>
    struct append_to_tupple<void, void>
    {
        typedef variadic_typedef<void> type;
        typedef std::tuple<void> tuple_type;
    };
}
