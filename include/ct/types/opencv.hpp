#ifndef CT_TYPES_OPENCV_HPP
#define CT_TYPES_OPENCV_HPP
#include <opencv2/core.hpp>

#include "../StringView.hpp"
#include "../reflect.hpp"

namespace ct
{
    /*template <class VISITED>
    struct Reflect<cv::Mat, VISITED, void>
    {
        static constexpr int SPECIALIZED = true;
        using DataType = cv::Mat;
        static constexpr StringView getName() { return GetName<DataType>::getName(); }

        REFLECT_STUB
            PUBLIC_ACCESS(rows)
            PUBLIC_ACCESS(cols)
            MEMBER_FUNCTION(elemSize)
            MEMBER_FUNCTION(elemSize1)
            MEMBER_FUNCTION(type)
            MEMBER_FUNCTION(depth)
            MEMBER_FUNCTION(channels)
            MEMBER_FUNCTION(step1)
        REFLECT_INTERNAL_END;
        static constexpr auto end() { return ct::Indexer<NUM_FIELDS - 1>(); }
    };*/

    template <class T, int R, int C>
    struct Reflect<cv::Matx<T, R, C>>
    {
        static constexpr int SPECIALIZED = true;
        using DataType = cv::Matx<T, R, C>;
        static constexpr StringView getName() { return GetName<DataType>::getName(); }
        static constexpr int rows() { return R; }
        static constexpr int cols() { return C; }

        REFLECT_STUB
            STATIC_FUNCTION(eye, &DataType::eye)
            STATIC_FUNCTION(zeros, &DataType::zeros)
            STATIC_FUNCTION(ones, &DataType::ones)
            STATIC_FUNCTION(randu, &DataType::randu)
            STATIC_FUNCTION(randn, &DataType::randn)
            PUBLIC_ACCESS(val)
            MEMBER_FUNCTION(row)
            MEMBER_FUNCTION(col)
            STATIC_FUNCTION(rows, &Reflect<DataType>::rows)
            STATIC_FUNCTION(rows, &Reflect<DataType>::cols)
        REFLECT_INTERNAL_END;
        static constexpr auto end() { return ct::Indexer<NUM_FIELDS - 1>(); }
    };

    template <class T>
    struct Reflect<cv::Mat_<T>>
    {
        static constexpr int SPECIALIZED = true;
        using DataType = cv::Mat_<T>;
        static constexpr StringView getName() { return GetName<DataType>::getName(); }

        REFLECT_STUB
            PUBLIC_ACCESS(rows)
            PUBLIC_ACCESS(cols)
            MEMBER_FUNCTION(elemSize)
            MEMBER_FUNCTION(elemSize1)
            MEMBER_FUNCTION(type)
            MEMBER_FUNCTION(depth)
            MEMBER_FUNCTION(channels)
            MEMBER_FUNCTION(step1)
        REFLECT_INTERNAL_END;
        static constexpr auto end() { return ct::Indexer<NUM_FIELDS - 1>(); }
    };

    template <class T, int R>
    struct Reflect<cv::Vec<T, R>> : public Reflect<cv::Matx<T, R, 1>>
    {
        static constexpr int SPECIALIZED = true;
        using DataType = cv::Vec<T, R>;
        static constexpr StringView getName() { return GetName<DataType>::getName(); }
    };

    template <class T>
    struct Reflect<cv::Scalar_<T>> : public Reflect<cv::Vec<T, 4>>
    {
    };

    REFLECT_TEMPLATED_START(cv::Rect_)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(width)
        PUBLIC_ACCESS(height)
        MEMBER_FUNCTION(size)
        MEMBER_FUNCTION(topLeft, &DataType::tl)
        MEMBER_FUNCTION(bottomRight, &DataType::br)
        MEMBER_FUNCTION(area)
        MEMBER_FUNCTION(empty)
    REFLECT_END;

    REFLECT_TEMPLATED_START(cv::Point_)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
    REFLECT_END;

    REFLECT_TEMPLATED_START(cv::Point3_)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
    REFLECT_END;

    REFLECT_TEMPLATED_START(cv::Size_)
        PUBLIC_ACCESS(width)
        PUBLIC_ACCESS(height)
    REFLECT_END;
}

#endif // CT_TYPES_HPP
