#include <ct/enum.hpp>

enum MyEnum : uint32_t
{
    kVALUE0 = 0,
    kVALUE1 = 1
};

namespace ct
{
    template <>
    struct ReflectImpl<MyEnum>
    {
        static constexpr ct::EnumField<ct::EnumValue<MyEnum, uint32_t, MyEnum::kVALUE0, 0>> getPtr(ct::Indexer<0>)
        {
            return ct::makeEnumField<ct::EnumValue<MyEnum, uint32_t, MyEnum::kVALUE0, 0>>("kVALUE0");
        }

        static constexpr ct::EnumField<ct::EnumValue<MyEnum, uint32_t, MyEnum::kVALUE1, 1>> getPtr(ct::Indexer<1>)
        {
            return ct::makeEnumField<ct::EnumValue<MyEnum, uint32_t, MyEnum::kVALUE1, 1>>("kVALUE1");
        }

        static constexpr const index_t NUM_FIELDS = 2;
    };
} // namespace ct

int main() {}