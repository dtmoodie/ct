#include <ct/interop/boost_program_options.hpp>
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
    REFLECT_INTERNAL_START(ProgramOptions)
        REFLECT_INTERNAL_MEMBER(int, num_iterations, 10)
        REFLECT_INTERNAL_MEMBER(float, epsilon, 1e-3f)
        float field;
        constexpr static auto getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_START - 1>)
        {
            return ct::makeMemberObjectPointer(
                "field", &ProgramOptions::field, ct::Description("Description of field"));
        }
        REFLECT_INTERNAL_MEMBER(SubOptions, sub)
    REFLECT_INTERNAL_END
};

int main()
{
    ct::po::options_description desc;
    ProgramOptions opts;
    ct::registerOptions(opts, desc);

    std::cout << desc << std::endl;
}
