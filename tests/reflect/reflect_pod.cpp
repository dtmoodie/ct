#include "Data.hpp"
#include "Reflect.hpp"

#include <ct/String.hpp>
#include <ct/Hash.hpp>

#include <cereal/archives/json.hpp>
#include <iostream>

using namespace ct;


template<class T, int I>
static constexpr size_t getSize()
{
    return sizeof(typename std::decay<decltype(ct::reflect::get<I>(std::declval<T>()))>::type);
}

template<class T, int I>
static size_t getOffset()
{
    return reinterpret_cast<size_t>(&ct::reflect::get<I>(*static_cast<T*>(nullptr)));
}

template<class T, int I>
using getType = typename std::decay<decltype(ct::reflect::get<I>(std::declval<T>()))>::type;

void testSerialization();
void testHash();
void testAccess();

int main(int /*argc*/, char** /*argv*/)
{

    std::cout << "Offset: " << getOffset<ReflectedStruct, 1>() << std::endl;
    static_assert(ct::reflect::ReflectData<Inherited>::I0 == 4, "Fetching base param count is broken");
    static_assert(ct::reflect::ReflectData<Inherited>::N == 5, "Inheritance param counting is broken");

    std::cout << std::endl;
    



    static_assert(ct::strLen(ct::reflect::ReflectData<Inherited>::getName()) == 9, "ct::strLen(ct::reflect::ReflectData<Inherited>::getName()) == 9");

    static_assert(ct::detail::ctcrc32(ct::reflect::ReflectData<Inherited>::getName(ct::_counter_<0>())) != ct::detail::ctcrc32(ct::reflect::ReflectData<Inherited>::getName(ct::_counter_<1>())),
                  "ct::detail::ctcrc32(ct::reflect::ReflectData<Inherited>::getName(ct::_counter_<0>())) != ct::detail::ctcrc32(ct::reflect::ReflectData<Inherited>::getName(ct::_counter_<1>()))");


    std::cout << "TestA.x offset " << getOffset<TestA, 0>() << std::endl;
    std::cout << "TestA.y offset " << getOffset<TestA, 1>() << std::endl;
    std::cout << "TestA.z offset " << getOffset<TestA, 2>() << std::endl;

    {
        TestA val;
        val.x = 0;
        val.y = 1;
        val.z = 2;

        if (reinterpret_cast<unsigned char*>(&val.x) != (reinterpret_cast<unsigned char*>(&val) + getOffset<TestA, 0>()))
        {
            std::cout << "Offset incorrect" << std::endl;
            return 1;
        }

        if (reinterpret_cast<unsigned char*>(&val.y) != (reinterpret_cast<unsigned char*>(&val) + getOffset<TestA, 1>()))
        {
            std::cout << "Offset incorrect" << std::endl;
            return 1;
        }

        if (reinterpret_cast<unsigned char*>(&val.z) != (reinterpret_cast<unsigned char*>(&val) + getOffset<TestA, 2>()))
        {
            std::cout << "Offset incorrect" << std::endl;
            return 1;
        }

        static_assert(std::is_same<getType<TestA, 0>, float>::value, "std::is_same_v<getType<TestA, 0>, float>");
    }

    {
        Inherited val;
        val.x = 0;
        val.y = 1;
        val.z = 2;
        val.id = 10;
        val.w = 5;

        if (reinterpret_cast<unsigned char*>(&val.x) != (reinterpret_cast<unsigned char*>(&val) + getOffset<Inherited, 0>()))
        {
            std::cout << "Offset incorrect" << std::endl;
            return 1;
        }

        if (reinterpret_cast<unsigned char*>(&val.y) != (reinterpret_cast<unsigned char*>(&val) + getOffset<Inherited, 1>()))
        {
            std::cout << "Offset incorrect" << std::endl;
            return 1;
        }

        if (reinterpret_cast<unsigned char*>(&val.z) != (reinterpret_cast<unsigned char*>(&val) + getOffset<Inherited, 2>()))
        {
            std::cout << "Offset incorrect" << std::endl;
            return 1;
        }
        if (reinterpret_cast<unsigned char*>(&val.id) != (reinterpret_cast<unsigned char*>(&val) + getOffset<Inherited, 3>()))
        {
            std::cout << "Offset incorrect" << std::endl;
            return 1;
        }
        if (reinterpret_cast<unsigned char*>(&val.w) != (reinterpret_cast<unsigned char*>(&val) + getOffset<Inherited, 4>()))
        {
            std::cout << "Offset incorrect" << std::endl;
            return 1;
        }

        if (*reinterpret_cast<double*>(reinterpret_cast<unsigned char*>(&val) + getOffset<Inherited, 4>()) != val.w)
        {
            std::cout << "Offset based access not working" << std::endl;
            return 1;
        }
        static_assert(std::is_same<getType<Inherited, 4>, double>::value, "std::is_same_v<getType<Inherited, 4>, double>");
    }
    
    testSerialization();
    testHash();
    testAccess();


    return 0;
}

