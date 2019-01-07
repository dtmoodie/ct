#include <ct/TypeTraits.hpp>

struct DefaultConstructor
{
    DefaultConstructor() {}
};

struct NonDefaultConstructor
{
    NonDefaultConstructor(int x) {}
};

struct NonWritable
{

};

int main()
{
    static_assert(ct::is_default_constructible<DefaultConstructor>::value, "is_default_constructible<DefaultConstructor>::value");
    static_assert(!ct::is_default_constructible<NonDefaultConstructor>::value, "is_default_constructible<DefaultConstructor>::value");

    static_assert(ct::StreamWritable<float>::value == true, "StreamWritable<float>");

    static_assert(ct::StreamWritable<NonWritable>::value == false, "StreamWritable<NonWritable>");
}