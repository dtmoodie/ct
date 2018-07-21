#pragma once
#include "Data.hpp"
#include "Reflect.hpp"

template<class Tester>
void testTypes(Tester& tester)
{
    //tester.test<ReflectedStruct>();
    //tester.test<Inherited>();
    //tester.test<Composite>();
    tester.template test<TestA>();
    tester.template test<TestB>();
    tester.template test<TestC>();
    //tester.test<TestVec>();
    //tester.test<InternallyReflected>();
}