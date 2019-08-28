#include "Reflect.hpp"
#include "common.hpp"

#include <ct/reflect/print-container-inl.hpp>
#include <ct/reflect/print.hpp>
#include <ct/reflect/visitor.hpp>

#include <iostream>

struct StaticPrintStruct
{
    template <class T>
    ct::EnableIfReflected<T> test(const T&)
    {
        ct::printStructInfo<T>(std::cout);
    }

    template <class T>
    ct::DisableIfReflected<T> test(const T&)
    {
    }
};

template <class T>
struct CheckPrint
{
    static void check(std::string val)
    {
        std::string unimplemented;
        EXPECT_EQ(unimplemented, val);
    }
};

#define EXPECTED_OBJECT_PRINT(TYPE, ...)                                                                               \
    template <>                                                                                                        \
    struct CheckPrint<TYPE>                                                                                            \
    {                                                                                                                  \
        static void check(std::string val)                                                                             \
        {                                                                                                              \
            std::string str = __VA_ARGS__;                                                                             \
            EXPECT_EQ(str, val);                                                                                       \
        }                                                                                                              \
    }

EXPECTED_OBJECT_PRINT(ReflectedStruct, "(x: 0 y: 1 z: 2 id: 3 ) ");
EXPECTED_OBJECT_PRINT(Inherited, "(x: 0 y: 1 z: 2 id: 3 w: 4 ) ");
EXPECTED_OBJECT_PRINT(Composite, "(a: (x: 0 y: 1 z: 2 id: 3 )  b: (x: 4 y: 5 z: 6 id: 7 )  ) ");
EXPECTED_OBJECT_PRINT(TestA, "(x: 0 y: 1 z: 2 norm: 2.23607 ) ");
EXPECTED_OBJECT_PRINT(TestB, "(x: 0 y: 1 z: 2 ) ");
EXPECTED_OBJECT_PRINT(TestC, "(y: 0 x: 1 z: 2 ) ");
EXPECTED_OBJECT_PRINT(TestVec, "(vec: [0 1 2 3 4] ) ");
EXPECTED_OBJECT_PRINT(PrivateMutableAccess, "(private_mutable_property: 4 ) ");
EXPECTED_OBJECT_PRINT(InternallyReflected, "(x: 5 y: 10 z: 15 ) ");
EXPECTED_OBJECT_PRINT(PrivateGetAndSet, "(private_get_and_set: 5.2 ) ");
EXPECTED_OBJECT_PRINT(std::vector<ReflectedStruct>,
                      "[(x: 0 y: 1 z: 2 id: 3 )  (x: 4 y: 5 z: 6 id: 7 )  (x: 8 y: 9 z: 10 id: 11 ) ]");
EXPECTED_OBJECT_PRINT(StringMap, "{asdf:(x: 0 y: 1 z: 2 id: -1 w: 15 )  asdfg:(x: 0 y: 1 z: 4 id: -1 w: 15 ) }");

#ifdef HAVE_OPENCV
EXPECTED_OBJECT_PRINT(cv::Point2f, "(x: 0 y: 1 ) ");
EXPECTED_OBJECT_PRINT(cv::Point, "(x: 0 y: 1 ) ");
EXPECTED_OBJECT_PRINT(cv::Point3f, "(x: 0 y: 1 z: 2 ) ");
EXPECTED_OBJECT_PRINT(cv::Vec2f, "(data: [2 3] shape: [2 1] size: 2 ) ");
EXPECTED_OBJECT_PRINT(cv::Scalar, "(data: [0 1 2 3] shape: [4 1] size: 4 ) ");
EXPECTED_OBJECT_PRINT(cv::Mat_<float>,
                      "(data: [1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1] shape: [4 4] size: 16 elemSize: 4 "
                      "elemSize1: 4 type: 5 depth: 5 channels: 1 ) ");
EXPECTED_OBJECT_PRINT(cv::Mat_<cv::Vec3f>,
                      "(data: [[3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] "
                      "[3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] [3.14159, "
                      "0, 0] [3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] "
                      "[3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0]] shape: [4 4] size: 16 "
                      "elemSize: 12 elemSize1: 4 type: 21 depth: 5 channels: 3 ) ");

#endif

#ifdef HAVE_EIGEN
EXPECTED_OBJECT_PRINT(Eigen::Matrix3f,
                      "(data: [1 0 0 0 1 0 0 0 1] shape: [3 3] size: 9 colStride: 3 rowStride: 1 cols: 3 rows: 3 ) ");
EXPECTED_OBJECT_PRINT(Eigen::MatrixXf,
                      "(data: [1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 1] shape: [5 5] size: "
                      "25 colStride: 5 rowStride: 1 cols: 5 rows: 5 ) ");
#endif

template <class T>
struct ReflectPrinter : ::testing::Test
{
    void staticPrint()
    {
        T data = TestData<T>::init();
        StaticPrintStruct str;
        str.test(data);
        std::cout << std::endl;
    }

    void objectPrint()
    {
        T data = TestData<T>::init();
        std::cout << "\n====================\n";
        std::cout << ct::Reflect<T>::getName() << std::endl;
        std::stringstream ss;
        ct::printStruct<ct::PrintAllOptions>(ss, data);
        CheckPrint<T>::check(ss.str());
    }
};

TYPED_TEST_SUITE_P(ReflectPrinter);

TYPED_TEST_P(ReflectPrinter, object_test)
{
    this->objectPrint();
}

TYPED_TEST_P(ReflectPrinter, struct_test)
{
    this->staticPrint();
}

REGISTER_TYPED_TEST_SUITE_P(ReflectPrinter, object_test, struct_test);

INSTANTIATE_TYPED_TEST_SUITE_P(print, ReflectPrinter, ToTestTypes<TestTypes>::type);

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
