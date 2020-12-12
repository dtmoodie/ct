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
EXPECTED_OBJECT_PRINT(MultipleInheritance, "(private_mutable_property: 4 x: 0 y: 1 z: 2 id: 3 asdf: 5 ) ");
EXPECTED_OBJECT_PRINT(Inherited, "(x: 0 y: 1 z: 2 id: 3 w: 4 ) ");
EXPECTED_OBJECT_PRINT(Composite, "(a: (x: 0 y: 1 z: 2 id: 3 )  b: (x: 4 y: 5 z: 6 id: 7 )  ) ");
EXPECTED_OBJECT_PRINT(TestA, "(x: 0 y: 1 z: 2 ) ");
EXPECTED_OBJECT_PRINT(TestB, "(x: 0 y: 1 z: 2 ) ");
EXPECTED_OBJECT_PRINT(TestC, "(y: 0 x: 1 z: 2 ) ");
EXPECTED_OBJECT_PRINT(DerivedA, "(base_x: 0 base_y: 1 base_z: 2 derived_a: 3 ) ");
EXPECTED_OBJECT_PRINT(DerivedB, "(base_x: 0 base_y: 1 base_z: 2 derived_b: 4 ) ");
EXPECTED_OBJECT_PRINT(DerivedC, "(derived_b: 4 base_x: 0 base_y: 1 base_z: 2 derived_a: 3 derived_c: 5 ) ");
EXPECTED_OBJECT_PRINT(TestVec, "(vec: [0 1 2 3 4] ) ");
EXPECTED_OBJECT_PRINT(ExplicitThisProperty, "(prop: 0 ) ");
EXPECTED_OBJECT_PRINT(Virtual, "() ");
EXPECTED_OBJECT_PRINT(PrivateMutableAccess, "(private_mutable_property: 4 ) ");
EXPECTED_OBJECT_PRINT(InternallyReflected, "(x: 5 y: 10 z: 15 ) ");
EXPECTED_OBJECT_PRINT(PrivateGetAndSet, "(private_get_and_set: 5.2 ) ");
EXPECTED_OBJECT_PRINT(std::vector<ReflectedStruct>,
                      "[(x: 0 y: 1 z: 2 id: 3 )  (x: 4 y: 5 z: 6 id: 7 )  (x: 8 y: 9 z: 10 id: 11 ) ]");
EXPECTED_OBJECT_PRINT(StringMap, "{asdf:(x: 0 y: 1 z: 2 id: -1 w: 15 )  asdfg:(x: 0 y: 1 z: 4 id: -1 w: 15 ) }");

EXPECTED_OBJECT_PRINT(ct::ext::DataTableStorage<float>,
                      "((size: 1 data: [10] ) size: 10 data: [0 1 2 3 4 5 6 7 8 9] ) ");
EXPECTED_OBJECT_PRINT(
    ct::ext::DataTableStorage<ct::TArrayView<float>>,
    "(shape: [10 10] size: 100 data: [0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 "
    "30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 "
    "68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99] ) ");

#ifdef HAVE_OPENCV
EXPECTED_OBJECT_PRINT(cv::Point2f, "(x: 0 y: 1 ) ");
EXPECTED_OBJECT_PRINT(cv::Point, "(x: 0 y: 1 ) ");
EXPECTED_OBJECT_PRINT(cv::Point3f, "(x: 0 y: 1 z: 2 ) ");
EXPECTED_OBJECT_PRINT(cv::Vec2f, "(data: [2 3] shape: [2 1] size: 2 ) ");
EXPECTED_OBJECT_PRINT(cv::Scalar, "(data: [0 1 2 3] shape: [4 1] size: 4 ) ");
EXPECTED_OBJECT_PRINT(cv::Rect2f, "(x: 0 y: 1 width: 2 height: 3 ) ");
EXPECTED_OBJECT_PRINT(cv::Rect, "(x: 0 y: 1 width: 2 height: 3 ) ");
EXPECTED_OBJECT_PRINT(cv::Mat_<float>, "(data: [1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1] shape: [4 4] size: 16 ) ");
EXPECTED_OBJECT_PRINT(cv::Mat_<cv::Vec3f>,
                      "(data: [[3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] "
                      "[3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] [3.14159, "
                      "0, 0] [3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0] "
                      "[3.14159, 0, 0] [3.14159, 0, 0] [3.14159, 0, 0]] shape: [4 4] size: 16 ) ");
EXPECTED_OBJECT_PRINT(cv::Mat, "(rows: 4 cols: 4 ) ");

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
        std::cout << ct::Reflect<T>::getTypeName() << std::endl;
        std::stringstream ss;
        ct::printStruct<ct::PrintOptions>(ss, data);
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
