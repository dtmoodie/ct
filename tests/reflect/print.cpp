#include "Reflect.hpp"
#include "common.hpp"
#include <ct/reflect/print-container-inl.hpp>
#include <ct/reflect/print.hpp>
#include <iostream>

struct Printer
{
    template <class T>
    void test(T& data)
    {
        std::cout << "\n====================\n";
        std::cout << ct::Reflect<T>::getName() << std::endl;
        ct::printStruct(std::cout, data);
        std::cout << std::endl;
    }
};

int main()
{
    Printer printer;
    testTypes(printer);
}
