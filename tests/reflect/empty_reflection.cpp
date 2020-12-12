// Here we replicate a class structure that I am having issues with in a different library
#include <ct/reflect.hpp>
#include <ct/reflect/print.hpp>
#include <ct/reflect_macros.hpp>
#include <gtest/gtest.h>

struct IObject
{
};

struct IMetaObject : IObject
{
    REFLECT_INTERNAL_DERIVED(IMetaObject, IObject)
    REFLECT_INTERNAL_END;
};

struct MetaObject : IMetaObject
{
    const void* getStream() const { return nullptr; }
    void setStream(const void*) {}

    REFLECT_INTERNAL_DERIVED(MetaObject, IMetaObject)
        PROPERTY(stream, &MetaObject::getStream, &MetaObject::setStream)
    REFLECT_INTERNAL_END;
};

struct MetaObjectSlots : MetaObject
{
    void test_void() {}
    void test_int(int) {}

    REFLECT_INTERNAL_DERIVED(MetaObjectSlots, MetaObject)
        MEMBER_FUNCTION(test_void)
        MEMBER_FUNCTION(test_int)
    REFLECT_INTERNAL_END;
};

namespace ct
{
    REFLECT_BEGIN(IObject)
    REFLECT_END;
} // namespace ct

TEST(empty_reflection, print)
{
    // auto ptr = ct::Reflect<IObject>::getPtr(ct::Indexer<0>());
    MetaObjectSlots obj;
    std::cout << obj << std::endl;
}