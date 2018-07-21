#include <ct/String.hpp>
#include <type_traits>

static constexpr const char* getString() { return "asdf"; }


int main()
{
    static_assert(std::integral_constant<int, ct::strLen("asdf")>::value == 4, "strlen(\"asdf\") == 4");
    static_assert(std::integral_constant<int, ct::strLen(getString())>::value == 4, "strlen(getString()) == 4");

    static_assert(ct::findFirst("asdf:asdf") == 4, "findFirst(\"asdf:asdf\") == 4");
    static_assert(ct::findLast("asdf:asdf") == 4, "findLast(\"asdf:asdf\") == 4");
    static_assert(ct::findLast("asdf:asdf", 'a') == 5, "findLast(\"asdf:asdf\", 'a') == 5");
    static_assert("asdf:asdf"[4] == ':', "\"asdf:asdf\"[4] == ':'");
    static_assert(ct::classNameIdx("TestHash::getName()") == 8, "ct::classNameIdx(\"TestHash::getName()\") == 8");
    
    return 0;
}