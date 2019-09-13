#include <ct/type_traits.hpp>

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
    static_assert(ct::IsDefaultConstructible<DefaultConstructor>::value, "IsDefaultConstructible<DefaultConstructor>::value");
    static_assert(!ct::IsDefaultConstructible<NonDefaultConstructor>::value, "IsDefaultConstructible<DefaultConstructor>::value");

    static_assert(ct::StreamWritable<float>::value == true, "StreamWritable<float>");

    static_assert(ct::StreamWritable<NonWritable>::value == false, "StreamWritable<NonWritable>");

}
