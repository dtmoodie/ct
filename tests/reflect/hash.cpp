#include "Reflect.hpp"
#include <ct/reflect/hash.hpp>

#include <iostream>

namespace
{
template<class T>
void printType()
{
    std::cout << ct::reflect::ReflectData<T>::getName()<< std::endl;
    std::cout << "  " << ct::reflect::classHash<T>() << std::endl;
    std::cout << "  " << ct::reflect::hashMembers<T>() << std::endl;
    std::cout << std::endl;
}
}


void testHash()
{
    printType<ReflectedStruct>();
    printType<Inherited>();
    printType<Composite>();
    printType<TestA>();
    printType<TestB>();
    printType<TestC>();
    printType<TestVec>();
    static_assert(ct::reflect::detail::hashMember<Inherited, 0>() != 0, "asdf");
    static_assert(ct::reflect::detail::hashMember<Inherited, 1>() != 0, "asdf");
    static_assert(ct::reflect::detail::hashMember<Inherited, 2>() != 0, "asdf");
    static_assert(ct::reflect::detail::hashMember<Inherited, 3>() != 0, "asdf");
    static_assert(ct::reflect::detail::hashMember<Inherited, 4>() != 0, "asdf");
    static_assert(ct::reflect::detail::hashDataType<float>() != ct::reflect::detail::hashDataType<int>(), "ct::reflect::detail::hashDataType<float>() != ct::reflect::detail::hashDataType<int>()");
    static_assert(ct::reflect::detail::hashMembers<Inherited>(ct::_counter_<4>()) != 0, "asdf");
    static_assert(ct::reflect::hashMembers<Inherited>() != 0, "ct::reflect::hashMembers<Inherited>() != 0");
    static_assert(ct::reflect::detail::hashMember<Inherited, 0>() != ct::reflect::detail::hashMember<Inherited, 1>(),
                 "ct::reflect::detail::hashMember<Inherited>(ct::_counter_<0>()) != ct::reflect::detail::hashMember<Inherited>(ct::_counter_<1>())");

    static_assert(ct::reflect::detail::hashDataType<float>() != ct::reflect::detail::hashDataType<double>(), "ct::reflect::detail::hashDataType<float>() != ct::reflect::detail::hashDataType<double>()");
    static_assert(ct::reflect::hashMembers<TestA>() == ct::reflect::hashMembers<TestB>(), "");
    static_assert(ct::reflect::hashMembers<TestA>() != ct::reflect::hashMembers<TestC>(), "");

    static_assert(ct::reflect::detail::hashDataType<float>() == 3383058069, "Cross platform hash test");

    // Need to make sure hashes are the same on windows and linux
    //static_assert(ct::reflect::classHash<TestA>() == 2489699677, "Test to make sure the hash is consistent across platforms");

    std::cout << std::endl << ct::reflect::classHash<ReflectedStruct>() << std::endl;
}

