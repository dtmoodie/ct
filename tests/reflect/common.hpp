#pragma once
#include "Data.hpp"
#include "Reflect.hpp"
#include <ostream>
#include <vector>
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


template<class Tester>
void testTypes(Tester& tester)
{
    {
        ReflectedStruct data{ 0,1,2,3 };
        tester.test(data);
    }
    {
        Inherited data;
        data.id = 0;
        data.w = 0;
        data.x = 0;
        data.y = 1;
        data.z = 2;
        tester.test(data);
    }
    {
        Composite data{ {0,1,2,3}, {4,5,6,7} };
        tester.test(data);
    }
    {
        TestA data{ 0,1,2 };
        tester.test(data);
    }
    {
        TestB data{ 0,1,2 };
        tester.test(data);
    }
    {
        TestC data{ 0,1,2 };
        tester.test(data);
    }
    {
        TestVec data;
        data.vec = { 0,1,2,3,4 };
        tester.test(data);
    }
    {
        PrivateMutableAccess pma;
        pma.mutateX() = 4;
        tester.test(pma);
    }
    {
        PrivateGetAndSet pgs;
        pgs.setX(5.2);
        tester.test(pgs);
    }
}
