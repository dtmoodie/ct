#include "Reflect.hpp"
#include <ct/reflect/printer.hpp>

#include <iostream>

namespace {
template<class T>
void printType(const T& data = T())
{
    ct::reflect::printStruct(std::cout, data);
    std::cout << std::endl;
}

}

void testAccess()
{
    printType<ReflectedStruct>();
    printType<Inherited>();
    printType<Composite>();
    printType<TestA>();
    printType<TestB>();
    printType<TestC>();
    printType<TestVec>();

}
