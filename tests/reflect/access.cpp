#include "common.hpp"
#include "../common.hpp"

int main()
{
    {
        TestA a{ 0,1,2 };
        ct::Reflect<TestA>::getAccessor(ct::Indexer<0>{});
    }
    {
        using type = ct::AccessorType<TestA, 0>;
        STATIC_EQUAL((std::is_same<type::GetType, float>::value), true);

    }
    {
        using type = ct::AccessorType<TestA, 1>;
        STATIC_EQUAL((std::is_same<type::GetType, float>::value), true);

    }
    {
        using type = ct::AccessorType<TestA, 2>;
        STATIC_EQUAL((std::is_same<type::GetType, float>::value), true);
    }

    {
        using type = ct::AccessorType<Inherited, 0>;
        STATIC_EQUAL((std::is_same<type::GetType, float>::value), true);
        STATIC_EQUAL((std::is_same<ct::GetterType<Inherited, 0>::type, float>::value), true);
    }

    {
        using type = ct::AccessorType<Inherited, 1>;
        STATIC_EQUAL((std::is_same<type::GetType, float>::value), true);
        STATIC_EQUAL((std::is_same<ct::GetterType<Inherited, 1>::type, float>::value), true);
    }

    {
        using type = ct::AccessorType<Inherited, 2>;
        STATIC_EQUAL((std::is_same<type::GetType, float>::value), true);
        STATIC_EQUAL((std::is_same<ct::GetterType<Inherited, 2>::type, float>::value), true);
    }
    {
        using type = ct::AccessorType<Inherited, 3>;
        STATIC_EQUAL((std::is_same<type::GetType, int>::value), true);
        STATIC_EQUAL((std::is_same<ct::GetterType<Inherited, 3>::type, int>::value), true);
    }
    {
        using type = ct::AccessorType<Inherited, 4>;
        STATIC_EQUAL((std::is_same<type::GetType, double>::value), true);
        STATIC_EQUAL((std::is_same<ct::GetterType<Inherited, 4>::type, double>::value), true);
    }
    
}