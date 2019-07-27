#ifndef CT_CONCEPTS_HPP
#define CT_CONCEPTS_HPP
#include "reflect.hpp"

namespace ct
{
    template<class T, template<class> class CONCEPT>
    constexpr bool conceptCheck()
    {
        return CONCEPT<T>::value;
    }
}

#endif // CT_CONCEPTS_HPP
