#pragma once

#include <ct/operators/Operator.hpp>
#include <ct/String.hpp>

namespace ct
{
    template<class T, typename enable = void>
    struct Square;

    template<class T>
    using data_type = typename remove_reference_t<T>::Type;

    template<class T>
    struct Square<T, enable_pod<T>>
    {
        DECLARE_CLASS_HASH;
        typedef OperatorType<T> Input_t;
        typedef decay_t<typename decay_t<T>::Type> Output_t;
        typedef typename remove_reference_t<T>::Type OutputReference_t;
        typedef decay_t<typename decay_t<T>::Type> Type;
        enum
        {
            ConstSize = remove_reference_t<T>::ConstSize,
            Dims = remove_reference_t<T>::Dims,
            IsIndexable = remove_reference_t<T>::IsIndexable
        };
        Square(T&& value_):
            value(std::forward<T>(value_))
        {
        }

        Output_t operator()()
        {
            Output_t  tmp = value();
            return tmp*tmp;
        }
        
        Input_t value;
    };

    template<class T>
    struct Square<T, enable_indexable<T>>
    {
        DECLARE_CLASS_HASH;
        typedef OperatorType<T> Input_t;
        typedef decay_t<typename decay_t<T>::Elem_t> Output_t;
        typedef typename remove_reference_t<T>::Elem_t OutputReference_t;
        typedef decay_t<typename decay_t<T>::Type> Type;
        typedef decay_t<typename decay_t<T>::Elem_t> Elem_t;
        enum
        {
            ConstSize = remove_reference_t<T>::ConstSize,
            Dims = remove_reference_t<T>::Dims,
            IsIndexable = remove_reference_t<T>::IsIndexable
        };
        Square(T&& value_) :
            value(std::forward<T>(value_))
        {
        }

        template<class... Dims>
        Output_t operator()(Dims... dims)
        {
            Output_t  tmp = value(dims...);
            return tmp*tmp;
        }

        Input_t value;
    };

    template<class T, typename enable = void>
    struct Sqrt;

    template<class T>
    struct Sqrt<T, enable_pod<T>>
    {
        DECLARE_CLASS_HASH;
        typedef OperatorType<T> Input_t;
        typedef remove_reference_t<typename remove_reference_t<T>::Type> Output_t;
        typedef typename remove_reference_t<T>::Type OutputReference_t;
        typedef Output_t Type;
        enum
        {
            ConstSize = remove_reference_t<T>::ConstSize,
            Dims = remove_reference_t<T>::Dims,
            IsIndexable = remove_reference_t<T>::IsIndexable
        };

        Sqrt(T&& value_) :
            value(std::forward<T>(value_))
        {
        }

        Output_t operator()()
        {
            return std::sqrt(value());
        }
        
        Input_t value;
    };
    template<class T>
    struct Sqrt<T, enable_indexable<T>>
    {
        DECLARE_CLASS_HASH;
        typedef OperatorType<T> Input_t;
        typedef remove_reference_t<typename remove_reference_t<T>::Elem_t> Output_t;
        typedef typename remove_reference_t<T>::Elem_t OutputReference_t;
        typedef Output_t Elem_t;
        enum
        {
            ConstSize = remove_reference_t<T>::ConstSize,
            Dims = remove_reference_t<T>::Dims,
            IsIndexable = remove_reference_t<T>::IsIndexable
        };

        Sqrt(T&& value_) :
            value(std::forward<T>(value_))
        {
        }

        template<class ... Dims>
        Output_t operator()(Dims... dims)
        {
            return std::sqrt(value(dims...));
        }

        Input_t value;
    };


    template<template<class, class> class OP, class IN, class... Args>
    OP<IN, void> makeUnary(IN&& input, Args... args)
    {
        return {std::forward<IN>(input), args...};
    }
} // namespace ct
