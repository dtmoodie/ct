#include "common.hpp"
#include <ct/extensions/DataTable.hpp>

#include <chrono>
#include <cstring>

#include <gtest/gtest.h>

using namespace ct;

struct TimeIt
{
    TimeIt(double& out) : m_out(out) { start = std::chrono::high_resolution_clock::now(); }
    ~TimeIt()
    {
        auto delta = (std::chrono::high_resolution_clock::now() - start);
        const auto secs = std::chrono::duration_cast<std::chrono::seconds>(delta);
        delta = delta - secs;
        const auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(delta);
        m_out = secs.count();
        m_out += nsec.count() * 1e-6;
    }

    decltype(std::chrono::high_resolution_clock::now()) start;
    double& m_out;
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

struct DerivedDynStruct : public DynStruct
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

template <class T>
ct::ext::DataTable<T> createAndFillTable(size_t elems)
{
    auto val = TestData<T>::init();
    ct::ext::DataTable<TestB> table;
    table.reserve(elems);
    for (size_t i = 0; i < 20; ++i)
    {
        table.push_back(val);
        mul(val);
    }
    return table;
}

TEST(datatable, write)
{
    ct::ext::DataTable<TestB> table = createAndFillTable<TestB>(20);
    EXPECT_EQ(table.size(), 20);
}

TEST(datatable, read)
{
    ct::ext::DataTable<TestB> table = createAndFillTable<TestB>(20);
    TestB val = TestData<TestB>::init();
    for (size_t i = 0; i < 20; ++i)
    {
        auto read = table.access(i);
        EXPECT_EQ(val, read);
        mul(val);
    }
}

TEST(datatable, read_element)
{
    ct::ext::DataTable<TestB> table = createAndFillTable<TestB>(20);
    TestB val = TestData<TestB>::init();

    auto begin_x = table.begin(&TestB::x);
    auto begin_y = table.begin(&TestB::y);
    auto begin_z = table.begin(&TestB::z);
    auto end_x = table.end(&TestB::x);
    auto end_y = table.end(&TestB::y);
    auto end_z = table.end(&TestB::z);
    EXPECT_EQ(end_x - begin_x, 20);
    EXPECT_EQ(end_y - begin_y, 20);
    EXPECT_EQ(end_z - begin_z, 20);

    val = TestData<TestB>::init();
    for (auto x : table.view(&TestB::x))
    {
        EXPECT_EQ(x, val.x);
        mul(val);
    }

    val = TestData<TestB>::init();
    for (auto y : table.view(&TestB::y))
    {
        EXPECT_EQ(y, val.y);
        mul(val);
    }

    val = TestData<TestB>::init();
    for (auto z : table.view(&TestB::z))
    {
        EXPECT_EQ(z, val.z);
        mul(val);
    }
}

TEST(datatable, ctr_from_vec)
{
    std::vector<TestB> vec;
    auto val = TestData<TestB>::init();
    for (size_t i = 0; i < 20; ++i)
    {
        vec.push_back(val);
        mul(val);
    }

    ct::ext::DataTable<TestB> table(vec);
    EXPECT_EQ(table.size(), 20);
    for (size_t i = 0; i < 20; ++i)
    {
        EXPECT_EQ(table.access(i), vec[i]);
    }
}

TEST(datatable, push)
{
    std::vector<TestB> vec;
    auto val = TestData<TestB>::init();
    for (size_t i = 0; i < 20; ++i)
    {
        vec.push_back(val);
        mul(val);
    }

    ct::ext::DataTable<TestB> table;
    for (const auto& v : vec)
    {
        table.push_back(v);
    }
    EXPECT_EQ(table.size(), 20);
    for (size_t i = 0; i < 20; ++i)
    {
        EXPECT_EQ(table.access(i), vec[i]);
    }
}

struct DataTablePerformance : ::testing::TestWithParam<size_t>
{
    using T = TestB;

    void fillVec()
    {
        const size_t size = 1ULL << GetParam();
        TimeIt time(vec_fill_time);
        vec_of_structs.reserve(size);
        auto val = TestData<TestB>::init();
        for (size_t i = 0; i < size; ++i)
        {
            vec_of_structs.push_back(val);
            mul(val);
        }
    }

    void fillTable()
    {
        const size_t size = 1ULL << GetParam();
        TimeIt time(table_fill_time);
        table.reserve(vec_of_structs.size());
        auto val = TestData<TestB>::init();
        for (size_t i = 0; i < size; ++i)
        {
            table.push_back(val);
            mul(val);
        }
    }

    void testEquality()
    {
        for (size_t i = 0; i < vec_of_structs.size(); ++i)
        {
            EXPECT_EQ(table.access(i), vec_of_structs[i]);
        }
    }

    void testFill()
    {
        fillVec();
        fillTable();
        // It takes longer to fill a table than a vec since it's 3 allocations intead of 1
        EXPECT_GE(table_fill_time, vec_fill_time);
        testEquality();
    }

    void testSearch()
    {
        fillVec();
        fillTable();
        const size_t size = 1ULL << GetParam();
        auto idx = size_t(size * (float(std::rand()) / (float(RAND_MAX) * 2)) + 0.5F);
        auto val = TestData<TestB>::init();
        for (size_t i = 0; i < idx; ++i)
        {
            mul(val);
        }
        auto ptr = Reflect<T>::getPtr(Indexer<1>());
        double vec_search_time = 0;
        {
            TimeIt timer(vec_search_time);
            size_t i = 0;
            for (; i < size; ++i)
            {
                if (fclose(ptr.get(vec_of_structs[i]), ptr.get(val)))
                {
                    break;
                }
            }
            EXPECT_EQ(i, idx) << " did not find expected value in vec of structs";
        }
        double table_search_time = 0;
        {
            TimeIt timer(table_search_time);
            size_t i = 0;
            auto view = table.view(ptr.m_ptr);
            for (const auto& v : view)
            {
                if (fclose(v, ptr.get(val)))
                {
                    break;
                }
                ++i;
            }
            EXPECT_EQ(i, idx) << " value not found in table";
        }
        EXPECT_GE(vec_search_time, table_search_time)
            << " vec of structs was faster while searching for a value at index " << idx;
    }

  private:
    std::vector<TestB> vec_of_structs;
    ct::ext::DataTable<TestB> table;
    double table_fill_time;
    double vec_fill_time;
};

TEST_P(DataTablePerformance, fill_performance)
{
    testFill();
}

TEST_P(DataTablePerformance, search_performance)
{
    testSearch();
}

INSTANTIATE_TEST_SUITE_P(DataTablePerformance, DataTablePerformance, ::testing::Values(3, 6, 10));

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    /*for (size_t i = 20; i < 18; i += 4)
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
    }*/

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
            if (val.embeddings.data() != emb.data())
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
            for (int i = 0; i < 20; ++i)
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
