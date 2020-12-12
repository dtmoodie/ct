#ifndef CT_TYPES_OPENCV_HPP
#define CT_TYPES_OPENCV_HPP

#include "../StringView.hpp"
#include "../reflect.hpp"
#include "../typename.hpp"
#include "TArrayView.hpp"

#include <opencv2/core.hpp>

namespace ct
{
    template <>
    struct ReflectImpl<cv::Mat, void>
    {
        using DataType = cv::Mat;
        static constexpr StringView getTypeName() { return "cv::Mat"; }

        REFLECT_STUB
            PUBLIC_ACCESS(rows)
            PUBLIC_ACCESS(cols)
            MEMBER_FUNCTION(elemSize)
            MEMBER_FUNCTION(elemSize1)
            MEMBER_FUNCTION(type)
            MEMBER_FUNCTION(depth)
            MEMBER_FUNCTION(channels)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return ct::Indexer<NUM_FIELDS - 1>(); }
    };

    template <class T, int R, int C>
    struct ReflectImpl<cv::Matx<T, R, C>, void>
    {
        using DataType = cv::Matx<T, R, C>;
        using this_t = ReflectImpl<DataType, void>;
        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }
        static constexpr int rows() { return R; }
        static constexpr int cols() { return C; }

        static TArrayView<T> getDataMutable(DataType& mat) { return {mat.val, R * C}; }
        static TArrayView<const T> getData(const DataType& mat) { return {mat.val, R * C}; }
        static std::array<int, 2> getShape(const DataType&) { return {R, C}; }
        static size_t getSize(const DataType&) { return C * R; }

        REFLECT_STUB
            PROPERTY(data, &this_t::getData, &this_t::getDataMutable)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, shape, &this_t::getShape, nullptr)
            PROPERTY_WITH_FLAG(Flags::COMPILE_TIME_CONSTANT, size, &this_t::getSize, nullptr)
            STATIC_FUNCTION(eye, &DataType::eye)
            STATIC_FUNCTION(zeros, &DataType::zeros)
            STATIC_FUNCTION(ones, &DataType::ones)
            STATIC_FUNCTION(randu, &DataType::randu)
            STATIC_FUNCTION(randn, &DataType::randn)
            MEMBER_FUNCTION(row)
            MEMBER_FUNCTION(col)
            STATIC_FUNCTION(rows, &this_t::rows)
            STATIC_FUNCTION(cols, &this_t::cols)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return ct::Indexer<NUM_FIELDS - 1>(); }
    };

    template <class T>
    struct ReflectImpl<cv::Mat_<T>, void>
    {
        using DataType = cv::Mat_<T>;
        using this_t = ReflectImpl<DataType, void>;
        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }
        static std::array<int, 2> getShape(const DataType& data) { return {data.rows, data.cols}; }

        static void reshape(DataType& data, const std::array<int, 2>& new_shape)
        {
            data.create(new_shape[0], new_shape[1]);
        }
        static TArrayView<const T> getData(const DataType& mat)
        {
            return {mat[0], static_cast<size_t>(mat.rows * mat.cols)};
        }

        static size_t getSize(const DataType& data) { return data.rows * data.cols; }

        static TArrayView<T> getDataMutable(DataType& mat)
        {
            return {mat[0], static_cast<size_t>(mat.rows * mat.cols)};
        }

        REFLECT_STUB
            PROPERTY(data, &this_t::getData, &this_t::getDataMutable)
            PROPERTY(shape, &this_t::getShape, &this_t::reshape)
            PROPERTY(size, &this_t::getSize)
            MEMBER_FUNCTION(elemSize)
            MEMBER_FUNCTION(elemSize1)
            MEMBER_FUNCTION(type)
            MEMBER_FUNCTION(depth)
            MEMBER_FUNCTION(channels)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return Indexer<NUM_FIELDS - 1>(); }
    };

    template <class T, int R>
    struct ReflectImpl<cv::Vec<T, R>, void> : public ReflectImpl<cv::Matx<T, R, 1>, void>
    {
        using DataType = cv::Vec<T, R>;
        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }
    };

    template <class T>
    struct ReflectImpl<cv::Scalar_<T>, void> : public ReflectImpl<cv::Vec<T, 4>, void>
    {
    };

    template <class T>
    cv::Rect_<T> intersection(const cv::Rect_<T>& r0, const cv::Rect_<T>& r1)
    {
        return r0 & r1;
    }

    template <class T>
    cv::Rect_<T> rectUnion(const cv::Rect_<T>& r0, const cv::Rect_<T>& r1)
    {
        return r0 | r1;
    }

    REFLECT_TEMPLATED_BEGIN(cv::Rect_)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(width)
        PUBLIC_ACCESS(height)
        MEMBER_FUNCTION(size)
        MEMBER_FUNCTION(topLeft, &DataType::tl)
        MEMBER_FUNCTION(bottomRight, &DataType::br)
        MEMBER_FUNCTION(area)
        MEMBER_FUNCTION(contains)
        MEMBER_FUNCTION(intersection, &intersection<Args...>)
        MEMBER_FUNCTION(union, &rectUnion<Args...>)
#if ((CV_VERSION_MAJOR == 3 && CV_VERSION_MINOR > 2) || CV_VERSION_MAJOR > 3)
        MEMBER_FUNCTION(empty)
#endif
    REFLECT_END;

    REFLECT_TEMPLATED_BEGIN(cv::Point_)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        MEMBER_FUNCTION(dot)
        MEMBER_FUNCTION(ddot)
        MEMBER_FUNCTION(cross)
        MEMBER_FUNCTION(inside)
    REFLECT_END;

    REFLECT_TEMPLATED_BEGIN(cv::Point3_)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
        MEMBER_FUNCTION(dot)
        MEMBER_FUNCTION(ddot)
        MEMBER_FUNCTION(cross)
    REFLECT_END;

    REFLECT_TEMPLATED_BEGIN(cv::Size_)
        PUBLIC_ACCESS(width)
        PUBLIC_ACCESS(height)
        MEMBER_FUNCTION(area)
#if ((CV_VERSION_MAJOR == 3 && CV_VERSION_MINOR > 2) || CV_VERSION_MAJOR > 3)
        MEMBER_FUNCTION(empty)
#endif
    REFLECT_END;

    DECL_NAME(cv::Point2f);
    DECL_NAME(cv::Vec3b);
    DECL_NAME(cv::Point);
    DECL_NAME(cv::Point3f);
    DECL_NAME(cv::Rect);
    DECL_NAME(cv::Rect2f);
    DECL_NAME(cv::Size);
    DECL_NAME(cv::Scalar);
    // It makes no sense why the compiler wants cv::Vec<double, 4> instead of cv::Scalar_<double>.....
    using CvScalarVec = cv::Vec<double, 4>;
    DECL_NAME(CvScalarVec, cv::Scalar);
} // namespace ct
#endif // CT_TYPES_OPENCV_HPP