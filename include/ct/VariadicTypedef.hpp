#pragma once
#include <utility>

namespace ct
{
    template <typename... Args>
    struct VariadicTypedef
    {
    };

    template <typename... Args>
    struct ConvertToTuple
    {
        // Leaving this empty will cause the compiler
        // to complain if you try to access a "type" member.
        // You may also be able to do something like:
        // static_assert(std::is_same<>::value, "blah")
        // if you know something about the types.
    };

    template <class... Args>
    struct ConvertFromTuple
    {
    };

    template <typename... Args>
    struct ConvertToTuple<VariadicTypedef<Args...>>
    {
        // use Args normally
        using type = std::tuple<Args...>;
    };

    template <class... Args>
    struct ConvertFromTuple<std::tuple<Args...>>
    {
        using type = VariadicTypedef<Args...>;
    };

    template <typename T, class T2>
    struct Append
    {
        using type = VariadicTypedef<T, T2>;
        using tuple_type = std::tuple<T, T2>;
    };

    template <typename T, typename... Args>
    struct Append<T, VariadicTypedef<Args...>>
    {
        using type = VariadicTypedef<T, Args...>;
        using tuple_type = std::tuple<T, Args...>;
    };

    template <typename... T, typename... Args>
    struct Append<VariadicTypedef<T...>, VariadicTypedef<Args...>>
    {
        using type = VariadicTypedef<T..., Args...>;
        using tuple_type = std::tuple<T..., Args...>;
    };

    template <typename... Args>
    struct Append<void, VariadicTypedef<Args...>>
    {
        using type = VariadicTypedef<Args...>;
        using tuple_type = std::tuple<Args...>;
    };

    template <typename... Args>
    struct Append<VariadicTypedef<Args...>, void>
    {
        using type = VariadicTypedef<Args...>;
        using tuple_type = std::tuple<Args...>;
    };

    template <typename... Args>
    struct Append<VariadicTypedef<Args...>, VariadicTypedef<void>>
    {
        using type = VariadicTypedef<Args...>;
        using tuple_type = std::tuple<Args...>;
    };

    template <typename... Args>
    struct Append<VariadicTypedef<void>, VariadicTypedef<Args...>>
    {
        using type = VariadicTypedef<Args...>;
        using tuple_type = std::tuple<Args...>;
    };

    template <typename Arg>
    struct Append<Arg, VariadicTypedef<void>>
    {
        using type = VariadicTypedef<Arg>;
        using tuple_type = std::tuple<Arg>;
    };

    template <>
    struct Append<void, VariadicTypedef<void>>
    {
        using type = VariadicTypedef<void>;
        using tuple_type = std::tuple<void>;
    };

    template <typename Arg>
    struct Append<VariadicTypedef<void>, Arg>
    {
        using type = VariadicTypedef<Arg>;
        using tuple_type = std::tuple<Arg>;
    };

    template <class... T, class Arg>
    struct Append<VariadicTypedef<T...>, Arg>
    {
        using type = VariadicTypedef<T..., Arg>;
        using tuple_type = std::tuple<T..., Arg>;
    };

    template <>
    struct Append<void, void>
    {
        using type = VariadicTypedef<void>;
        using tuple_type = std::tuple<void>;
    };

    template <class T, class U>
    using append = typename Append<T, U>::type;

    template <class T>
    using toTuple = typename ConvertToTuple<T>::type;

    template <class T>
    using fromTuple = typename ConvertFromTuple<T>::type;

    template <class T, class U>
    struct ContainsTypeImpl;

    template <class T>
    struct ContainsTypeImpl<T, ct::VariadicTypedef<>>
    {
        constexpr static const bool value = false;
    };

    template <class T, class U1>
    struct ContainsTypeImpl<T, ct::VariadicTypedef<U1>>
    {
        constexpr static const bool value = std::is_same<T, U1>::value;
    };

    template <class T, class U1, class... U>
    struct ContainsTypeImpl<T, ct::VariadicTypedef<U1, U...>>
    {
        constexpr static const bool value =
            std::is_same<T, U1>::value || ContainsTypeImpl<T, ct::VariadicTypedef<U...>>::value;
    };

    template <class T, class U>
    struct ContainsType
    {
        constexpr static const bool value = ContainsTypeImpl<T, U>::value;
    };

    template <class T1, class T2>
    struct InsertUnique
    {
        using type = typename std::conditional<ContainsType<T1, T2>::value, T2, typename Append<T1, T2>::type>::type;
    };

    template <class T, class U>
    struct InsertUniqueMultiple;

    template <class T, class U>
    struct InsertUniqueMultiple<VariadicTypedef<T>, U>
    {
        using type = typename InsertUnique<T, U>::type;
    };

    template <class U>
    struct InsertUniqueMultiple<VariadicTypedef<>, U>
    {
        using type = U;
    };

    template <class T, class... TS, class U>
    struct InsertUniqueMultiple<VariadicTypedef<T, TS...>, U>
    {
        using type = typename InsertUnique<T, typename InsertUniqueMultiple<VariadicTypedef<TS...>, U>::type>::type;
    };

    template <class... T1, class T2>
    struct InsertUnique<VariadicTypedef<T1...>, T2>
    {
        using type = typename InsertUniqueMultiple<VariadicTypedef<T1...>, T2>::type;
    };
}
