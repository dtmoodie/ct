#include "../common.hpp"
#include <ct/hash.hpp>

static constexpr const char* getString()
{
    return "asdf";
}

int main()
{
    STATIC_NE(ct::crc32("asdf"), 0);
    STATIC_EQUAL(ct::crc32Range("asdfasdf", 4), ct::crc32("asdf"));
    STATIC_EQUAL(ct::crc32Range("asdfasdf", 4, 4), ct::crc32("asdf"));
    STATIC_EQUAL(ct::crc32Range("asdfasdf", 4, 4), ct::crc32(getString()));
    STATIC_EQUAL(ct::crc32("asdf"), ct::crc32_ignore_whitespace("as   df"));

    STATIC_NE(ct::crc32("asdfkjlaksjdfkjaslkdfjlkasjfklaslkfjsakdjflkasdfj"), 0);
}
