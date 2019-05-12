#pragma once
#include <ct/Indexer.hpp>
#include <ct/enum.hpp>
#include <ct/reflect.hpp>
#include <ct/reflect/print.hpp>

struct MyClass
{
    ENUM_START(MyEnum, uint32_t)
        ENUM_VALUE(kVALUE0, 0)
        ENUM_VALUE(kVALUE1, 1)
        ENUM_VALUE(kVALUE2, 2)
        ENUM_VALUE(kVALUE3, 3)
    ENUM_END;

    ENUM_START(SecondEnum, uint8_t)
        ENUM_VALUE(kBGR, 0)
        ENUM_VALUE(kRGB, kBGR + 1)
        ENUM_VALUE(kHSV, kRGB + 1)
        ENUM_VALUE(kYUV, kHSV + 1)
        ENUM_VALUE(kHSL, kYUV + 1)
    ENUM_END;

    ENUM_START(BitwiseEnum, uint8_t)
        ENUM_VALUE(kVALUE0, 1)
        ENUM_VALUE(kVALUE1, 8)
        ENUM_VALUE(kVALUE2, 16)
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
        static constexpr const bool SPECIALIZED = true;
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
}
#endif
