#ifndef CT_TYPES_EIGEN_HPP
#define CT_TYPES_EIGEN_HPP
#include "../reflect.hpp"
#include "TArrayView.hpp"
#include <Eigen/Core>
#include <array>

namespace ct
{
    template <class T, int ROWS, int COLS, int OPTS, int MAX_ROWS, int MAX_COLS>
    struct ReflectImpl<Eigen::Matrix<T, ROWS, COLS, OPTS, MAX_ROWS, MAX_COLS>, void>
    {
        using DataType = Eigen::Matrix<T, ROWS, COLS, OPTS, MAX_ROWS, MAX_COLS>;
        using this_t = ReflectImpl<DataType, void>;
        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }

        static std::array<Eigen::Index, 2> shape(const DataType& data) { return {data.rows(), data.cols()}; }

        static void reshape(DataType& data, const std::array<Eigen::Index, 2> shape)
        {
            data.resize(shape[0], shape[1]);
        }

        static TArrayView<const T> getData(const DataType& mat) { return {mat.data(), ROWS * COLS}; }

        static TArrayView<T> getDataMutable(DataType& mat) { return {mat.data(), ROWS * COLS}; }

        REFLECT_STUB
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, shape, &this_t::shape)
            PROPERTY(data, &this_t::getData, &this_t::getDataMutable)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, size)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, colStride)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, rowStride)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, cols)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, rows)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return Indexer<NUM_FIELDS - 1>(); }
    };

    template <class T, int OPTS, int MAX_ROWS, int MAX_COLS>
    struct ReflectImpl<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, OPTS, MAX_ROWS, MAX_COLS>, void>
    {
        using DataType = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, OPTS, MAX_ROWS, MAX_COLS>;
        using this_t = ReflectImpl<DataType, void>;
        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }

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
            PROPERTY(shape, &this_t::shape, &this_t::reshape)
            PROPERTY(data, &this_t::getData, &this_t::getDataMutable)
            PROPERTY(size)
            PROPERTY(colStride)
            PROPERTY(rowStride)
            PROPERTY(cols)
            PROPERTY(rows)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return ct::Indexer<NUM_FIELDS - 1>(); }
    };

    template <typename T, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
    struct ReflectImpl<Eigen::Array<T, _Rows, _Cols, _Options, _MaxRows, _MaxCols>, void>
    {
        using DataType = Eigen::Array<T, _Rows, _Cols, _Options, _MaxRows, _MaxCols>;
        using this_t = ReflectImpl<DataType, void>;
        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }

        static TArrayView<const T> getData(const DataType& arr)
        {
            return {arr.data(), static_cast<size_t>(arr.rows() * arr.cols())};
        }

        static TArrayView<T> getDataMutable(DataType& arr)
        {
            return {arr.data(), static_cast<size_t>(arr.rows() * arr.cols())};
        }

        REFLECT_STUB
            PROPERTY(data, &this_t::getData, &this_t::getDataMutable)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return Indexer<NUM_FIELDS - 1>(); }
    };

    DECL_NAME(Eigen::MatrixXf);
    DECL_NAME(Eigen::Matrix2f);
    DECL_NAME(Eigen::Matrix3f);
    DECL_NAME(Eigen::Matrix4f);

    DECL_NAME(Eigen::MatrixXd);
    DECL_NAME(Eigen::Matrix2d);
    DECL_NAME(Eigen::Matrix3d);
    DECL_NAME(Eigen::Matrix4d);

} // namespace ct

#endif // CT_EIGEN_HPP
