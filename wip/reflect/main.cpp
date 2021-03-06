#include "equal.hpp"
#include "object.hpp"
#include "reflect.hpp"

#include "print.hpp"

#ifdef HAVE_CEREAL
#include "cerealize.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#endif
#include <cassert>
#include <iostream>

namespace ct
{
    REFLECT_BEGIN(Foo)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(z)
    REFLECT_END;

    REFLECT_BEGIN(Bar)
        ACCESSOR(data, &Bar::get, &Bar::set)
        MEMBER_FUNCTION(half, &Bar::half)
    REFLECT_END;
    REFLECT_BEGIN(TestNonSerizableData)
        PUBLIC_ACCESS(a)
        PUBLIC_ACCESS(b)
    REFLECT_END;

    REFLECT_DERIVED(DerivedFoo, Foo)
        PUBLIC_ACCESS(id)
    REFLECT_END;

} // namespace ct

#ifdef HAVE_OPENCV
#include <opencv2/core/types.hpp>
namespace ct
{
    REFLECT_BEGIN(cv::Point)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
    REFLECT_END;

    REFLECT_BEGIN(cv::Size)
        PUBLIC_ACCESS(width)
        PUBLIC_ACCESS(height)
    REFLECT_END;

    REFLECT_BEGIN(cv::Rect)
        PUBLIC_ACCESS(x)
        PUBLIC_ACCESS(y)
        PUBLIC_ACCESS(width)
        PUBLIC_ACCESS(height)
        MEMBER_FUNCTION(tl, &cv::Rect::tl)
        MEMBER_FUNCTION(br, &cv::Rect::br)
        MEMBER_FUNCTION(area, &cv::Rect::area)
        MEMBER_FUNCTION(size, &cv::Rect::size)
    REFLECT_END;
} // namespace ct
#endif

template <class T>
void test(T& obj)
{
    std::cout << ct::Reflect<T>::getTypeName() << std::endl;
    ct::printStruct(std::cout, obj);
    std::cout << std::endl;
#ifdef HAVE_CEREAL
    std::cout << "JSON: " << std::endl;
    {
        cereal::JSONOutputArchive ar(std::cout);
        ct::saveStruct(ar, obj);
    }
    std::cout << std::endl;
    std::cout << "Binary: " << std::endl;
    {
        {
            cereal::BinaryOutputArchive ar(std::cout);
            ct::saveStruct(ar, obj);
        }
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ct::saveStruct(ar, obj);
        }
        {
            cereal::BinaryInputArchive ar(ss);
            T loaded_object;
            ct::loadStruct(ar, loaded_object);
            std::cout << std::endl;
            assert(ct::equal(obj, loaded_object));
        }
    }
#endif
}

int main()
{
    Foo obj;
    obj.x = 0;
    obj.y = 1;
    obj.z = 2;
    test(obj);
    static_assert(ct::Reflect<Foo>::REFLECTION_COUNT == 3, "Reflect<Foo>::REFLECTION_COUNT == 3");
    std::cout << std::endl;

    Bar bar;
    test(bar);
    static_assert(ct::Reflect<Bar>::REFLECTION_COUNT == 2, "Reflect<Bar>::REFLECTION_COUNT == 2");

    TestNonSerizableData non_serializable;
    ct::printStruct<ct::SkipUnprintable>(std::cout, non_serializable);
    static_assert(ct::detail::stream_writable<NonSerializable>::value == false,
                  "ct::detail::stream_writable<NonSerializable>::value == false");
    static_assert(ct::detail::stream_writable<float>::value, "ct::detail::stream_writable<float>::value");
    static_assert(ct::CanWrite<TestNonSerizableData, 0>::value == false,
                  "ct::CanWrite<TestNonSerizableData, 0>::value == false");
    static_assert(ct::CanWrite<TestNonSerizableData, 1>::value, "ct::CanWrite<TestNonSerizableData, 1>::value == true");
    std::cout << std::endl;

#ifdef HAVE_OPENCV
    cv::Rect rect(0, 2, 4, 5);
    test(rect);
    static_assert(ct::Reflect<cv::Rect>::REFLECTION_COUNT == 8, "Reflect<cv::Rect>::REFLECTION_COUNT == 8");
#endif
    DerivedFoo derived_foo;
    test(derived_foo);
    static_assert(ct::Reflect<DerivedFoo>::REFLECTION_COUNT == 4, "");
    ct::Reflect<DerivedFoo>::getAccessor(ct::_counter_<0>{});
    std::cout << ct::Reflect<DerivedFoo>::REFLECTION_COUNT << std::endl;
}
