#include "reflect.hpp"
#include "object.hpp"
#include <iostream>

REFLECT_BEGIN(Foo)
    PUBLIC_ACCESS(x)
    PUBLIC_ACCESS(y)
    PUBLIC_ACCESS(z)
REFLECT_END;

REFLECT_BEGIN(Bar)
    ACCESSOR(data, &Bar::get, &Bar::set)
    MEMBER_FUNCTION(half, &Bar::half)
REFLECT_END;

template<int I, class T>
void printValue(const T& obj)
{
    auto accessor = Reflect<T>::getAccessor(ct::_counter_<I>{});
    std::cout << accessor.get(obj) << std::endl;
}

int main()
{
    Foo obj;
    obj.x = 0;
    obj.y = 1;
    obj.z = 2;
    printValue<0>(obj);
    printValue<1>(obj);
    printValue<2>(obj);
    std::cout << std::endl;
    Bar bar;
    bar.set(4);
    printValue<0>(bar);
    printValue<1>(bar);
}
