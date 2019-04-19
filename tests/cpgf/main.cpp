#include <ct/interop/cpgf.hpp>
#include "../reflect/common.hpp"
#include "cpgf/goutmain.h"
#ifdef HAVE_OPENCV
#include <ct/types/opencv.hpp>
#endif

#ifdef HAVE_EIGEN3
#include <Eigen/Core>
#endif

namespace ct
{
    template <class T, int ROWS, int COLS, int OPTS, int MAXROWS, int MAXCOLS>
    struct Reflect<Eigen::Matrix<T, ROWS, COLS, OPTS, MAXROWS, MAXCOLS>>
    {
        using DataType = Eigen::Matrix<T, ROWS, COLS, OPTS, MAXROWS, MAXCOLS>;
        using Base = VariadicTypedef<void>;

        static constexpr int SPECIALIZED = true;
        static constexpr index_t I0 = 0;
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;

        static constexpr const char* getName(){return "Eigen::Matrix<T, ROWS, COLS, OPTS, MAXROWS, MAXCOLS>";}

        MEMBER_FUNCTION(data,
                        static_cast<const T*(DataType::*)()const>(&DataType::data),
                        static_cast<T*(DataType::*)()>(&DataType::data))
        MEMBER_FUNCTION(rows)
        MEMBER_FUNCTION(cols)
    REFLECT_END;

}

G_AUTO_RUN_BEFORE_MAIN()
{
    using namespace cpgf;
    ct::registerReflectedClass<ReflectedStruct>();
    ct::registerReflectedClass<Inherited>();

    ct::registerReflectedClass<Composite>();
    ct::registerReflectedClass<TestA>();
    ct::registerReflectedClass<TestB>();
    ct::registerReflectedClass<TestC>();
    ct::registerReflectedClass<TestVec>();

#ifdef HAVE_OPENCV
    ct::registerReflectedClass<cv::Rect2f>("Rect2f");
    ct::registerReflectedClass<cv::Rect2i>("Rect2i");
#endif
#ifdef HAVE_EIGEN3
    ct::registerReflectedClass<Eigen::Matrix2f>("Eigen::Matrix2f");
#endif

}

template<class T>
void test()
{
    using namespace cpgf;
    auto type = createMetaType<T>();
    auto cls = findMetaClass(type);
    if(cls)
    {
        std::cout << cls << std::endl;
    }
}

int main()
{
    test<ReflectedStruct>();
    test<Inherited>();
    test<Composite>();
    test<TestA>();
    test<TestB>();
    test<TestC>();
    test<TestVec>();

#ifdef HAVE_OPENCV
    test<cv::Rect2f>();
    test<cv::Rect2i>();
#endif
#ifdef HAVE_EIGEN3
    test<Eigen::Matrix2f>();
#endif
}
