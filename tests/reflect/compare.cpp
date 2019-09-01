#include "common.hpp"

#include <ct/reflect.hpp>
#include <ct/reflect/compare.hpp>
#include <ct/reflect/print.hpp>

#include <iostream>

struct TestTrue
{
    template <class T>
    void test(const T& data)
    {
        T test_data = data;
        ct::compare(data, test_data, DebugEqual());
    }
};

struct TestFalse
{
    template <class T>
    void test(const T& data)
    {
        T test_data;
        ct::compare(data, test_data, DebugEqual());
    }
};

template <class T>
struct Comparator : ::testing::Test
{
    void testTrue()
    {
        T data = TestData<T>::init();
        T test_data = data;
        ct::compare(data, test_data, DebugEqual());
    }

    void testFalse()
    {
        T data = TestData<T>::init();
        T test_data;
        ct::compare(data, test_data, DebugEqual());
    }
};

TYPED_TEST_SUITE_P(Comparator);

TYPED_TEST_P(Comparator, test_true)
{
    this->testTrue();
}

TYPED_TEST_P(Comparator, test_false)
{
    this->testTrue();
}

REGISTER_TYPED_TEST_SUITE_P(Comparator, test_true, test_false);

INSTANTIATE_TYPED_TEST_SUITE_P(comparison, Comparator, ToTestTypes<TestTypes>::type);

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
