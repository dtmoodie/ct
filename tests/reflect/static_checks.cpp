#include <ct/reflect.hpp>
#include "Reflect.hpp"
#include "Data.hpp"

int main()
{
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

}
