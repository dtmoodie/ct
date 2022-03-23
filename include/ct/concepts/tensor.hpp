#ifndef CT_CONCEPTS_TENSOR_HPP
#define CT_CONCEPTS_TENSOR_HPP
#include "../reflect.hpp"

namespace ct
{
    template <class T>
    struct TensorConcept
    {
        constexpr static const index_t DATA_INDEX = indexOfField<T>("data");
        constexpr static const index_t SHAPE_INDEX = indexOfField<T>("shape");
        constexpr static const index_t SIZE_INDEX = indexOfField<T>("size");
        static constexpr const bool value = DATA_INDEX != -1 && SHAPE_INDEX != -1 && SIZE_INDEX != -1;
    };
} // namespace ct

#endif // CT_CONCEPTS_TENSOR_HPP
