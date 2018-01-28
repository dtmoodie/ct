#include <ct/detail/TypeTraits.hpp>

struct DefaultConstructor
{
    DefaultConstructor() {}
};

struct NonDefaultConstructor
{
    NonDefaultConstructor(int x) {}
};


int main()
{
    static_assert(ct::is_default_constructible<DefaultConstructor>::value, "is_default_constructible<DefaultConstructor>::value");
    static_assert(!ct::is_default_constructible<NonDefaultConstructor>::value, "is_default_constructible<DefaultConstructor>::value");
}