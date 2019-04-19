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

template <class Read, class Write>
struct CerealizationTester
{
    CerealizationTester(std::string path) : m_path(std::move(path)) {}

    template <class T>
    void test(const T& data)
    {
        {
            std::ofstream ofs(m_path);
            Write archive(ofs);
            archive(cereal::make_nvp("data", data));
        }
        {
            std::cout << "Cerealization of " << ct::Reflect<T>::getName() << std::endl;
            Write archive(std::cout);
            archive(cereal::make_nvp("data", data));
        }
        {
            std::ifstream ifs(m_path);
            Read archive(ifs);
            T loaded_data;
            archive(cereal::make_nvp("data", loaded_data));
            if (!ct::compare(data, loaded_data, DebugEqual()))
            {
                std::cout << "Failed cerealization" << std::endl;
                std::abort();
            }
        }
        std::cout << std::endl;
    }

    std::string m_path;
};

int main()
{
    {
        CerealizationTester<cereal::JSONInputArchive, cereal::JSONOutputArchive> json_tester("test.json");
        testTypes(json_tester);
    }
    std::cout << "====================" << std::endl;
    std::cout << "Binary serialization" << std::endl;
    std::cout << "====================" << std::endl;
    {
        // CerealizationTester<cereal::BinaryInputArchive, cereal::BinaryOutputArchive> json_tester("test.bin");
        // testTypes(json_tester);
    }
}
