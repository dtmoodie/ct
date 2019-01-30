#include "../common.hpp"
#include "common.hpp"
#include "ct/reflect/print.hpp"
#include <chrono>
#include <ct/Hash.hpp>

ct::EnableIfReflected<TestA, void> testEnabledFunction()
{
}



int main()
{
    testEnabledFunction();
    {
        // TestA a{ 0,1,2 };
        ct::Reflect<TestA>::getPtr(ct::Indexer<0>{});
    }
    {
        using type = ct::FieldGetType<TestA, 0>;
        using test = StaticEqualTypes<typename std::decay<type>::type, float>;
    }
    {
        using type = ct::FieldGetType<TestA, 1>;
        using test = StaticEqualTypes<typename std::decay<type>::type, float>;
    }
    {
        using type = ct::FieldGetType<TestA, 2>;
        using test = StaticEqualTypes<typename std::decay<type>::type, float>;
    }

    /*{
        using type = ct::AccessorType<Inherited, 0>;
        STATIC_EQUAL((std::is_same<type::Get_t, float>::value), true);
        STATIC_EQUAL((std::is_same<ct::GetterType<Inherited, 0>::type, float>::value), true);
    }

    {
        using type = ct::AccessorType<Inherited, 1>;
        STATIC_EQUAL((std::is_same<type::Get_t, float>::value), true);
        STATIC_EQUAL((std::is_same<ct::GetterType<Inherited, 1>::type, float>::value), true);
    }

    {
        using type = ct::AccessorType<Inherited, 2>;
        STATIC_EQUAL((std::is_same<type::Get_t, float>::value), true);
        STATIC_EQUAL((std::is_same<ct::GetterType<Inherited, 2>::type, float>::value), true);
    }
    {
        using type = ct::AccessorType<Inherited, 3>;
        STATIC_EQUAL((std::is_same<type::Get_t, int>::value), true);
        STATIC_EQUAL((std::is_same<ct::GetterType<Inherited, 3>::type, int>::value), true);
    }
    {
        using type = ct::AccessorType<Inherited, 4>;
        STATIC_EQUAL((std::is_same<type::Get_t, double>::value), true);
        STATIC_EQUAL((std::is_same<ct::GetterType<Inherited, 4>::type, double>::value), true);

        // TODO move to hash unit test
        const auto accessor = ct::Reflect<Inherited>::getPtr(ct::Indexer<0>{});
        STATIC_NE(ctv<ct::crc32(ct::Reflect<Inherited>::getName(ct::Indexer<0>{}))>::value, 0);
        STATIC_EQUAL(ct::crc32(ct::Reflect<Inherited>::getName(ct::Indexer<0>{})), ct::crc32("x"));
    }*/
    PrivateGetAndSet pgs;
    pgs.setX(2);
    ct::printStruct(std::cout, pgs);
    std::cout << std::endl;
    mul(pgs);
    if(pgs.getX() != 4.0f)
    {
        std::cout << "Private mutation not working" << std::endl;
        return 1;
    }
    ct::printStruct(std::cout, pgs);
    std::cout << std::endl;
    PrivateMutableAccess pms;
    pms.mutateX() = 2;
    mul(pms);
    ct::printStruct(std::cout, pms);
    std::cout << std::endl;

    {
        const auto accessor = ct::Reflect<TestA>::getPtr(ct::Indexer<3>{});
        const auto start1 = std::chrono::high_resolution_clock::now();

        static_assert(ct::IsMemberObject<TestA, 0>::value, "asdf");
        static_assert(std::is_same<ct::GlobMemberObjectsHelper<TestA, 0, void>::types, ct::VariadicTypedef<float>>::value, "asdf");
        static_assert(std::is_same<ct::GlobMemberObjectsHelper<TestA, 1, void>::types, ct::VariadicTypedef<float, float>>::value, "asdf");
        static_assert(std::is_same<ct::GlobMemberObjectsHelper<TestA, 2, void>::types, ct::VariadicTypedef<float, float, float>>::value, "asdf");
        static_assert(std::is_same<typename ct::GlobMemberObjects<TestA>::types, ct::VariadicTypedef<float, float, float>>::value, "asdf");



        TestA instance;
        for (size_t i = 0; i < 1e9; ++i)
        {
            //accessor.invoke(instance);
        }
        const auto start2 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < 1e9; ++i)
        {
            instance.norm();
        }
        const auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::endl;
        std::cout << "With indirection: " << (start2 - start1).count() << std::endl;
        std::cout << "Direct:           " << (end - start2).count() << std::endl;
        std::cout << "% difference: " << float((start2 - start1).count()) / float((end - start2).count()) << std::endl;
    }
}
