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

int main()
{
    checkFieldRange<ReflectedStruct, 0, 4, 4>();
    checkFieldRange<TestA, 0, 8, 8>();
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

    ct::StaticEquality<uint32_t, ct::crc32(ct::Reflect<ReflectedStruct>::getName()), ct::crc32("ReflectedStruct")>{};

    ct::StaticEquality<uint32_t, ct::crc32(ct::Reflect<Inherited>::getName()), ct::crc32("Inherited")>{};

    ct::StaticEquality<uint32_t,
                       ct::crc32(ct::Reflect<MultipleInheritance>::getName()),
                       ct::crc32("MultipleInheritance")>{};

    ct::StaticEquality<bool, ct::ConstFunction<Virtual, 0>::value, false>{};
    ct::StaticEquality<bool, ct::ConstFunction<Virtual, 1>::value, false>{};

    ct::StaticEquality<bool, ct::ShouldSerialize<Virtual, 0>::value, false>{};
    ct::StaticEquality<bool, ct::ShouldSerialize<Virtual, 1>::value, false>{};

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
    return 0;
}
