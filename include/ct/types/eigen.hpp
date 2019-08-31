#ifndef CT_TYPES_EIGEN_HPP
#define CT_TYPES_EIGEN_HPP
#include "../reflect.hpp"
#include "TArrayView.hpp"
#include <Eigen/Core>

namespace ct
{
    template <class T, int ROWS, int COLS, int OPTS, int MAX_ROWS, int MAX_COLS>
    struct Reflect<Eigen::Matrix<T, ROWS, COLS, OPTS, MAX_ROWS, MAX_COLS>>
    {
        static constexpr int SPECIALIZED = true;
        using DataType = Eigen::Matrix<T, ROWS, COLS, OPTS, MAX_ROWS, MAX_COLS>;
        static constexpr StringView getName() { return GetName<DataType>::getName(); }

        static std::array<Eigen::Index, 2> shape(const DataType& data) { return {data.rows(), data.cols()}; }

        static void reshape(DataType& data, const std::array<Eigen::Index, 2> shape)
        {
            data.resize(shape[0], shape[1]);
        }

        static TArrayView<const T> getData(const DataType& mat) { return {mat.data(), ROWS * COLS}; }

        static TArrayView<T> getDataMutable(DataType& mat) { return {mat.data(), ROWS * COLS}; }

        REFLECT_STUB
            PROPERTY(data, &Reflect<DataType>::getData, &Reflect<DataType>::getDataMutable)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, shape, &Reflect<DataType>::shape)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, size)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, colStride)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, rowStride)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, cols)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, rows)
        REFLECT_INTERNAL_END;
        static constexpr auto end() { return ct::Indexer<NUM_FIELDS - 1>(); }
    };

    template <class T, int OPTS, int MAX_ROWS, int MAX_COLS>
    struct Reflect<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, OPTS, MAX_ROWS, MAX_COLS>>
    {
        static constexpr int SPECIALIZED = true;
        using DataType = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, OPTS, MAX_ROWS, MAX_COLS>;
        static constexpr StringView getName() { return GetName<DataType>::getName(); }

        static std::array<Eigen::Index, 2> shape(const DataType& data) { return {data.rows(), data.cols()}; }

        static void reshape(DataType& data, const std::array<Eigen::Index, 2> shape)
        {
            data.resize(shape[0], shape[1]);
        }

        static TArrayView<const T> getData(const DataType& mat)
        {
            return {mat.data(), static_cast<size_t>(mat.rows() * mat.cols())};
        }

        static TArrayView<T> getDataMutable(DataType& mat)
        {
            return {mat.data(), static_cast<size_t>(mat.rows() * mat.cols())};
        }

        REFLECT_STUB
            PROPERTY(data, &Reflect<DataType>::getData, &Reflect<DataType>::getDataMutable)
            PROPERTY(shape, &Reflect<DataType>::shape, &Reflect<DataType>::reshape)
            PROPERTY(size)
            PROPERTY(colStride)
            PROPERTY(rowStride)
            PROPERTY(cols)
            PROPERTY(rows)
        REFLECT_INTERNAL_END;
        static constexpr auto end() { return ct::Indexer<NUM_FIELDS - 1>(); }
    };

#ifdef _MSC_VER
    template <class T, int ROWS, int COLS, int OPTS, int MAX_ROWS, int MAX_COLS>
    struct CerealizerSelector<Eigen::Matrix<T, ROWS, COLS, OPTS, MAX_ROWS, MAX_COLS>, 5, void>
        : public TensorCerealizer<Eigen::Matrix<T, ROWS, COLS, OPTS, MAX_ROWS, MAX_COLS>>
    {
    };
#endif
}

#endif // CT_EIGEN_HPP
