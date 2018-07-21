#include "common.hpp"
#include "Reflect.hpp"
#include <ct/reflect/print.hpp>
#include <iostream>

struct Printer
{
    template<class T>
    void test()
    {
        T data;
        std::cout << "====================\n";
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