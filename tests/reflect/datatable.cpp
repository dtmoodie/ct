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

struct DynStruct
{
    REFLECT_INTERNAL_START(DynStruct)
        REFLECT_INTERNAL_MEMBER(float, x)
        REFLECT_INTERNAL_MEMBER(float, y)
        REFLECT_INTERNAL_MEMBER(float, w)
        REFLECT_INTERNAL_MEMBER(float, h)
        REFLECT_INTERNAL_MEMBER(TArrayView<float>, embeddings)
    REFLECT_INTERNAL_END;
};

struct DerivedDynStruct: public DynStruct
{
    REFLECT_INTERNAL_DERIVED(DerivedDynStruct, DynStruct)
        REFLECT_INTERNAL_MEMBER(float, conf)
    REFLECT_INTERNAL_END;
};


// We intentionally use a IDataTable of the base struct here to ensure
// We can still interact with a table of the derived type
void tableViewer(const ext::IDataTable<DynStruct>& table)
{
    std::cout << table << std::endl;
}

bool fclose(float f1, float f2, float eps = 0.00001F)
{
    return std::abs(f1 - f2) < eps;
}

int main()
{
    {
        ct::ext::DataTable<TestB> table;
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

    for (size_t i = 20; i < 18; i += 4)
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
            assert(table.storage(&TestB::x).size() == vec_of_structs.size());
        }

        std::chrono::high_resolution_clock::duration d1;
        {
            TimeIt time;
            for (size_t i = 0; i < vec_of_structs.size(); ++i)
            {
                if (fclose(vec_of_structs[i].x, search_value))
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
                if (fclose(table.access(&TestB::x, i), search_value))
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
                if (fclose(*itr, search_value))
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

    {
        std::vector<float> embeddings;
        embeddings.resize(20);
        for (auto& x : embeddings)
        {
            x = float(std::rand()) / float(RAND_MAX);
        }
        {
            ext::DataTable<DynStruct> table;
            table.resizeSubarray(&DynStruct::embeddings, 20);
            table.reserve(10);

            DynStruct tmp{0.1F, 0.2F, 0.3F, 0.4F, {embeddings.data(), 20}};
            table.push_back(tmp);
            auto val = table[0];
            for (size_t i = 0; i < val.embeddings.size(); ++i)
            {
                if (!fclose(val.embeddings[ssize_t(i)], embeddings[i]))
                {
                    std::cout << "Embeddings not copied correctly" << std::endl;
                    return -1;
                }
            }
            auto emb = table.access(&DynStruct::embeddings, 0);
            if(val.embeddings.data() != emb.data())
            {
                std::cout << "Retrieved embeddings pointing to the wrong thing" << std::endl;
                return -1;
            }
            assert(table.storage(&DynStruct::x).size() == 1);
            assert(table.storage(&DynStruct::embeddings).size() == 20);
        }
        {
            ext::DataTable<DerivedDynStruct> table;
            DerivedDynStruct tmp;
            tmp.embeddings = ct::TArrayView<float>(embeddings.data(), 20);
            for(int i = 0; i < 20; ++i)
            {
                tmp.x = i;
                tmp.y = i * 2;
                tmp.w = i * 3;
                tmp.h = i * 4;
                tmp.conf = 0.9F;
                for (auto& x : embeddings)
                {
                    x = float(std::rand()) / float(RAND_MAX);
                }
                table.push_back(tmp);
            }
            tableViewer(table);
        }
    }

}
