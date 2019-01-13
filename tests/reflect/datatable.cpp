#include <ct/extensions/DataTable.hpp>
#include "common.hpp"
#include <cstring>
#include <chrono>

using namespace ct;
struct TimeIt
{
    TimeIt()
    {
        start = std::chrono::high_resolution_clock::now();
    }
    ~TimeIt()
    {
        const auto delta = (std::chrono::high_resolution_clock::now() - start);
        std::cout << delta.count();
        std::cout << std::endl;
    }
    std::chrono::high_resolution_clock::duration delta() const
    {
        return (std::chrono::high_resolution_clock::now() - start);
    }
    decltype(std::chrono::high_resolution_clock::now()) start;
};
int main()
{
    {
        ct::ext::DataTable<TestB> table;
        //std::integral_constant<size_t, pointerValue(&TestA::x)>::value;
        std::cout << ext::pointerValue(&TestA::x) << std::endl;
        std::cout << ext::pointerValue(&TestA::y) << std::endl;
        std::cout << ext::pointerValue(&TestA::z) << std::endl;

        TestB val;
        val.x = 1;
        val.y = 2;
        val.z = 3;
        for(size_t i = 0; i < 20; ++i)
        {
            table.push_back(val);
            mul(val);
        }

        for(size_t i = 0; i < 20; ++i)
        {
            const auto x = table.access(&TestB::x, i);
            const auto y = table.access(&TestB::y, i);
            const auto z = table.access(&TestB::z, i);
            std::cout << x << ", " << y << ", " << z << std::endl;
        }
    }


    for(size_t i = 0; i < 10; ++i)
    {


    {
        // compare performance to vector of structs
        std::vector<TestB> vec_of_structs;

        {
            TimeIt time;
            vec_of_structs.reserve(1e9);
            for(size_t i = 0; i < 1e9; ++i)
            {
                vec_of_structs.push_back({float(i), float(i*2), float(i*3)});
            }
            std::cout << "Filling vec of structs took ";
        }

        ct::ext::DataTable<TestB> table;
        {
            TimeIt time;
            table.reserve(vec_of_structs.size());
            for(const auto& vec : vec_of_structs)
            {
                table.push_back(vec);
            }
            std::cout << "Filling table took          ";
        }

        std::chrono::high_resolution_clock::duration d1;
        {
            TimeIt time;
            for(size_t i = 0; i < vec_of_structs.size(); ++i)
            {
                if(vec_of_structs[i].x == float(1e8))
                {
                    std::cout << "Found at " << i << std::endl;
                    break;
                }
            }
            std::cout << "Searching vec of structs took ";

            d1 = time.delta();
        }

        std::chrono::high_resolution_clock::duration d2;
        {
            TimeIt time;
            for(size_t i = 0; i < vec_of_structs.size(); ++i)
            {
                if(table.access(&TestB::x, i) == float(1e8))
                {
                    std::cout << "Found at " << i << std::endl;
                    break;
                }
            }
            std::cout << "Searching data table took     ";
            d2 = time.delta();
        }
        std::cout << "Speedup                       " << float(d1.count()) / float(d2.count()) << std::endl;
    }

    }

}
