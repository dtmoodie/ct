#include "Data.hpp"
#include "Reflect.hpp"
#include <ct/reflect.hpp>

#include <gtest/gtest.h>

#include <iostream>

template <class T>
void checkName(ct::StringView expected_name)
{
    if (ct::Reflect<T>::getName() != expected_name)
    {
        std::cout << "funcname: " << ct::GetName<T>::funcName() << std::endl;
    }
    EXPECT_EQ(ct::Reflect<T>::getName(), expected_name);
}

#define CHECK_NAME(NAME)                                                                                               \
    TEST(check_name, NAME) { checkName<NAME>(#NAME); }

CHECK_NAME(float);
CHECK_NAME(double);
CHECK_NAME(uint32_t);
CHECK_NAME(int32_t);
CHECK_NAME(int8_t);
CHECK_NAME(uint8_t);
CHECK_NAME(uint16_t);
CHECK_NAME(int16_t);
CHECK_NAME(InternallyReflected);
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

TEST(check_name, string)
{
    checkName<std::string>("std::string");
}

TEST(check_name, vector)
{
    checkName<std::vector<float>>("std::vector<float>");
}

TEST(check_name, vector_string)
{
    checkName<std::vector<std::string>>("std::vector<std::string>");
}

TEST(check_name, Templated)
{
    checkName<Templated<double>>("Templated<double>");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
