#ifndef CT_REFLECT_FORWARD_HPP
#define CT_REFLECT_FORWARD_HPP
#include "Indexer.hpp"
#include "VariadicTypedef.hpp"
#include "type_traits.hpp"

namespace ct
{
    template <class T, class VISITED = VariadicTypedef<>, class ENABLE = void>
    struct Reflect;

    template <class T>
    struct ReflectImpl;

    struct EmptyImplementation;

    template <class T, class V>
    struct ReflectBasesImpl;

    template <class T, class V>
    struct ReflectBases;

    template <class T, class IMPL, class VISITED, class ENABLE = void>
    struct ImplementationFilter;

    template <class IMPL, class ENABLE = void>
    struct BaseSelector;

    template <class T, class E = void>
    struct IsReflected;

    template <class T, index_t I>
    struct IsMemberFunction;
}

#endif // CT_REFLECT_FORWARD_HPP
