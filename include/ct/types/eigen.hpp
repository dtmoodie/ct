#ifndef CT_TYPES_EIGEN_HPP
#define CT_TYPES_EIGEN_HPP
#include "../reflect.hpp"
#include <Eigen/Core>

namespace ct
{
    template <class T, int ROWS, int COLS>
    struct Reflect<Eigen::Matrix<T, ROWS, COLS>>
    {
        static constexpr int SPECIALIZED = true;
        using DataType = Eigen::Matrix<T, ROWS, COLS>;
        static constexpr StringView getName() { return GetName<DataType>::getName(); }

        REFLECT_STUB
            MEMBER_FUNCTION(data,
                            static_cast<const T* (DataType::*)() const>(&DataType::data),
                            static_cast<T* (DataType::*)()>(&DataType::data))
            MEMBER_FUNCTION(size)
            MEMBER_FUNCTION(colStride)
            MEMBER_FUNCTION(rowStride)
            MEMBER_FUNCTION(cols)
            MEMBER_FUNCTION(rows)

        REFLECT_INTERNAL_END;
        static constexpr auto end() { return ct::Indexer<NUM_FIELDS - 1>(); }
    };
}

#endif // CT_EIGEN_HPP
