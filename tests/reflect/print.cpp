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
struct CheckPrint;

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

EXPECTED_OBJECT_PRINT(ReflectedStruct, "(x: 0 y:1 z: 2 id: 3");
EXPECTED_OBJECT_PRINT(Inherited, "(x: 0 y:1 z: 2 w: 4");
EXPECTED_OBJECT_PRINT(Composite, "(a: (x: 0 y:1 z: 2 id: 3) b: (x:4 y:5 z:6 id:7 ) )");
EXPECTED_OBJECT_PRINT(TestA, "(x: 0 y:1 z: 2 norm: 2.23607 )");
EXPECTED_OBJECT_PRINT(TestB, "(x: 0 y:1 z: 2");
EXPECTED_OBJECT_PRINT(TestC, "(x: 0 y:1 z: 2");
EXPECTED_OBJECT_PRINT(TestVec, "(vec: [0 1 2 3 4] )");
EXPECTED_OBJECT_PRINT(PrivateMutableAccess, "(private_mutable_property: 4 )");
EXPECTED_OBJECT_PRINT(InternallyReflected, "(x: 5 y: 10 z: 15 )");
EXPECTED_OBJECT_PRINT(PrivateGetAndSet, "(private_get_and_set: 5.2 )");
EXPECTED_OBJECT_PRINT(PrivateGetAndSet, "(private_get_and_set: 5.2 )");
EXPECTED_OBJECT_PRINT(PrivateGetAndSet, "(private_get_and_set: 5.2 )");
EXPECTED_OBJECT_PRINT(PrivateGetAndSet, "(private_get_and_set: 5.2 )");
EXPECTED_OBJECT_PRINT(PrivateGetAndSet, "(private_get_and_set: 5.2 )");
EXPECTED_OBJECT_PRINT(PrivateGetAndSet, "(private_get_and_set: 5.2 )");
EXPECTED_OBJECT_PRINT(PrivateGetAndSet, "(private_get_and_set: 5.2 )");

template <class T>
struct StaticReflectPrinter : ::testing::Test
{
    void test()
    {
        T data = TestData<T>::init();
        StaticPrintStruct str;
        str.test(data);
        std::cout << std::endl;
    }
};

template <class T>
struct ReflectPrinter : ::testing::Test
{
    void test()
    {
        T data = TestData<T>::init();
        std::cout << "\n====================\n";
        std::cout << ct::Reflect<T>::getName() << std::endl;
        ct::printStruct<ct::PrintAllOptions>(std::cout, data);
        std::cout << std::endl;
    }
};

TYPED_TEST_SUITE_P(ReflectPrinter);
TYPED_TEST_SUITE_P(StaticReflectPrinter);

TYPED_TEST_P(ReflectPrinter, object_print)
{
    this->test();
}

TYPED_TEST_P(StaticReflectPrinter, struct_print)
{
    this->test();
}

REGISTER_TYPED_TEST_SUITE_P(ReflectPrinter, object_print);

REGISTER_TYPED_TEST_SUITE_P(StaticReflectPrinter, struct_print);

INSTANTIATE_TYPED_TEST_SUITE_P(ReflectPrinterTest, ReflectPrinter, TestTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(ReflectStaticPrinterTest, StaticReflectPrinter, TestTypes);

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    /*Printer printer;
    testTypes(printer);

    std::cout << std::endl;
    ct::Reflect<DerivedC>::printHierarchy(std::cout);
    std::cout << ct::GetNameGCC<DerivedC>::funcName() << std::endl;*/
}
