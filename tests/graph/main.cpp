#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "ct"

#include <boost/test/unit_test.hpp>
#include <ct/Hash.hpp>
#include <ct/String.hpp>
#include <ct/Object.hpp>
#include <ct/operators/Unary.hpp>
#include <iostream>

using namespace ct;

template<class T> 
const char* getBaseName(T&& t)
{
    return typeid(T).name();
}

template<class T>
const char* getOutputType(T&& t)
{
    //return typeid(decltype(t())).name();
    return typeid(typename std::remove_reference<T>::type::Type).name();
}

template<class T>
const char* getInputType(T&& t)
{
    //return typeid(decltype(t.inputType())).name();
    return typeid(typename T::Input_t).name();
}

template<class T>
ct::enable_if_pod<T> testPod(T&& t)
{
    return;
}

struct vec
{
    float data[3];
};

template<class T> int test(const T&)
{
    return std::is_pod<remove_reference_t<typename remove_reference_t<T>::Type>>::value || T::IsIndexable;
}

BOOST_AUTO_TEST_CASE(unary_operators)
{
    float value = 15.0f;
    auto hashed = CT_MAKE_HASHED(value);
    
    std::cout << hashed.getHash() << std::endl;
    std::cout << getBaseName(hashed) << std::endl;
    std::cout << getOutputType(hashed) << std::endl;
    testPod(hashed);
    
    auto sqop = ct::makeUnary<ct::Square>(hashed);
    std::cout << std::endl;
    std::cout << sqop.getHash() << std::endl;
    std::cout << getBaseName(sqop) << std::endl;
    std::cout << getOutputType(sqop) << std::endl;
    BOOST_REQUIRE_EQUAL(sqop(), value*value);
    auto result = sqop();
    
    std::cout << std::endl;
    auto sqrtop = ct::makeUnary<ct::Sqrt>(sqop);
    std::cout << sqrtop.getHash() << std::endl;
    std::cout << getBaseName(sqrtop) << std::endl;
    std::cout << getOutputType(sqrtop) << std::endl;
    BOOST_REQUIRE_EQUAL(sqrtop(), value);
    std::vector<int> intvec(100, 5);
    auto hashedvec = CT_MAKE_HASHED(intvec);
    intvec.resize(100);
    hashedvec.getSize(0);
    std::enable_if<test_pod_or_indexable(&hashed)>::type* ptr = 0;
}
