#pragma once
#include <ct/EnumBitset.hpp>
#include <ct/Indexer.hpp>
#include <ct/enum.hpp>
#include <ct/reflect.hpp>
#include <ct/reflect/print.hpp>

struct MyClass
{
    ENUM_BEGIN(MyEnum, uint32_t)
        ENUM_VALUE(kVALUE0, 1)
        ENUM_VALUE(kVALUE1, 2)
        ENUM_VALUE(kVALUE2, 3)
        ENUM_VALUE(kVALUE3, 4)
    ENUM_END;

    ENUM_BEGIN(SecondEnum, uint8_t)
        ENUM_VALUE(kBGR, 0)
        ENUM_VALUE(kRGB, kBGR + 1)
        ENUM_VALUE(kHSV, kRGB + 1)
        ENUM_VALUE(kYUV, kHSV + 1)
        ENUM_VALUE(kHSL, kYUV + 1)
    ENUM_END;

    BITSET_BEGIN(BitwiseEnum)
        ENUM_BITVALUE(kVALUE0, 0)
        ENUM_BITVALUE(kVALUE1, 1)
        ENUM_BITVALUE(kVALUE2, 2)
    ENUM_END;

    BITSET_BEGIN(ExtendedEnum)
        ENUM_BITVALUE(kVALUE3, ct::index(BitwiseEnum::kVALUE2) + 1)
        ENUM_BITVALUE(kVALUE4, ct::index(kVALUE3) + 1)
    ENUM_END;

    BITSET_BEGIN(Bitset)
        ENUM_BITVALUE(v0, 0)
        ENUM_BITVALUE(v1, 1)
        ENUM_BITVALUE(v2, 2)
        ENUM_BITVALUE(v3, 3)
        ENUM_BITVALUE(v4, 4)
        ENUM_BITVALUE(v5, 5)
    ENUM_END;

    ENUM_BEGIN(MixedBitwise, uint64_t)
        ENUM_VALUE(kVALUE0, 1)
        ENUM_VALUE(kVALUE1, 2)
        ENUM_VALUE(kVALUE2, 3)
        ENUM_VALUE(kVALUE3, 4)
        ENUM_VALUE(kVALUE4, 5)
        ENUM_VALUE(kVALUE5, 6)
        ENUM_VALUE(kVALUE6, 7)

        ENUM_BITVALUE(kFLAG0, 4)
        ENUM_BITVALUE(kFLAG1, 5)
        ENUM_BITVALUE(kFLAG2, 6)
    ENUM_END;

    enum StandardEnum : uint32_t
    {
        k0 = 0,
        k1,
        k2,
        k3
    };

    enum StandardEnum2 : int32_t
    {
        kASDF,
        k1234
    };
};

#ifndef __NVCC__
namespace ct
{
    template <>
    struct ReflectImpl<MyClass::StandardEnum>
    {
        static constexpr auto getPtr(ct::Indexer<0>)
        {
            return ct::makeEnumField<ct::EnumValue<MyClass::StandardEnum, uint32_t, MyClass::StandardEnum::k0, 0>>(
                "k0");
        }
        static constexpr auto getPtr(ct::Indexer<1>)
        {
            return ct::makeEnumField<ct::EnumValue<MyClass::StandardEnum, uint32_t, MyClass::StandardEnum::k1, 1>>(
                "k1");
        }
        static constexpr auto getPtr(ct::Indexer<2>)
        {
            return ct::makeEnumField<ct::EnumValue<MyClass::StandardEnum, uint32_t, MyClass::StandardEnum::k2, 2>>(
                "k2");
        }
        static constexpr auto getPtr(ct::Indexer<3>)
        {
            return ct::makeEnumField<ct::EnumValue<MyClass::StandardEnum, uint32_t, MyClass::StandardEnum::k3, 3>>(
                "k3");
        }
        static constexpr const index_t NUM_FIELDS = 4;
    };

    REFLECT_BEGIN(MyClass::StandardEnum2)
        ENUM(kASDF)
        ENUM(k1234)
    REFLECT_END;
} // namespace ct
#endif
