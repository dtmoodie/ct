#ifndef CT_TYPES_HPP
#define CT_TYPES_HPP
#include <opencv2/core.hpp>
#include "../reflect.hpp"
#include "../String.hpp"

namespace ct
{
    template <class T, int R>
    struct Reflect<cv::Vec<T, R>>
    {
        static constexpr int N = 0;
        static constexpr int SPECIALIZED = true;

        static constexpr ct::Accessor<decltype(&cv::Vec<T, R>::val),decltype(&cv::Vec<T, R>::val)>
        getPtr(const ct::Indexer<0>)
        {
            return {&cv::Vec<T, R>::val,&cv::Vec<T, R>::val};
        }
        static constexpr const char* getName(Indexer<0>){return "data";}

        static auto getPtr(const ct::Indexer<1>)
            ->decltype(ct::makeAccessor<CalculatedValue>(&cv::Vec<T,R>::t))
        {
            return ct::makeAccessor<CalculatedValue>(&cv::Vec<T,R>::t);
        }
        \
        static constexpr const char* getName(const ct::Indexer<1>) { return "transpose"; }

        static constexpr const char* getName(){return "cv::Vec_<T, R>";}

        static constexpr ct::Indexer<N> end()
        {
            return ct::Indexer<N>{};
        }
    };

    template<class T>
    struct Reflect<cv::Scalar_<T>>: public Reflect<cv::Vec<T, 4>>
    {

    };

    REFLECT_TEMPLATED_START(cv::Rect_)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(width)
        PUBLIC_ACCESS(height)
        MEMBER_FUNCTION(size, &cv::Rect_<Args...>::size)
        MEMBER_FUNCTION(topLeft, &cv::Rect_<Args...>::tl)
        MEMBER_FUNCTION(bottomRight, &cv::Rect_<Args...>::br)
        MEMBER_FUNCTION(area, &cv::Rect_<Args...>::area)
        MEMBER_FUNCTION(empty, &cv::Rect_<Args...>::empty)
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
