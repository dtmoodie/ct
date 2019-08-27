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

int main(int argc, char** argv)
{
    TestTrue true_tester;
    testTypes(true_tester);
    std::cout << "\n\n The following are are intentionally != values\n";
    TestFalse false_tester;
    testTypes(false_tester);
}
