#ifndef CT_REFLECT_MEMBER_FUNCTION_HPP
#define CT_REFLECT_MEMBER_FUNCTION_HPP
#include "accessor_traits.hpp"
#include "../VariadicTypedef.hpp"
#include <utility>

namespace ct
{
    template<class T>
    struct MemberFunction{};

    template<class R, class T, class ... Args>
    struct MemberFunction<R(T::*)(Args...)>
    {
        using Args_t = VariadicTypedef<Args...>;
        using Return_t = R;
        using Get_t = Return_t;
        static const bool IS_CONST = false;

        constexpr MemberFunction(R(T::* fptr)(Args...)):
            m_fptr(fptr)
        {

        }

        R invoke(T& inst, Args&&... args) const
        {
            return (inst.*m_fptr)(std::forward<Args>(args)...);
        }

        R(T::*m_fptr)(Args...);
    };

    template<class R, class T, class ... Args>
    struct MemberFunction<R(T::*)(Args...) const>
    {
        using Args_t = VariadicTypedef<Args...>;
        using Return_t = R;
        using Get_t = Return_t;
        static const bool IS_CONST = true;

        constexpr MemberFunction(R(T::* fptr)(Args...) const):
            m_fptr(fptr)
        {

        }

        R invoke(const T& inst, Args&&... args) const
        {
            return (inst.*m_fptr)(std::forward<Args>(args)...);
        }

        R(T::*m_fptr)(Args...) const;
    };
}

#endif // CT_REFLECT_MEMBER_FUNCTION_HPP
