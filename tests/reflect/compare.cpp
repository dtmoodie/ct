
#include <ct/reflect.hpp>
#include <ct/reflect/compare.hpp>
#include <ct/reflect/print.hpp>
#include "common.hpp"
#include <iostream>

struct DebugEqual
{
    template<class T>
    bool test(const char* name, const T& lhs, const T& rhs) const
    {
        if(lhs != rhs)
        {
            std::cout << name << " values not equal: " << lhs << " != " << rhs << std::endl;
            return false;
        }
        return true;
    }
};

struct TestTrue
{
    template<class T>
    void test(const T& data)
    {
        T test_data = data;
        ct::compare(data, test_data, DebugEqual());
    }
};

struct TestFalse
{
    template<class T>
    void test(const T& data)
    {
        T test_data;
        ct::compare(data, test_data, DebugEqual());
    }
};

int main()
{
    TestTrue true_tester;
    testTypes(true_tester);
    std::cout << "\n\n The following are are intentionally != values\n";
    TestFalse false_tester;
    testTypes(false_tester);
}
