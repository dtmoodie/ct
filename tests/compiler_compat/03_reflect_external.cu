#include <ct/reflect.hpp>
#include <ct/reflect_macros.hpp>

struct TestStruct0
{
    float x, y, z;
};

struct TestStruct1
{
    float x, y, z;
};

namespace ct
{
    template <>
    struct ReflectImpl<TestStruct0>
    {
        using DataType = TestStruct0;
        static constexpr ct::StringView getTypeName() { return "TestStruct0"; }
        static constexpr const ct::index_t REFLECT_COUNT_BEGIN = __COUNTER__ + 1;

        constexpr static ct::MemberObjectPointer<float DataType::*, ct::Flags::READABLE | ct::Flags::WRITABLE>
        getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)
        {
            return ct::makeMemberObjectPointer("x", &DataType::x);
        }

        constexpr static ct::MemberObjectPointer<float DataType::*, ct::Flags::READABLE | ct::Flags::WRITABLE>
        getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)
        {
            return ct::makeMemberObjectPointer("y", &DataType::y);
        }

        constexpr static ct::MemberObjectPointer<float DataType::*, ct::Flags::READABLE | ct::Flags::WRITABLE>
        getPtr(const ct::Indexer<__COUNTER__ - REFLECT_COUNT_BEGIN>)
        {
            return ct::makeMemberObjectPointer("z", &DataType::z);
        }

        static constexpr const ct::index_t NUM_FIELDS = __COUNTER__ - REFLECT_COUNT_BEGIN;
    };

    REFLECT_BEGIN(TestStruct1)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
    REFLECT_END;
} // namespace ct

int main() {}