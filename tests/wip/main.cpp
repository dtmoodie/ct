#include "reflect.hpp"
#include "object.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

#include <iostream>

REFLECT_BEGIN(Foo)
    PUBLIC_ACCESS(x)
    PUBLIC_ACCESS(y)
    PUBLIC_ACCESS(z)
REFLECT_END;

REFLECT_BEGIN(Bar)
    ACCESSOR(data, &Bar::get, &Bar::set)
    MEMBER_FUNCTION(half, &Bar::half)
REFLECT_END;

struct PrintOptions
{
    static const bool print_name = true;
    static const char name_separator = ':';
    static const char value_separator = ' ';
};

template<int I, class Options = PrintOptions, class T>
void printValue(std::ostream& os, const T& obj)
{
    auto accessor = Reflect<T>::getAccessor(ct::_counter_<I>{});
    if(Options::print_name)
    {
        os << accessor.getName() << Options::name_separator;
    }
    os << accessor.get(obj);
}

template<class T, class Options = PrintOptions>
void printStructHelper(std::ostream& os, const T& obj, ct::_counter_<0>)
{
    printValue<0, Options>(os, obj);
}

template<int I, class Options = PrintOptions, class T>
void printStructHelper(std::ostream& os, const T& obj, ct::_counter_<I> idx)
{
    printStructHelper(os, obj, --idx);
    os << Options::value_separator;
    printValue<I>(os, obj);
}

template<class T, class Options = PrintOptions>
void printStruct(std::ostream& os, const T& obj)
{
    printStructHelper(os, obj, Reflect<T>::end());
}

template<class AR, class T>
void saveStructHelper(AR& ar, const T& obj, ct::_counter_<0> idx)
{
    auto accessor = Reflect<T>::getAccessor(idx);
    ar(cereal::make_nvp(accessor.getName(), accessor.get(obj)));
}

template<int I, class AR, class T>
void saveStructHelper(AR& ar, const T& obj, ct::_counter_<I> idx)
{
    saveStructHelper(ar, obj, --idx);
    auto accessor = Reflect<T>::getAccessor(idx);
    ar(cereal::make_nvp(accessor.getName(), accessor.get(obj)));
}

template<class AR, class T>
void saveStruct(AR& ar, const T& obj)
{
    saveStructHelper(ar, obj, Reflect<T>::end());
}

template<class AR, class T>
void loadStructHelper(AR& ar,  T&, ct::_counter_<0> idx)
{
    auto accessor = Reflect<T>::getAccessor(idx);

}

template<int I,  class AR, class T>
void loadStructHelper(AR& ar,  T&, ct::_counter_<I> idx)
{
    auto accessor = Reflect<T>::getAccessor(idx);

}

template<class AR, class T>
void loadStruct(AR& ar, T& obj)
{

}

template<class T>
void test(T& obj)
{
    std::cout << Reflect<T>::getName() << std::endl;
    printStruct(std::cout, obj);
    std::cout << std::endl;
    std::cout << "JSON: " << std::endl;
    {
        cereal::JSONOutputArchive ar(std::cout);
        saveStruct(ar, obj);
    }
    std::cout << "Binary: " << std::endl;
    std::cout << std::endl;
    {
        cereal::BinaryOutputArchive ar(std::cout);
        saveStruct(ar, obj);
    }
}

int main()
{
    Foo obj;
    obj.x = 0;
    obj.y = 1;
    obj.z = 2;
    test(obj);
    static_assert(Reflect<Foo>::REFLECTION_COUNT == 3, "Reflect<Foo>::REFLECTION_COUNT == 3");
    std::cout << std::endl;

    Bar bar;
    bar.set(4);
    test(bar);
    static_assert(Reflect<Bar>::REFLECTION_COUNT == 2, "Reflect<Bar>::REFLECTION_COUNT == 2");


}
