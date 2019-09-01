#ifndef CT_BIND_HPP
#define CT_BIND_HPP
#include <cstddef>
#include <functional>
#include <type_traits>
// *************************************************************************
// http://stackoverflow.com/questions/21192659/variadic-templates-and-stdbind
namespace ct
{

    template <int...>
    struct int_sequence
    {
    };
    template <int N, int... Is>
    struct make_int_sequence : make_int_sequence<N - 1, N - 1, Is...>
    {
    };
    template <int... Is>
    struct make_int_sequence<0, Is...> : int_sequence<Is...>
    {
    };
    template <int>
    struct placeholder_template
    {
    };

    template <class R, class... Args, int... Is>
    std::function<R(Args...)> variadicBind(R (*p)(Args...), int_sequence<Is...>)
    {
        return std::bind(p, placeholder_template<Is>{}...);
    }

    template <class R, class C, class B, class... Args, int... Is>
    std::function<R(Args...)> variadicBind(R (C::*p)(Args...), B* ptr, int_sequence<Is...>)
    {
        return std::bind(p, ptr, placeholder_template<Is>{}...);
    }

    template <class R, class C, class B, class... Args, int... Is>
    std::function<R(Args...)> variadicBind(R (C::*p)(Args...) const, const B* ptr, int_sequence<Is...>)
    {
        return std::bind(p, ptr, placeholder_template<Is>{}...);
    }

    template <class R, class C, class B, class... Args, int... Is>
    std::function<R(Args...)> variadicBind(R (*p)(const C&, Args...), const B* ptr, int_sequence<Is...>)
    {
        return std::bind(p, ptr, placeholder_template<Is>{}...);
    }

    template <class R, class C, class B, class... Args, int... Is>
    std::function<R(Args...)> variadicBind(R (*p)(C&, Args...), B* ptr, int_sequence<Is...>)
    {
        return std::bind(p, ptr, placeholder_template<Is>{}...);
    }
}

namespace std
{
    template <int N>
    struct is_placeholder<ct::placeholder_template<N>> : integral_constant<int, N + 1>
    {
    };
}
// *************************************************************************
#endif // CT_BIND_HPP
