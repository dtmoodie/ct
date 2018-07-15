#include "Reflect.hpp"
#include <ct/reflect/cereal.hpp>
#include <cereal/types/vector.hpp>

#ifdef HAVE_CEREAL
#include <cereal/archives/json.hpp>
#endif

namespace{
template<class T>
void printType()
{
    T data;
    {
        cereal::JSONOutputArchive ar(std::cout);
        ar(data);
    }

    std::cout << std::endl;
}
}


void testSerialization()
{

    static_assert(std::is_same<ct::reflect::enable_if_reflected<ReflectedStruct>, void>::value, "test1");
    printType<ReflectedStruct>();
    printType<Inherited>();
    printType<Composite>();
    printType<TestA>();
    printType<TestB>();
    printType<TestC>();
    printType<TestVec>();
}
