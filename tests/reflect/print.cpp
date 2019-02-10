#include "Reflect.hpp"
#include "common.hpp"
#include <ct/reflect/print-container-inl.hpp>
#include <ct/reflect/print.hpp>
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

struct Printer
{
    template <class T>
    void test(T& data)
    {
        std::cout << "\n====================\n";
        std::cout << ct::Reflect<T>::getName() << std::endl;
        ct::printStruct<ct::PrintAllOptions>(std::cout, data);
        std::cout << std::endl;
        StaticPrintStruct str;
        str.test(data);
        // printStructInfo<T>(std::cout);
    }
};

int main()
{
    Printer printer;
    testTypes(printer);

    std::cout << std::endl;
    ct::Reflect<DerivedC>::printHierarchy(std::cout);
}
