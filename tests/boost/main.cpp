#include "../common.hpp"
#include <ct/interop/boost_program_options.hpp>
#include <ct/reflect/print.hpp>
#include <iostream>

struct SubOptions
{
    float alpha = 1.0;
    float beta = 0.0;
};

namespace ct
{
    REFLECT_BEGIN(SubOptions)
        PUBLIC_ACCESS(alpha)
        PUBLIC_ACCESS(beta)
    REFLECT_END;
}

struct ProgramOptions
{
    REFLECT_INTERNAL_START
        REFLECT_INTERNAL_MEMBER(int, num_iterations, 10)
        REFLECT_INTERNAL_MEMBER(float, epsilon, 1e-3f)
        float field;
        constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START>)
        {
            return ct::makeMemberObjectPointer(
                "field", &ProgramOptions::field, ct::Description("Description of field"));
        }
        REFLECT_INTERNAL_MEMBER(SubOptions, sub)
    REFLECT_INTERNAL_END
};

struct TestStruct
{
    static constexpr auto getTypeHelper() -> typename std::remove_reference<decltype(*this)>::type;
    // using DataType = decltype(getTypeHelper());
    using DataType = decltype(getTypeHelper());
};

int main(int ac, char** av)
{
    ct::po::options_description desc;
    ProgramOptions opts;
    ct::registerOptions(opts, desc);

    std::cout << desc << std::endl;

    ct::po::variables_map vm;
    ct::po::store(ct::po::parse_command_line(ac, av, desc), vm);
    ct::po::notify(vm);

    ct::readOptions(opts, vm);

    std::cout << opts << std::endl;
    using type = TestStruct::DataType;
    std::cout << std::integral_constant<uint32_t, ct::crc32(ct::GetName<type>::getName())>::value << std::endl;
    StaticEquality<uint32_t,
                   std::integral_constant<uint32_t, ct::crc32(ct::GetName<type>::getName())>::value,
                   ct::crc32("TestStruct")>();
    std::cout << ct::GetName<type>::getName() << std::endl;

    std::cout << ct::findNthFromBeginning("asdfasdfasdf", 2, 'a') << std::endl;

    std::cout << ct::GetNameMSVC<float>::parseClassName(
                     "GetName<class std::vector<struct TestStruct,class std::allocator<struct TestStruct> > >::getName")
              << std::endl;
}
