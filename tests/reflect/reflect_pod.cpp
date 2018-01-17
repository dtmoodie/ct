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
    REFLECT_INTERNAL_START(Composite)
        REFLECT_INTERNAL_MEMBER(ReflectedStruct, a)
        REFLECT_INTERNAL_MEMBER(ReflectedStruct, b)
    REFLECT_INTERNAL_END;
};

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

        REFLECT_DATA_DERIVED(Inherited, ReflectedStruct)
            REFLECT_DATA_MEMBER(w)
        REFLECT_DATA_END;
    }
}

struct InternallyReflected
{
    REFLECT_INTERNAL_START(InternallyReflected)
        REFLECT_INTERNAL_MEMBER(float, x)
        REFLECT_INTERNAL_MEMBER(float, y)
        REFLECT_INTERNAL_MEMBER(float, z)
    REFLECT_INTERNAL_END;
};

template<class T, int I>
static constexpr size_t getOffset()
{
    return reinterpret_cast<size_t>(&ct::reflect::ReflectData<T>::get(*static_cast<T*>(nullptr), ct::_counter_<I>()));
}

struct TestA
{
    REFLECT_INTERNAL_START(TestA)
        REFLECT_INTERNAL_MEMBER(float, x)
        REFLECT_INTERNAL_MEMBER(float, y)
        REFLECT_INTERNAL_MEMBER(float, z)
    REFLECT_INTERNAL_END;
};

struct TestB
{
    REFLECT_INTERNAL_START(TestB)
        REFLECT_INTERNAL_MEMBER(float, x)
        REFLECT_INTERNAL_MEMBER(float, y)
        REFLECT_INTERNAL_MEMBER(float, z)
    REFLECT_INTERNAL_END;
};

struct TestC
{
    REFLECT_INTERNAL_START(TestC)

        REFLECT_INTERNAL_MEMBER(float, y)
        REFLECT_INTERNAL_MEMBER(float, x)
        REFLECT_INTERNAL_MEMBER(float, z)
    REFLECT_INTERNAL_END;
};

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

