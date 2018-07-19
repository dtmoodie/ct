#include "reflect.hpp"
#include "object.hpp"
#include "equal.hpp"
#include "cerealize.hpp"
#include "print.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

#include <iostream>
#include <cassert>

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
}


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
#endif
}

template<class T>
void test(T& obj)
{
    std::cout << ct::Reflect<T>::getName() << std::endl;
    ct::printStruct(std::cout, obj);
    std::cout << std::endl;
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
    bar.set(4);
    test(bar);
    static_assert(ct::Reflect<Bar>::REFLECTION_COUNT == 2, "Reflect<Bar>::REFLECTION_COUNT == 2");

    TestNonSerizableData non_serializable;
    ct::printStruct<ct::SkipUnprintable>(std::cout, non_serializable);
    static_assert(ct::CanWrite<TestNonSerizableData, 0>::value == false, "");
    static_assert(ct::CanWrite<TestNonSerizableData, 1>::value == true, "");
    std::cout << std::endl;

    #ifdef HAVE_OPENCV
        cv::Rect rect(0,2, 4, 5);
        test(rect);
        static_assert(ct::Reflect<cv::Rect>::REFLECTION_COUNT == 8, "Reflect<cv::Rect>::REFLECTION_COUNT == 8");
    #endif
}
