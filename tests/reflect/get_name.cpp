#include "Data.hpp"
#include "Reflect.hpp"
#include <ct/reflect.hpp>

#include <iostream>

template <class T>
bool checkName(ct::StringView expected_name)
{
    if (ct::Reflect<T>::getName() != expected_name)
    {
        std::cout << "ct::Reflect::getName() not working for " << expected_name << ", got: '"
                  << ct::Reflect<T>::getName() << "'" << std::endl;
        return false;
    }
    return true;
}

#define CHECK_NAME(NAME)                                                                                               \
    if (!checkName<NAME>(#NAME))                                                                                       \
    return 1

int main()
{
    CHECK_NAME(InternallyReflected);
    CHECK_NAME(Templated<double>);
    CHECK_NAME(TestA);
    CHECK_NAME(ReflectedStruct);
    CHECK_NAME(Inherited);
    CHECK_NAME(Base);
    CHECK_NAME(DerivedA);
    CHECK_NAME(DerivedB);
    CHECK_NAME(DerivedC);
    CHECK_NAME(Wrapper);
    CHECK_NAME(TestB);
    CHECK_NAME(TestVec);
    CHECK_NAME(PrivateMutableAccess);
    CHECK_NAME(PrivateGetAndSet);
    CHECK_NAME(MultipleInheritance);
    CHECK_NAME(PointerOwner);
    CHECK_NAME(Virtual);
    CHECK_NAME(ExplicitThisProperty);
    CHECK_NAME(WeirdWeakOwnerShip);

    std::cout << "No errors detected" << std::endl;
    return 0;
}
