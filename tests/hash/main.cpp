#include <ct/Hash.hpp>
#include <ct/Object.hpp>

#include <iostream>
using namespace ct;

struct HashedStruct
{
    DECLARE_CLASS_HASH
};

int main()
{
    //static_assert(std::integral_constant<uint32_t, HashedStruct::hash>::value != 0, "hashed struct");
    static_assert(ct::ctcrc32("asdf") != 0, "asdf");
    static_assert(ct::ctcrc32Range("asdfasdf", 4) == ct::ctcrc32("asdf"), "ct::ctcrc32Range(\"asdfasdf\", 4) == ct::ctcrc32(\"asdf\")");
    static_assert(ct::ctcrc32Range("asdfasdf", 4, 4) == ct::ctcrc32("asdf"), "ct::ctcrc32Range(\"asdfasdf\", 4, 4) == ct::ctcrc32(\"asdf\")");
    static_assert(ct::classNameIdx("HashedStruct::") == ct::classNameIdx(static_cast<const char*>("HashedStruct::")), "asdf");
    static_assert(HashedStruct::getHash() == ct::ctcrc32("HashedStruct"), "HashedStruct::getHash() == ct::ctcrc32(\"HashedStruct\")");
}