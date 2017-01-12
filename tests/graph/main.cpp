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

BOOST_AUTO_TEST_CASE(operators)
{
    float value = 15.0f;
    auto hashed = CT_MAKE_HASHED(value);
    std::cout << hashed.getHash() << std::endl;
    std::cout << getBaseName(hashed) << std::endl;
    std::cout << getOutputType(hashed) << std::endl;
    testPod(hashed);

    auto sqop = ct::makeUnary<ct::Square>(hashed);
    std::cout << sqop.getHash() << std::endl;
    std::cout << getBaseName(sqop) << std::endl;
    std::cout << getOutputType(sqop) << std::endl;
    auto result = sqop();
    auto sqrtop = ct::makeUnary<ct::Sqrt>(sqop);
    std::cout << sqrtop.getHash() << std::endl;
    std::cout << getBaseName(sqrtop) << std::endl;
    std::cout << getOutputType(sqrtop) << std::endl;
    
    auto result2 = sqrtop();
}
