#ifndef CT_POINTER_TRAITS_HPP
#define CT_POINTER_TRAITS_HPP

namespace ct
{

    template <class T>
    struct GetType;

    template <class T>
    struct SetType;

    template <class PTR>
    struct InferPointerType;

    template <class DTYPE, class CTYPE, class... ARGS>
    struct InferPointerType<DTYPE (CTYPE::*)(ARGS...)>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        static constexpr const bool IMPLICIT_THIS = false;
        static constexpr const int NUM_ARGS = sizeof...(ARGS);
    };

    template <class DTYPE, class CTYPE, class... ARGS>
    struct InferPointerType<DTYPE (CTYPE::*)(ARGS...) const>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        static constexpr const bool IMPLICIT_THIS = false;
        static constexpr const int NUM_ARGS = sizeof...(ARGS);
    };

    // Visual studio has issues so we make an explicit version for when args is nothing
    template <class DTYPE, class CTYPE>
    struct InferPointerType<DTYPE (CTYPE::*)() const>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        static constexpr const bool IMPLICIT_THIS = false;
        static constexpr const int NUM_ARGS = 0;
    };

    template <class DTYPE, class CTYPE, class... ARGS>
    struct InferPointerType<DTYPE (*)(CTYPE, ARGS...)>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        static constexpr const bool IMPLICIT_THIS = true;
        static constexpr const int NUM_ARGS = sizeof...(ARGS);
    };

    template <class DTYPE, class... ARGS>
    struct InferPointerType<DTYPE (*)(ARGS...)>
    {
        using Class_t = void;
        using Data_t = DTYPE;
        static constexpr const bool IMPLICIT_THIS = false;
        static constexpr const int NUM_ARGS = sizeof...(ARGS);
    };

    template <class... TYPES>
    struct InferClassType;

    template <class T>
    struct InferClassType<T>
    {
        using Class_t = typename InferPointerType<T>::Class_t;
    };

    template <class T, class... TYPES>
    struct InferClassType<T, TYPES...>
    {
        using Class_t = typename InferPointerType<T>::Class_t;
    };

    template <class PTR>
    struct InferSetterType;

    template <class DTYPE, class CTYPE>
    struct InferSetterType<DTYPE (CTYPE::*)()>
    {
        using type = DTYPE;
    };

    template <class DTYPE, class CTYPE>
    struct InferSetterType<void (CTYPE::*)(DTYPE)>
    {
        using type = DTYPE;
    };

    // operator =
    template <class DTYPE, class CTYPE>
    struct InferSetterType<CTYPE& (CTYPE::*)(DTYPE)>
    {
        using type = ct::decay_t<DTYPE>;
    };

    template <class DTYPE, class CTYPE>
    struct InferSetterType<void (*)(CTYPE, DTYPE)>
    {
        using type = DTYPE;
    };

    // Explicit this override
    template <class DTYPE, class CTYPE>
    struct InferSetterType<DTYPE (*)(CTYPE&)>
    {
        using type = DTYPE;
    };

} // namespace ct

#endif // CT_POINTER_TRAITS_HPP
