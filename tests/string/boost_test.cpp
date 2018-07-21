#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "ct"

#include <boost/test/unit_test.hpp>
//#include <ct/Hash.hpp>
#include <ct/String.hpp>
//#include <ct/Object.hpp>
#include <iostream>

using namespace ct;

BOOST_AUTO_TEST_CASE(string)
{
    
    
    /*ctv<ctcrc32("test_string")>::value;
    ctv<classNameIdx("test_string:")>::value;
    
    
    BOOST_REQUIRE_EQUAL(ctv<findLast("asdf:", ':')>::value, 4);
    BOOST_REQUIRE_EQUAL(ctv<findFirst("asdf:")>::value, findFirst("asdf:"));
    BOOST_REQUIRE_EQUAL(findFirst("as:df"), 2);*/
}

template<class T> struct TestHash
{
    //DECLARE_CLASS_HASH;
};

BOOST_AUTO_TEST_CASE(hash)
{
    /*BOOST_REQUIRE_EQUAL(ctv<ctcrc32("test_string")>::value, ctcrc32("test_string"));
    BOOST_REQUIRE_EQUAL(ctv<hashClassName("asdf:asdf")>::value, hashClassName("asdf:asdf"));
    BOOST_REQUIRE_EQUAL(ctv<hashClassName("asdf:asdf")>::value, hashClassName("asdf:asdasdfasdf"));
    BOOST_REQUIRE_EQUAL(ctcrc32("asdf"), ctcrc32Range("asdf", 5));
    BOOST_REQUIRE_EQUAL(hashClassName("asdf::asdlfjasd"), ctcrc32("asdf"));
    BOOST_REQUIRE_EQUAL(TestHash<float>::hash, ctcrc32("TestHash<float>"));
    float value1;
    float value2;
    auto hashed1 = CT_MAKE_HASHED(value1);
    auto hashed2 = CT_MAKE_HASHED(value2);
    BOOST_REQUIRE_NE(hashed1.hash, hashed2.hash);
    BOOST_REQUIRE_NE(TestHash<int>::hash, ctcrc32("TestHash<float>"));
    auto val1 = ctv<hashed1.getHash()>::value;
    auto val2 = ctv<hashed2.getHash()>::value;*/
}
