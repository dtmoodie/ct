#include <ct/reflect.hpp>
#include <ct/reflect/print.hpp>
#include <iostream>

#ifdef HAVE_CEREAL
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <ct/reflect/cerealize.hpp>
#endif // HAVE_CEREAL

struct MyStruct
{
    REFLECT_INTERNAL_START
        REFLECT_INTERNAL_MEMBER(float, member_a, 1.0)
        REFLECT_INTERNAL_MEMBER(float, member_b, 2.0)
        REFLECT_INTERNAL_MEMBER(float, member_c, 3.0)
    REFLECT_INTERNAL_END;
};

int main()
{
    MyStruct my_struct;
    std::cout << my_struct << std::endl;

#ifdef HAVE_CEREAL
    {
        cereal::JSONOutputArchive ar(std::cout);
        ar(my_struct);
    }
    std::cout << std::endl;
#endif
}
