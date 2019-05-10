#include "common.hpp"
#include <chrono>
#include <cstring>
#include <ct/extensions/DataTable.hpp>

using namespace ct;
struct TimeIt
{
    TimeIt() { start = std::chrono::high_resolution_clock::now(); }
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
        // std::integral_constant<size_t, memberOffset(&TestA::x)>::value;
        std::cout << memberOffset(&TestA::x) << std::endl;
        std::cout << memberOffset(&TestA::y) << std::endl;
        std::cout << memberOffset(&TestA::z) << std::endl;

        TestB val;
        val.x = 1;
        val.y = 2;
        val.z = 3;
        for (size_t i = 0; i < 20; ++i)
        {
            table.push_back(val);
            mul(val);
        }

        for (size_t i = 0; i < 20; ++i)
        {
            const auto x = table.access(&TestB::x, i);
            const auto y = table.access(&TestB::y, i);
            const auto z = table.access(&TestB::z, i);
            std::cout << x << ", " << y << ", " << z << std::endl;
        }
    }

    for (size_t i = 20; i < 32; i += 4)
    {
        // compare performance to vector of structs
        std::vector<TestB> vec_of_structs;
        const size_t size = 1ULL << i;
        const auto search_value = float(1ULL << (i - 1));
        std::cout << "Testing with " << size << " elements" << std::endl;
        {
            TimeIt time;
            vec_of_structs.reserve(size);
            for (size_t i = 0; i < size; ++i)
            {
                vec_of_structs.push_back({float(i), float(i * 2), float(i * 3)});
            }
            std::cout << "Filling vec of structs took ";
        }

        ct::ext::DataTable<TestB> table;
        {
            TimeIt time;
            table.reserve(vec_of_structs.size());
            for (const auto& vec : vec_of_structs)
            {
                table.push_back(vec);
            }
            std::cout << "Filling table took          ";
        }

        std::chrono::high_resolution_clock::duration d1;
        {
            TimeIt time;
            for (size_t i = 0; i < vec_of_structs.size(); ++i)
            {
                if (vec_of_structs[i].x == search_value)
                {
                    std::cout << "Found at " << i << std::endl;
                    break;
                }
            }
            std::cout << "Searching vec of structs took      ";

            d1 = time.delta();
        }

        std::chrono::high_resolution_clock::duration d2;
        {
            TimeIt time;
            for (size_t i = 0; i < vec_of_structs.size(); ++i)
            {
                if (table.access(&TestB::x, i) == search_value)
                {
                    std::cout << "Found at " << i << std::endl;
                    break;
                }
            }
            std::cout << "Searching data table took          ";
            d2 = time.delta();
        }
        std::cout << "Speedup                            " << float(d1.count()) / float(d2.count()) << std::endl;

        std::chrono::high_resolution_clock::duration d3;
        {
            TimeIt time;
            const auto begin = table.begin(&TestB::x);
            const auto end = table.end(&TestB::x);
            for (auto itr = begin; itr != end; ++itr)
            {
                if (*itr == search_value)
                {
                    std::cout << "Found at " << (itr - begin) << std::endl;
                    break;
                }
            }
            std::cout << "Iterator Searching data table took ";
            d3 = time.delta();
        }
        std::cout << "Iterator Speedup                   " << float(d1.count()) / float(d3.count()) << std::endl;
    }
}
