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

template<class AR, class T, int I>
auto loadValue(AR& ar, T& obj) -> typename std::enable_if<!std::is_same<typename decltype(Reflect<T>::getAccessor(ct::_counter_<I>{}))::SetType, void>::value>::type
{
    auto accessor = Reflect<T>::getAccessor(ct::_counter_<I>{});
    ar(cereal::make_nvp(accessor.getName(), static_cast<typename decltype(accessor)::SetType&>(accessor.set(obj))));
}

template<class AR, class T, int I>
auto loadValue(AR&, T&) -> typename std::enable_if<std::is_same<typename decltype(Reflect<T>::getAccessor(ct::_counter_<I>{}))::SetType, void>::value>::type
{

}

template<class AR, class T>
void loadStructHelper(AR& ar,  T& obj, ct::_counter_<0>)
{
    loadValue<AR, T, 0>(ar, obj);
}

template<int I,  class AR, class T>
void loadStructHelper(AR& ar,  T&obj, ct::_counter_<I> idx)
{
    loadStructHelper(ar, obj, --idx);
    loadValue<AR, T, I>(ar, obj);
}

template<class AR, class T>
void loadStruct(AR& ar, T& obj)
{
    loadStructHelper(ar, obj, Reflect<T>::end());
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
    std::cout << std::endl;
    std::cout << "Binary: " << std::endl;
    {
        {
            cereal::BinaryOutputArchive ar(std::cout);
            saveStruct(ar, obj);
        }
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            saveStruct(ar, obj);
        }
        {
            cereal::BinaryInputArchive ar(ss);
            T loaded_object;
            loadStruct(ar, loaded_object);
            std::cout << std::endl;
            printStruct(std::cout, loaded_object);
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
    static_assert(Reflect<Foo>::REFLECTION_COUNT == 3, "Reflect<Foo>::REFLECTION_COUNT == 3");
    std::cout << std::endl;

    Bar bar;
    bar.set(4);
    test(bar);
    static_assert(Reflect<Bar>::REFLECTION_COUNT == 2, "Reflect<Bar>::REFLECTION_COUNT == 2");


}
