#include <ct/reflect.hpp>
#include <ct/reflect/print.hpp>
#include <iostream>

#ifdef HAVE_CEREAL
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <ct/reflect/cerealize.hpp>
#endif // HAVE_CEREAL

#ifdef HAVE_BOOST
#include <ct/interop/boost_program_options.hpp>
#endif

using Description = ct::Description;

struct MyStruct
{
    REFLECT_INTERNAL_START(MyStruct)
        REFLECT_INTERNAL_MEMBER(float, member_a, 1.0, Description("Member a description"))
        REFLECT_INTERNAL_MEMBER(float, member_b, 2.0, Description("Member b description"))
        REFLECT_INTERNAL_MEMBER(float, member_c, 3.0, Description("Member c description"))
    REFLECT_INTERNAL_END;
};

int main(int ac, char** av)
{
    MyStruct my_struct;
    std::cout << my_struct << std::endl;

#ifdef HAVE_CEREAL

    std::cout << "=======================\n Json cerealization\n";
    {
        cereal::JSONOutputArchive ar(std::cout);
        ar(my_struct);
    }
    std::cout << std::endl;
#endif

#ifdef HAVE_BOOST
    ct::po::options_description desc;
    ct::registerOptions(my_struct, desc);
    std::cout << "=======================\n Program options\n";
    std::cout << desc << std::endl;

    ct::po::variables_map vm;
    ct::po::store(ct::po::parse_command_line(ac, av, desc), vm);
    ct::po::notify(vm);

    ct::readOptions(my_struct, vm);

#endif
}
