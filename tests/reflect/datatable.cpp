#include "common.hpp"
#include <ct/extensions/DataTable.hpp>

#include <chrono>
#include <cstring>

#include <gtest/gtest.h>

using namespace ct;

namespace ct
{
    template <class T, class A>
    bool operator==(const ct::TArrayView<T>& view, const std::vector<T, A>& vec)
    {
        if (view.size() != vec.size())
        {
            return false;
        }
        if (view.data() == vec.data())
        {
            return true;
        }
        for (size_t i = 0; i < view.size(); ++i)
        {
            if (view[i] != vec[i])
            {
                return false;
            }
        }
        return true;
    }
} // namespace ct

struct TimeIt
{
    TimeIt(double& out) : m_out(out) { start = std::chrono::high_resolution_clock::now(); }
    ~TimeIt()
    {
        auto delta = (std::chrono::high_resolution_clock::now() - start);
        const auto secs = std::chrono::duration_cast<std::chrono::seconds>(delta);
        delta = delta - secs;
        const auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(delta);
        m_out = static_cast<double>(secs.count());
        m_out += nsec.count() * 1e-6;
    }

    decltype(std::chrono::high_resolution_clock::now()) start;
    double& m_out;
};

struct DynStruct
{
    REFLECT_INTERNAL_BEGIN(DynStruct)
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
template <class T>
void tableViewer(const ext::IDataTable<T>& table)
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
        inc(val);
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
        inc(val);
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
        inc(val);
    }

    val = TestData<TestB>::init();
    for (auto y : table.view(&TestB::y))
    {
        EXPECT_EQ(y, val.y);
        inc(val);
    }

    val = TestData<TestB>::init();
    for (auto z : table.view(&TestB::z))
    {
        EXPECT_EQ(z, val.z);
        inc(val);
    }
}

TEST(datatable, ctr_from_vec)
{
    std::vector<TestB> vec;
    auto val = TestData<TestB>::init();
    for (size_t i = 0; i < 20; ++i)
    {
        vec.push_back(val);
        inc(val);
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
        inc(val);
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

TEST(datatable, dyn_array_init)
{
    std::vector<float> embeddings;
    embeddings.resize(20);
    for (auto& x : embeddings)
    {
        x = float(std::rand()) / float(RAND_MAX);
    }
    {
        ext::DataTable<DynStruct> table;
        table.reserve(10);

        DynStruct tmp{0.1F, 0.2F, 0.3F, 0.4F, {embeddings.data(), 20}};
        table.push_back(tmp);
        auto val = table[0];
        for (size_t i = 0; i < val.embeddings.size(); ++i)
        {
            EXPECT_TRUE(fclose(val.embeddings[ssize_t(i)], embeddings[i]));
        }
        auto emb = table.access(&DynStruct::embeddings, 0);
        EXPECT_EQ(val.embeddings.data(), emb.data());
        EXPECT_EQ(table.storage(&DynStruct::x).size(), 1);
        EXPECT_EQ(table.storage(&DynStruct::embeddings).size(), 20);
    }
}

TEST(datatable, dyn_array_reserve_init)
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
            EXPECT_TRUE(fclose(val.embeddings[ssize_t(i)], embeddings[i]));
        }
        auto emb = table.access(&DynStruct::embeddings, 0);
        EXPECT_EQ(val.embeddings.data(), emb.data());
        EXPECT_EQ(table.storage(&DynStruct::x).size(), 1);
        EXPECT_EQ(table.storage(&DynStruct::embeddings).size(), 20);
    }
}

TEST(datatable, array_view)
{
    std::vector<float> embeddings;
    embeddings.resize(20);
    for (auto& x : embeddings)
    {
        x = float(std::rand()) / float(RAND_MAX);
    }
    {
        ext::DataTable<DerivedDynStruct> table;
        DerivedDynStruct tmp;
        tmp.embeddings = ct::TArrayView<float>(embeddings.data(), 20);
        for (int i = 0; i < 20; ++i)
        {
            tmp.x = static_cast<decltype(tmp.x)>(i);
            tmp.y = static_cast<decltype(tmp.x)>(i * 2);
            tmp.w = static_cast<decltype(tmp.x)>(i * 3);
            tmp.h = static_cast<decltype(tmp.x)>(i * 4);
            tmp.conf = 0.9F;
            for (auto& x : embeddings)
            {
                x = float(std::rand()) / float(RAND_MAX);
            }
            table.push_back(tmp);
        }
        tableViewer<DynStruct>(table);
        tableViewer<DerivedDynStruct>(table);

        DynStruct elem;
        static_cast<ext::IDataTable<DerivedDynStruct>&>(table).populateData(elem, 0);
        EXPECT_EQ(embeddings.size(), elem.embeddings.size());
        static_cast<ext::IDataTable<DerivedDynStruct>&>(table).populateData(elem, 19);
        EXPECT_EQ(elem.embeddings, embeddings);
    }
}

struct DataTablePerformance : ::testing::TestWithParam<size_t>
{
    using TestType = TestB;

    void fillVec();

    void fillTable();

    void testEquality();

    void testFill();

    void testSearch();

  private:
    std::vector<TestType> vec_of_structs;
    ct::ext::DataTable<TestType> table;
    double table_fill_time;
    double vec_fill_time;
};

void DataTablePerformance::fillVec()
{
    const size_t size = 1ULL << GetParam();
    TimeIt time(vec_fill_time);
    vec_of_structs.reserve(size);
    auto val = TestData<TestType>::init();
    for (size_t i = 0; i < size; ++i)
    {
        vec_of_structs.push_back(val);
        inc(val);
    }
}

void DataTablePerformance::fillTable()
{
    const size_t size = 1ULL << GetParam();
    TimeIt time(table_fill_time);
    table.reserve(vec_of_structs.size());
    auto val = TestData<TestType>::init();
    for (size_t i = 0; i < size; ++i)
    {
        table.push_back(val);
        inc(val);
    }
}

void DataTablePerformance::testEquality()
{
    EXPECT_EQ(table.size(), vec_of_structs.size());
    for (size_t i = 0; i < vec_of_structs.size(); ++i)
    {
        EXPECT_EQ(table.access(i), vec_of_structs[i]);
    }
}

void DataTablePerformance::testFill()
{
    fillVec();
    fillTable();
    // It takes longer to fill a table than a vec since it's 3 allocations intead of 1
    testEquality();
}

void DataTablePerformance::testSearch()
{
    fillVec();
    fillTable();
    const size_t size = 1ULL << GetParam();
    auto idx = size_t(size * (float(std::rand()) / (float(RAND_MAX) * 2)) + 0.5F);
    auto val = TestData<TestType>::init();
    ASSERT_LE(idx, size);
    for (size_t i = 0; i < idx; ++i)
    {
        inc(val);
    }
    // For several of the test types, the 0th element is intiialized as zero
    auto ptr = Reflect<TestType>::getPtr(Indexer<1>());
    double vec_search_time = 0;
    {
        TimeIt timer(vec_search_time);
        size_t i = 0;
        const auto search_val = ptr.get(val);
        for (; i < size; ++i)
        {
            if (fclose(ptr.get(vec_of_structs[i]), search_val))
            {
                break;
            }
        }
        if (i == size)
        {
            std::cout << table[idx];
        }
        ASSERT_NE(i, size) << " did not find expected value in vec of structs";
        if (i != idx)
        {
            EXPECT_EQ(vec_of_structs[idx], val);
        }
    }
    double table_search_time = 0;
    {
        TimeIt timer(table_search_time);
        size_t i = 0;
        auto view = table.view(ptr.m_ptr);
        const auto search_val = ptr.get(val);
        for (const auto& v : view)
        {
            if (fclose(v, search_val))
            {
                break;
            }
            ++i;
        }
        if (i == size)
        {
            std::cout << table[idx];
        }
        ASSERT_NE(i, size) << " did not find expected value in the table";
        if (i != idx)
        {
            EXPECT_EQ(table[idx], val);
        }
    }
    EXPECT_GE(vec_search_time, table_search_time)
        << " vec of structs was faster while searching for a value at index " << idx;
}

TEST_P(DataTablePerformance, search_performance)
{
    testSearch();
}

INSTANTIATE_TEST_SUITE_P(DataTablePerformance, DataTablePerformance, ::testing::Values(10, 14, 18));

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}