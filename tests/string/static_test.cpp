#include "../common.hpp"

#include <ct/String.hpp>
#include <type_traits>
#include <iostream>
#include <iterator>

static constexpr const char* getString() { return "asdf"; }

int main()
{
    STATIC_EQUAL(ct::strLen("asdf"), 4);
    STATIC_EQUAL(ct::strLen(getString()), 4);

    STATIC_EQUAL(ct::findFirst("asdf:asdf"), 4);
    STATIC_EQUAL(ct::findLast("asdf:asdf"), 4);
    STATIC_EQUAL(ct::findLast("asdf:asdf", 'a'), 5);
    STATIC_EQUAL("asdf:asdf"[4],  ':');
    STATIC_EQUAL(ct::classNameIdx("TestHash::getName()"), 8);


    return 0;
}
