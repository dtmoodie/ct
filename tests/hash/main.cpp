#include "../common.hpp"
#include <ct/Hash.hpp>
#include <ct/Object.hpp>

#include <iostream>
using namespace ct;

struct HashedStruct
{
    DECLARE_CLASS_HASH
    static constexpr const char* getName(){return CT_STRUCT_MAGIC_FUNCTION;}
};


int main()
{
    static_assert(ct::ctcrc32("asdf") != 0, "asdf");
    static_assert(ct::ctcrc32Range("asdfasdf", 4) == ct::ctcrc32("asdf"), "ct::ctcrc32Range(\"asdfasdf\", 4) == ct::ctcrc32(\"asdf\")");
    static_assert(ct::ctcrc32Range("asdfasdf", 4, 4) == ct::ctcrc32("asdf"), "ct::ctcrc32Range(\"asdfasdf\", 4, 4) == ct::ctcrc32(\"asdf\")");
    static_assert(ct::classNameIdx("HashedStruct::") == ct::classNameIdx(static_cast<const char*>("HashedStruct::")), "asdf");
    static_assert(HashedStruct::getHash() == ct::ctcrc32("HashedStruct"), "HashedStruct::getHash() == ct::ctcrc32(\"HashedStruct\")");

    std::cout << HashedStruct::getName() << std::endl;
}
