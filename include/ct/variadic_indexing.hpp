#ifndef CT_VARIADIC_INDEXING_HPP
#define CT_VARIADIC_INDEXING_HPP
#include "Indexer.hpp"
#include "VariadicTypedef.hpp"
namespace ct
{
    template <class R, index_t I, class... Ts>
    R getImpl(Indexer<I>, Indexer<I>, R&& arg, Ts&&...)
    {
        return std::forward<R>(arg);
    }

    template <class R, index_t I, index_t J, class T, class... Ts>
    R getImpl(Indexer<I> idx, Indexer<J> itr, T&& arg, Ts&&... args)
    {
        return getImpl<R>(idx, ++itr, std::forward<Ts>(args)...);
    }

    template <index_t I, class... Ts>
    auto get(Ts&&... args) -> typename VariadicTypedef<Ts...>::template TypeAt<I>
    {
        return getImpl<typename VariadicTypedef<Ts...>::template TypeAt<I>>(
            Indexer<I>{}, Indexer<0>{}, std::forward<Ts>(args)...);
    }

} // namespace ct

#endif // CT_VARIADIC_INDEXING_HPP