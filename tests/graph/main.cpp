#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "ct"

#include <boost/test/unit_test.hpp>
#include <ct/Hash.hpp>
#include <ct/Object.hpp>
#include <ct/String.hpp>
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
    //return typeid(ct::conditional_t<remove_reference_t<T>::IsIndexable,
    //                typename remove_reference_t<T>::Type, typename remove_reference_t<T>::Elem_t).name();
    return typeid(typename remove_reference_t<T>::Type).name();
}

template<class T>
const char* getInputType(T&& t)
{
    //return typeid(decltype(t.inputType())).name();
    return typeid(typename T::Input_t).name();
}

template<class T>
ct::enable_pod<T> testPod(T&& t)
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

template<class T>
typename std::enable_if<std::is_pod<ct::remove_reference_t<typename ct::remove_reference_t<T>::Type>>::value>::type
is_pod(T&& val)
{
    std::cout << "is pod\n";
}

template<class T>
typename std::enable_if<!std::is_pod<ct::remove_reference_t<typename ct::remove_reference_t<T>::Type>>::value>::type
is_pod(T&& val)
{
    std::cout << "is not pod\n";
}


template<class T>
typename std::enable_if<std::is_pod<ct::remove_reference_t<typename ct::remove_reference_t<T>::Type>>::value || remove_reference_t<T>::IsIndexable>::type
is_pod_or_indexable(T&& val)
{
    std::cout << "is pod or indexable\n";
}

BOOST_AUTO_TEST_CASE(unary_operators_scalar)
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
}

BOOST_AUTO_TEST_CASE(unary_operators_scalar_const)
{
    float value = 15.0f;
    auto hashed = CT_MAKE_HASHED(15.0f);

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
}


/*BOOST_AUTO_TEST_CASE(unary_operators_vector)
{
    std::vector<int> intvec(100, 5);
    int indexable = ct::Indexable<std::vector<int>>::IsIndexable;
    auto hashedvec = CT_MAKE_HASHED(intvec);
    hashedvec.getSize(0);
    int const_size = hashedvec.ConstSize;
    int dims = hashedvec.Dims;
    hashedvec(1);
    auto sqvec = ct::makeUnary<ct::Square>(hashedvec);

    std::cout << std::endl;
    std::cout << sqvec.getHash() << std::endl;
    std::cout << getBaseName(sqvec) << std::endl;
    std::cout << getOutputType(sqvec) << std::endl;
    for (int i = 0; i < hashedvec.getSize(); ++i)
    {
        BOOST_REQUIRE_EQUAL(intvec[i], hashedvec(i));
        BOOST_REQUIRE_EQUAL(sqvec(i), hashedvec(i) * hashedvec(i));
    }
}

BOOST_AUTO_TEST_CASE(unary_operators_vector_const)
{
    const std::vector<int> intvec(100, 5);
    int indexable = ct::Indexable<std::vector<int>>::IsIndexable;
    auto hashedvec = CT_MAKE_HASHED(intvec);
    hashedvec.getSize(0);
    int const_size = hashedvec.ConstSize;
    int dims = hashedvec.Dims;
    hashedvec(1);
    auto sqvec = ct::makeUnary<ct::Square>(hashedvec);

    std::cout << std::endl;
    std::cout << sqvec.getHash() << std::endl;
    std::cout << getBaseName(sqvec) << std::endl;
    std::cout << getOutputType(sqvec) << std::endl;
    for (int i = 0; i < hashedvec.getSize(); ++i)
    {
        BOOST_REQUIRE_EQUAL(intvec[i], hashedvec(i));
        BOOST_REQUIRE_EQUAL(sqvec(i), hashedvec(i) * hashedvec(i));
    }
    auto sqrtvec = ct::makeUnary<ct::Sqrt>(sqvec);
    for (int i = 0; i < hashedvec.getSize(); ++i)
    {
        BOOST_REQUIRE_EQUAL(sqrtvec(i), hashedvec(i));
    }
    sqrtvec(0);
}
*/
