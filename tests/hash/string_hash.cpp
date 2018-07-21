#include "../common.hpp"
#include <ct/Hash.hpp>

static constexpr const char* getString() { return "asdf"; }
int main()
{
    STATIC_NE(ct::ctcrc32("asdf"),  0);
    STATIC_EQUAL(ct::ctcrc32Range("asdfasdf", 4), ct::ctcrc32("asdf"));
    STATIC_EQUAL(ct::ctcrc32Range("asdfasdf", 4, 4), ct::ctcrc32("asdf"));
    STATIC_EQUAL(ct::ctcrc32Range("asdfasdf", 4, 4), ct::ctcrc32(getString()));
    STATIC_EQUAL(ct::ctcrc32("asdf"), ct::ctcrc32_ignore_whitespace("as   df"));
    STATIC_EQUAL(ct::ctcrc32Range("TestClass::functionName()", 0, ct::classNameIdx("TestClass::functionName()")), ct::ctcrc32("TestClass"));

    STATIC_NE(ct::ctcrc32("asdfkjlaksjdfkjaslkdfjlkasjfklaslkfjsakdjflkasdfj"), 0);
}