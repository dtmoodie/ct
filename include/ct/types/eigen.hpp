#ifndef CT_EIGEN_HPP
#define CT_EIGEN_HPP
#include "../reflect.hpp"
#include <Eigen/Core>

namespace ct
{
    template <class T, int R>
    struct Reflect<cv::Vec<T, R>>
    {
        static constexpr int N = 0;
        static constexpr int SPECIALIZED = true;

        static constexpr ct::Accessor<decltype(&cv::Vec<T, R>::val),decltype(&cv::Vec<T, R>::val)>
        getAccessor(const ct::Indexer<0>)
        {
            return {&cv::Vec<T, R>::val,&cv::Vec<T, R>::val};
        }
        static constexpr const char* getName(Indexer<0>){return "data";}

        static auto getAccessor(const ct::Indexer<1>)
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
}

#endif // CT_EIGEN_HPP
