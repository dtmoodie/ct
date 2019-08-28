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

template <class T>
struct Cerealization : ::testing::Test
{
    template <class READ, class WRITE>
    void test()
    {
        const T data = TestData<T>::init();
        {
            std::ofstream ofs;
            ofs.open(m_path);
            ASSERT_TRUE(ofs.is_open());

            WRITE archive(ofs);
            archive(cereal::make_nvp("data", data));
        }
        {
            std::ifstream ifs;
            ifs.open(m_path);
            ASSERT_TRUE(ifs.is_open());
            READ archive(ifs);
            T loaded_data;
            archive(cereal::make_nvp("data", loaded_data));
            const auto cerealization_success = ct::compare(data, loaded_data, DebugEqual());
            if (!cerealization_success)
            {
                std::cout << "Cerealization of " << ct::Reflect<T>::getName() << std::endl;
                WRITE archive(std::cout);
                archive(cereal::make_nvp("data", data));
            }
            EXPECT_TRUE(cerealization_success);
        }
        std::cout << std::endl;
    }

    void testBinary()
    {
        m_path = "test.bin";
        test<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
    }

    void testJson()
    {
        m_path = "test.json";
        test<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
    }

    std::string m_path;
};

TYPED_TEST_SUITE_P(Cerealization);
TYPED_TEST_SUITE_P(BinaryCerealizationTester);

TYPED_TEST_P(Cerealization, binary)
{
    this->testBinary();
}

TYPED_TEST_P(Cerealization, json)
{
    this->testJson();
}

REGISTER_TYPED_TEST_SUITE_P(Cerealization, json, binary);

INSTANTIATE_TYPED_TEST_SUITE_P(ReflectCerealize, Cerealization, ToTestTypes<TestTypes>::type);

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
