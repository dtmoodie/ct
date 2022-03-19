#ifndef CT_VARIADIC_INDEXING_HPP
#define CT_VARIADIC_INDEXING_HPP
#include "Indexer.hpp"
#include "VariadicTypedef.hpp"
#include "type_traits.hpp"

namespace ct
{
    template <class R, index_t I, class... Ts>
    R getImpl(Indexer<I>, Indexer<I>, R&& arg, Ts&&...)
    {
        return std::forward<R>(arg);
    }

    template <class R, index_t I, index_t J, class T, class... Ts>
    ct::EnableIf<std::is_same<R, T>::value == false, R> getImpl(Indexer<I> idx, Indexer<J> itr, T&& arg, Ts&&... args)
    {
        return getImpl<R>(idx, ++itr, std::forward<Ts>(args)...);
    }

    template <index_t I, class... Ts>
    auto get(Ts&&... args) -> typename VariadicTypedef<Ts...>::template TypeAt<I>
    {
        return getImpl<typename VariadicTypedef<Ts...>::template TypeAt<I>>(
            Indexer<I>{}, Indexer<0>{}, std::forward<Ts>(args)...);
    }

    template <class T, class... Ts>
    T& get(std::tuple<Ts...>& tup)
    {
        return std::get<VariadicTypedef<Ts...>::template indexOfType<T>()>(tup);
    }

    template <class T, class... Ts>
    const T& get(const std::tuple<Ts...>& tup)
    {
        return std::get<VariadicTypedef<Ts...>::template indexOfType<T>()>(tup);
    }

} // namespace ct

#endif // CT_VARIADIC_INDEXING_HPP
