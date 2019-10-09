#include "Data.hpp"
#include "Reflect.hpp"
#include "common.hpp"

#include <ct/reflect.hpp>

#include <gtest/gtest.h>

#include <iostream>

template <class T>
struct ExpectedName;

#define EXPECTED_NAME(...)                                                                                             \
    template <>                                                                                                        \
    struct ExpectedName<__VA_ARGS__>                                                                                   \
    {                                                                                                                  \
        static constexpr const ct::StringView getName() { return #__VA_ARGS__; }                                       \
    }

EXPECTED_NAME(float);
EXPECTED_NAME(double);
EXPECTED_NAME(uint32_t);
EXPECTED_NAME(int32_t);
EXPECTED_NAME(int8_t);
EXPECTED_NAME(uint8_t);
EXPECTED_NAME(uint16_t);
EXPECTED_NAME(int16_t);
EXPECTED_NAME(InternallyReflected);
EXPECTED_NAME(TestA);
EXPECTED_NAME(TestB);
EXPECTED_NAME(TestC);
EXPECTED_NAME(ReflectedStruct);
EXPECTED_NAME(Inherited);
EXPECTED_NAME(Composite);
EXPECTED_NAME(Base);
EXPECTED_NAME(DerivedA);
EXPECTED_NAME(DerivedB);
EXPECTED_NAME(DerivedC);
EXPECTED_NAME(Wrapper);
EXPECTED_NAME(TestVec);
EXPECTED_NAME(PrivateMutableAccess);
EXPECTED_NAME(PrivateGetAndSet);
EXPECTED_NAME(MultipleInheritance);
EXPECTED_NAME(PointerOwner);
EXPECTED_NAME(Virtual);
EXPECTED_NAME(ExplicitThisProperty);
EXPECTED_NAME(WeirdWeakOwnerShip);
EXPECTED_NAME(std::string);
EXPECTED_NAME(std::vector<float>);
EXPECTED_NAME(std::vector<ReflectedStruct>);
EXPECTED_NAME(std::vector<std::string>);
EXPECTED_NAME(std::vector<std::vector<std::string>>);
EXPECTED_NAME(Templated<double>);
EXPECTED_NAME(std::map<std::string, Inherited>);

#ifdef HAVE_EIGEN
EXPECTED_NAME(Eigen::Matrix3f);
EXPECTED_NAME(Eigen::MatrixXf);
#endif

#ifdef HAVE_OPENCV
using namespace cv;

EXPECTED_NAME(cv::Rect);
EXPECTED_NAME(cv::Rect2f);
EXPECTED_NAME(cv::Vec<float, 2>);
EXPECTED_NAME(cv::Point2f);
EXPECTED_NAME(cv::Point);
EXPECTED_NAME(cv::Point3f);
EXPECTED_NAME(cv::Scalar);
EXPECTED_NAME(cv::Mat);
EXPECTED_NAME(cv::Mat_<float>);
// clang-format off
EXPECTED_NAME(cv::Mat_<cv::Vec<float, 3> >);
// clang-format on
#endif

template <class T>
struct CheckName : ::testing::Test
{
    void testName()
    {
        auto expected_name = ExpectedName<T>::getName();
        if (ct::Reflect<T>::getName() != expected_name)
        {
            std::cout << "funcname: " << ct::GetName<T>::funcName() << std::endl;
        }
        EXPECT_EQ(ct::Reflect<T>::getName(), expected_name);
    }
};

TYPED_TEST_SUITE_P(CheckName);

TYPED_TEST_P(CheckName, test)
{
    this->testName();
}

REGISTER_TYPED_TEST_SUITE_P(CheckName, test);

using TestNameTypes = typename TestTypes::Append<ct::VariadicTypedef<float,
                                                                     double,
                                                                     uint32_t,
                                                                     int32_t,
                                                                     int8_t,
                                                                     uint8_t,
                                                                     uint16_t,
                                                                     int16_t,
                                                                     std::string,
                                                                     std::vector<std::string>,
                                                                     std::vector<std::vector<std::string>>,
                                                                     Templated<double>>>::type;

INSTANTIATE_TYPED_TEST_SUITE_P(name, CheckName, ToTestTypes<TestNameTypes>::type);

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
