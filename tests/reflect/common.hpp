#pragma once
#include "Data.hpp"
#include "Reflect.hpp"

template<class Tester>
void testTypes(Tester& tester)
{
    //tester.test<ReflectedStruct>();
    //tester.test<Inherited>();
    //tester.test<Composite>();
    tester.test<TestA>();
    tester.test<TestB>();
    tester.test<TestC>();
    //tester.test<TestVec>();
    //tester.test<InternallyReflected>();
}