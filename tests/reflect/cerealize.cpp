#include "common.hpp"
#include <fstream>

#include <ct/reflect/cerealize.hpp>
#include <ct/reflect/compare.hpp>
#include <ct/reflect/print-container-inl.hpp>
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
        std::stringstream serialized;
        {
            WRITE archive(serialized);
            archive(cereal::make_nvp("data", data));
        }
        {
            READ archive(serialized);
            T loaded_data;
            bool cerealization_success = false;
            std::stringstream msg;
            try
            {
                archive(cereal::make_nvp("data", loaded_data));
            }
            catch (std::exception& exception)
            {

                msg << "Cerealization of " << ct::Reflect<T>::getTypeName()
                    << " failed with exception: " << exception.what() << std::endl;
            }
            cerealization_success = ct::compare(data, loaded_data, DebugEqual());
            if (!cerealization_success)
            {
                msg << "Cerealization of " << ct::Reflect<T>::getTypeName() << std::endl;
                ct::printStruct(msg, data);
                msg << "\nDeserialized \n";
                ct::printStruct(msg, loaded_data);
                msg << "\n Serialized format:\n";
                msg << serialized.str();
                msg << std::endl;
            }
            EXPECT_TRUE(cerealization_success) << msg.str();
        }
        std::cout << std::endl;
    }

    void testBinary()
    {
        test<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
    }

    void testJson()
    {
        test<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
    }
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

#include "../enum/enum.hpp"


REGISTER_TYPED_TEST_SUITE_P(Cerealization, json, binary);

using Types = TestTypes::Append<ct::VariadicTypedef<MyClass::MyEnum, MyClass::SecondEnum>>::type;

TEST_DATA(MyClass::MyEnum, MyClass::MyEnum::kVALUE1);
TEST_DATA(MyClass::SecondEnum, MyClass::SecondEnum::kRGB);

INSTANTIATE_TYPED_TEST_SUITE_P(ReflectCerealize, Cerealization, ToTestTypes<Types>::type);


TEST(access_token, set_on_dtor)
{
    PrivateGetAndSet obj;
    ASSERT_NE(obj.getX(), 5);
    auto prop = ct::Reflect<PrivateGetAndSet>::getPtr(ct::Indexer<0>{});
    {
        auto token = prop.set(obj);
        token.get() = 5;
    }
    ASSERT_EQ(obj.getX(), 5);
}

TEST(access_token, set_on_dtor_from_temp)
{
    PrivateGetAndSet obj;
    ASSERT_NE(obj.getX(), 5);
    auto prop = ct::Reflect<PrivateGetAndSet>::getPtr(ct::Indexer<0>{});
    auto setter = [](float& val)
    {
        val = 5;
    };
    {
        setter(ct::ref(prop.set(obj)));
    }
    ASSERT_EQ(obj.getX(), 5);
}


TEST(access_token, set_on_dtor)
{
    PrivateGetAndSet obj;
    ASSERT_NE(obj.getX(), 5);
    auto prop = ct::Reflect<PrivateGetAndSet>::getPtr(ct::Indexer<0>{});
    {
        auto token = prop.set(obj);
        token.get() = 5;
    }
    ASSERT_EQ(obj.getX(), 5);
}

TEST(access_token, set_on_dtor_from_temp)
{
    PrivateGetAndSet obj;
    ASSERT_NE(obj.getX(), 5);
    auto prop = ct::Reflect<PrivateGetAndSet>::getPtr(ct::Indexer<0>{});
    auto setter = [](float& val)
    {
        val = 5;
    };
    {
        setter(ct::ref(prop.set(obj)));
    }
    ASSERT_EQ(obj.getX(), 5);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
