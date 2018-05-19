#include <ct/reflect/cereal.hpp>
#include <ct/reflect/hash.hpp>
#include <ct/reflect/printer.hpp>
#include <ct/reflect/reflect_data.hpp>

#include <cereal/archives/json.hpp>
#include <iostream>

using namespace ct;

struct ReflectedStruct
{
  public:
    float x;
    float y;
    float z;
    int id;
};

struct Inherited : public ReflectedStruct
{
    double w;
};

struct Composite
{
    ReflectedStruct a;
    ReflectedStruct b;
};

struct TestA
{
    float x, y, z;
};

struct TestB
{
    float x, y, z;
};

struct TestC
{
    float y, x, z;
};

struct TestVec
{
    std::vector<float> vec;
};

// Doesn't work with C++11 :/
#if __cplusplus > 201103

struct InternallyReflected
{
    REFLECT_INTERNAL_START(InternallyReflected)
        REFLECT_INTERNAL_MEMBER(float, x)
        REFLECT_INTERNAL_MEMBER(float, y)
        REFLECT_INTERNAL_MEMBER(float, z)
    REFLECT_INTERNAL_END;
};

#else
struct InternallyReflected
{
    float x,y,z;
};
#endif

namespace ct
{
    namespace reflect
    {
        REFLECT_DATA_START(ReflectedStruct)
            REFLECT_DATA_MEMBER(x)
            REFLECT_DATA_MEMBER(y)
            REFLECT_DATA_MEMBER(z)
            REFLECT_DATA_MEMBER(id)
        REFLECT_DATA_END;

        REFLECT_DATA_START(Composite)
            REFLECT_DATA_MEMBER(a)
            REFLECT_DATA_MEMBER(b)
        REFLECT_DATA_END;

        REFLECT_DATA_START(TestA)
            REFLECT_DATA_MEMBER(x)
            REFLECT_DATA_MEMBER(y)
            REFLECT_DATA_MEMBER(z)
        REFLECT_DATA_END;

        REFLECT_DATA_START(TestB)
            REFLECT_DATA_MEMBER(x)
            REFLECT_DATA_MEMBER(y)
            REFLECT_DATA_MEMBER(z)
        REFLECT_DATA_END;

        REFLECT_DATA_START(TestC)
            REFLECT_DATA_MEMBER(y)
            REFLECT_DATA_MEMBER(x)
            REFLECT_DATA_MEMBER(z)
        REFLECT_DATA_END;

        REFLECT_DATA_DERIVED(Inherited, ReflectedStruct)
            REFLECT_DATA_MEMBER(w)
        REFLECT_DATA_END;

        REFLECT_DATA_START(TestVec)
            REFLECT_DATA_MEMBER(vec)
        REFLECT_DATA_END;
#if __cplusplus <= 201103
        REFLECT_DATA_START(InternallyReflected)
            REFLECT_DATA_MEMBER(x)
            REFLECT_DATA_MEMBER(y)
            REFLECT_DATA_MEMBER(z)
        REFLECT_DATA_END;
#endif
    }
}


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

int main(int /*argc*/, char** /*argv*/)
{
    InternallyReflected data2;
    ReflectedStruct data;
    data.x = 0;
    data.y = 1;
    data.z = 2;
    data.id = 3;
    ct::reflect::printStruct(std::cout, data);
    std::cout << std::endl;
    static_assert(std::is_same<ct::reflect::enable_if_reflected<ReflectedStruct>, void>::value, "test1");
    {
        cereal::JSONOutputArchive ar(std::cout);
        ar(data);
    }
    std::cout << std::endl;

    std::cout << "Offset: " << getOffset<ReflectedStruct, 1>() << std::endl;
    //getOffset<ReflectedStruct, 1>() ;

    data2.x = 5;
    data2.y = 6;
    data2.z = 10;
    ct::reflect::printStruct(std::cout, data2);
    std::cout << std::endl;
    static_assert(ct::reflect::ReflectData<Inherited>::I0 == 4, "Fetching base param count is broken");
    static_assert(ct::reflect::ReflectData<Inherited>::N == 5, "Inheritance param counting is broken");
    std::cout << std::endl;
    
    static_assert(ct::reflect::detail::hashMember<Inherited, 0>() != 0, "asdf");
    static_assert(ct::reflect::detail::hashMember<Inherited, 1>() != 0, "asdf");
    static_assert(ct::reflect::detail::hashMember<Inherited, 2>() != 0, "asdf");
    static_assert(ct::reflect::detail::hashMember<Inherited, 3>() != 0, "asdf");
    static_assert(ct::reflect::detail::hashMember<Inherited, 4>() != 0, "asdf");
    static_assert(ct::reflect::detail::hashDataType<float>() != ct::reflect::detail::hashDataType<int>(), "ct::reflect::detail::hashDataType<float>() != ct::reflect::detail::hashDataType<int>()");

    static_assert(ct::strLen(ct::reflect::ReflectData<Inherited>::getName()) == 9, "ct::strLen(ct::reflect::ReflectData<Inherited>::getName()) == 9");
    static_assert(ct::reflect::detail::hashMembers<Inherited>(ct::_counter_<4>()) != 0, "asdf");
    static_assert(ct::reflect::hashMembers<Inherited>() != 0, "ct::reflect::hashMembers<Inherited>() != 0");
    static_assert(ct::detail::ctcrc32(ct::reflect::ReflectData<Inherited>::getName(ct::_counter_<0>())) != ct::detail::ctcrc32(ct::reflect::ReflectData<Inherited>::getName(ct::_counter_<1>())),
                  "ct::detail::ctcrc32(ct::reflect::ReflectData<Inherited>::getName(ct::_counter_<0>())) != ct::detail::ctcrc32(ct::reflect::ReflectData<Inherited>::getName(ct::_counter_<1>()))");

    static_assert(ct::reflect::detail::hashMember<Inherited, 0>() != ct::reflect::detail::hashMember<Inherited, 1>(),
                 "ct::reflect::detail::hashMember<Inherited>(ct::_counter_<0>()) != ct::reflect::detail::hashMember<Inherited>(ct::_counter_<1>())");
    static_assert(ct::reflect::detail::hashDataType<float>() != ct::reflect::detail::hashDataType<double>(), "ct::reflect::detail::hashDataType<float>() != ct::reflect::detail::hashDataType<double>()");
    static_assert(ct::reflect::hashMembers<TestA>() == ct::reflect::hashMembers<TestB>(), "");
    static_assert(ct::reflect::hashMembers<TestA>() != ct::reflect::hashMembers<TestC>(), "");

    static_assert(ct::reflect::detail::hashDataType<float>() == 3383058069, "Cross platform hash test");

    // Need to make sure hashes are the same on windows and linux
    static_assert(ct::reflect::classHash<TestA>() == 2489699677, "Test to make sure the hash is consistent across platforms");

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
    

    Inherited test;
    ct::reflect::printStruct(std::cout, test);
    std::cout << std::endl;
    {
        cereal::JSONOutputArchive ar(std::cout);
        ar(test);
    }
    std::cout << std::endl;

    Composite cmp;
    ct::reflect::printStruct(std::cout, cmp);
    std::cout << std::endl;
    {
        cereal::JSONOutputArchive ar(std::cout);
        ar(cmp);
    }
    
    std::cout << std::endl << ct::reflect::classHash<ReflectedStruct>() << std::endl;
    return 0;
}

