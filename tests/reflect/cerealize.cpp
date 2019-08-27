#include "common.hpp"
#include <fstream>

#include <ct/reflect/cerealize.hpp>
#include <ct/reflect/compare.hpp>
#include <ct/reflect/print.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

template <class T, class Read, class Write>
struct CerealizationTester : ::testing::Test
{
    CerealizationTester() : m_path(std::is_same<Read, cereal::JSONInputArchive>::value ? "test.json" : "test.bin") {}

    void test()
    {
        const T data = TestData<T>::init();
        {
            std::ofstream ofs;
            ofs.open(m_path);
            ASSERT_TRUE(ofs.is_open());

            Write archive(ofs);
            archive(cereal::make_nvp("data", data));
        }
        {
            std::ifstream ifs;
            ifs.open(m_path);
            ASSERT_TRUE(ifs.is_open());
            Read archive(ifs);
            T loaded_data;
            archive(cereal::make_nvp("data", loaded_data));
            const auto cerealization_success = ct::compare(data, loaded_data, DebugEqual());
            if (!cerealization_success)
            {
                std::cout << "Cerealization of " << ct::Reflect<T>::getName() << std::endl;
                Write archive(std::cout);
                archive(cereal::make_nvp("data", data));
            }
            EXPECT_TRUE(cerealization_success);
        }
        std::cout << std::endl;
    }

    std::string m_path;
};
template <class T>
using JsonCerealizationTester = CerealizationTester<T, cereal::JSONInputArchive, cereal::JSONOutputArchive>;

template <class T>
using BinaryCerealizationTester = CerealizationTester<T, cereal::BinaryInputArchive, cereal::BinaryOutputArchive>;

TYPED_TEST_SUITE_P(JsonCerealizationTester);
TYPED_TEST_SUITE_P(BinaryCerealizationTester);

TYPED_TEST_P(JsonCerealizationTester, cerealize)
{
    this->test();
}

TYPED_TEST_P(BinaryCerealizationTester, cerealize)
{
    this->test();
}

REGISTER_TYPED_TEST_SUITE_P(BinaryCerealizationTester, cerealize);
REGISTER_TYPED_TEST_SUITE_P(JsonCerealizationTester, cerealize);

INSTANTIATE_TYPED_TEST_SUITE_P(ReflectBinaryCerealizationTest, BinaryCerealizationTester, TestTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(ReflectJsonCerealizationTest, JsonCerealizationTester, TestTypes);

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
