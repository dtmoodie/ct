#ifndef CT_REFLECT_UTILS_HPP

#define CT_REFLECT_UTILS_HPP

namespace ct
{
    // The following two functions can be used to disambiguate a function, hopefully
    template <class T, class R, class... ARGS>
    constexpr R (T::*selectConstMemberFunctionPointer(R (T::*ptr)(ARGS...) const))(ARGS...) const
    {
        return ptr;
    }

    template <class T, class R, class... ARGS>
    constexpr R (T::*selectMemberFunctionPointer(R (T::*ptr)(ARGS...)))(ARGS...)
    {
        return ptr;
    }

    template <class T, class R, class... ARGS>
    constexpr R (*selectFunctionPointer(R (*ptr)(ARGS...)))(ARGS...)
    {
        return ptr;
    }
} // namespace ct

#endif // CT_REFLECT_UTILS_HPP