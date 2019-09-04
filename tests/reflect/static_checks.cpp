#include "Data.hpp"
#include "Reflect.hpp"
#include <ct/reflect.hpp>
#include <ct/reflect/print.hpp>

#include <iostream>

template <class T, ct::index_t START, ct::index_t END, ct::index_t COUNT>
void checkFieldRange()
{
    ct::StaticEquality<ct::index_t, ct::Reflect<T>::START_INDEX, START>{};
    ct::StaticEquality<ct::index_t, ct::Reflect<T>::END_INDEX, END>{};
    ct::StaticEquality<ct::index_t, ct::Reflect<T>::NUM_FIELDS, COUNT>{};
}

template <class PTR>
void checkWritable(PTR)
{
    static_assert(ct::flags<PTR>() & ct::Flags::WRITABLE, "Field not writable");
}

template <class PTR>
void checkReadable(PTR)
{
    static_assert(ct::flags<PTR>() & ct::Flags::READABLE, "Field not readable");
}

template <class PTR>
void checkInvokable(PTR)
{
    static_assert(ct::flags<PTR>() & ct::Flags::INVOKABLE, "Field not invokable");
}

int main()
{
    static_assert(ct::Has_name<ct::ReflectImpl<ReflectedStruct>>::value, "asdf");

    checkFieldRange<ReflectedStruct, 0, 4, 4>();
    checkWritable(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<0>()));
    checkWritable(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<1>()));
    checkWritable(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<2>()));
    checkWritable(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<3>()));
    checkReadable(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<0>()));
    checkReadable(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<1>()));
    checkReadable(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<2>()));
    checkReadable(ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<3>()));
    checkFieldRange<TestA, 0, 8, 8>();
    checkWritable(ct::Reflect<TestA>::getPtr(ct::Indexer<0>()));
    checkWritable(ct::Reflect<TestA>::getPtr(ct::Indexer<1>()));
    checkWritable(ct::Reflect<TestA>::getPtr(ct::Indexer<2>()));
    checkReadable(ct::Reflect<TestA>::getPtr(ct::Indexer<0>()));
    checkReadable(ct::Reflect<TestA>::getPtr(ct::Indexer<1>()));
    checkReadable(ct::Reflect<TestA>::getPtr(ct::Indexer<2>()));

    checkInvokable(ct::Reflect<TestA>::getPtr(ct::Indexer<3>()));
    checkInvokable(ct::Reflect<TestA>::getPtr(ct::Indexer<4>()));
    checkInvokable(ct::Reflect<TestA>::getPtr(ct::Indexer<5>()));
    checkInvokable(ct::Reflect<TestA>::getPtr(ct::Indexer<6>()));

    checkFieldRange<TestB, 0, 3, 3>();
    checkFieldRange<TestC, 0, 3, 3>();
    checkFieldRange<Inherited, 4, 5, 5>();
    checkFieldRange<Composite, 0, 2, 2>();
    checkFieldRange<MultipleInheritance, 5, 6, 6>();

    ct::StaticEquality<ct::index_t, ct::Reflect<ReflectedStruct>::NUM_FIELDS, 4>();
    ct::StaticEquality<ct::index_t, ct::Reflect<ReflectedStruct>::START_INDEX, 0>();
    ct::StaticEquality<ct::index_t, ct::Reflect<ReflectedStruct>::END_INDEX, 4>();

    ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<0>{});
    ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<1>{});
    ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<2>{});
    ct::Reflect<ReflectedStruct>::getPtr(ct::Indexer<3>{});

    ct::StaticEquality<ct::index_t, ct::Reflect<Inherited>::NUM_FIELDS, 5>();
    ct::StaticEquality<ct::index_t, ct::Reflect<Inherited>::START_INDEX, 4>();
    ct::StaticEquality<ct::index_t, ct::Reflect<Inherited>::END_INDEX, 5>();

    ct::StaticEquality<ct::index_t, ct::Reflect<MultipleInheritance>::NUM_FIELDS, 6>();
    ct::StaticEquality<ct::index_t, ct::Reflect<MultipleInheritance>::START_INDEX, 5>();

    if (ct::crc32(ct::Reflect<ReflectedStruct>::getName()) != ct::crc32("ReflectedStruct"))
    {
        std::cout << "crc32 of ReflectedStruct not working" << std::endl;
    }

    if (ct::crc32(ct::Reflect<Inherited>::getName()) != ct::crc32("Inherited"))
    {
        std::cout << "crc32 of Inherited not working" << std::endl;
    }

    if (ct::crc32(ct::Reflect<MultipleInheritance>::getName()) != ct::crc32("MultipleInheritance"))
    {
        std::cout << "crc32 of MultipleInheritance not working" << std::endl;
    }
#ifdef CT_HAVE_CONSTEXPR_NAME
#if !(defined(_MSC_VER))
    ct::StaticEquality<uint32_t, ct::crc32(ct::Reflect<ReflectedStruct>::getName()), ct::crc32("ReflectedStruct")>{};

    ct::StaticEquality<uint32_t, ct::crc32(ct::Reflect<Inherited>::getName()), ct::crc32("Inherited")>{};

    ct::StaticEquality<uint32_t,
                       ct::crc32(ct::Reflect<MultipleInheritance>::getName()),
                       ct::crc32("MultipleInheritance")>{};
#endif
#endif
    ct::StaticEquality<bool, ct::ConstFunction<Virtual, 0>::value, false>{};
    // ct::StaticEquality<bool, ct::ConstFunction<Virtual, 1>::value, false>{};

    ct::StaticEquality<bool, ct::ShouldSerialize<Virtual, 0>::value, false>{};
    // ct::StaticEquality<bool, ct::ShouldSerialize<Virtual, 1>::value, false>{};

    ct::StaticEqualTypes<typename ct::GlobMemberObjects<ReflectedStruct>::types,
                         ct::VariadicTypedef<float, float, float, int>>{};
    ct::StaticEquality<size_t, ct::GlobMemberObjects<ReflectedStruct>::num, 4>{};

    ct::StaticEqualTypes<typename ct::GlobMemberObjects<Inherited>::types,
                         ct::VariadicTypedef<float, float, float, int, double>>{};
    ct::StaticEquality<size_t, ct::GlobMemberObjects<Inherited>::num, 5>{};

    ct::StaticEqualTypes<typename ct::GlobMemberObjects<TestA>::types, ct::VariadicTypedef<float, float, float>>{};
    ct::StaticEquality<size_t, ct::GlobMemberObjects<TestA>::num, 3>{};

    ct::StaticEqualTypes<typename ct::GlobMemberObjects<DerivedC>::types,
                         ct::VariadicTypedef<float, float, float, float, float, float>>{};
    ct::StaticEquality<size_t, ct::GlobMemberObjects<DerivedC>::num, 6>{};

    static_assert(ct::IsDefaultConstructible<ReflectedStruct>::value, "asdf");

    static_assert(!ct::Has_name<InternallyReflected>::value, "asdf");
    static_assert(!ct::Has_name<ReflectedStruct>::value, "asdf");

    return 0;
}
