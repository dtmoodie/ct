#pragma once
#include <utility>
#include <ct/detail/counter.hpp>

template<class T, class T0, class T1>
struct Accessor
{
    Accessor(const char* name, T0(*getter)(const T&), void(*setter)(T&, T1)):
        m_name(name), m_getter(getter), m_setter(setter){}

    const char* getName() const{return m_name;}

    T0 get(const T& obj) const{return m_getter(obj);}

    void set(T& obj, T1&& value) const { m_setter(obj, std::forward(value));}

private:
    const char* m_name;
    T0(*m_getter)(const T&);
    void(*m_setter)(T&, T1);
};
template<class T, class T0>
struct Accessor<T, T0, void>
{
    Accessor(const char* name, T0(*getter)(const T&)):
        m_name(name), m_getter(getter){}

    const char* getName() const{return m_name;}

    T0 get(const T& obj) const{return m_getter(obj);}

private:
    const char* m_name;
    T0(*m_getter)(const T&);
};

template<class T, class T0, class T1>
struct MemberAccessor
{
    MemberAccessor(const char* name, T0(T::*getter)() const, void(T::*setter)(T1)):
        m_name(name), m_getter(getter), m_setter(setter){}

    const char* getName() const{return m_name;}

    T0 get(const T& obj) const{return (obj.*m_getter)();}

    void set(T& obj, T1&& value) const{(obj.*m_setter)(std::forward(value));}

private:
    const char* m_name;
    T0(T::*m_getter)() const;
    void(T::*m_setter)(T1);
};



template<class T, class T0>
struct MemberAccessor<T, T0, void>
{
    MemberAccessor(const char* name, T0(T::*getter)() const):
        m_name(name), m_getter(getter){}

    const char* getName(){return m_name;}

    T0 get(const T& obj) const{(obj.*m_getter)();}

private:
    const char* m_name;
    T0(T::*m_getter)() const;
};

template<class T, class T0, class T1>
Accessor<T, T0, T1> makeAccessor(const char* name, T0(*getter)(const T&), void(*setter)(T&, T1))
{
    return Accessor<T, T0, T1>(name, getter, setter);
}

template<class T, class T0, class T1>
MemberAccessor<T, T0, T1> makeAccessor(const char* name, T0(T::*getter)()const, void(T::*setter)(T1))
{
    return MemberAccessor<T, T0, T1>(name, getter, setter);
}

template<class T, class T0>
MemberAccessor<T, T0, void> makeAccessor(const char* name, T0(T::*getter)()const)
{
    return MemberAccessor<T, T0, void>(name, getter);
}

template<class T>
struct Reflect;

#define REFLECT_BEGIN(TYPE) \
    template<> struct Reflect<TYPE>{ \
        using DataType = TYPE; \
        static constexpr const char* getName(){return #TYPE;} \
        static constexpr int REFLECT_COUNT_START = __COUNTER__;

#define PUBLIC_ACCESS(NAME) \
    static Accessor<DataType, const decltype(DataType::NAME)&, decltype(DataType::NAME)&&> getAccessor(ct::_counter_<__COUNTER__ - REFLECT_COUNT_START - 1>){\
        return makeAccessor(#NAME, \
            +[](const DataType& obj)-> const decltype(DataType::NAME)&{return obj.NAME;  }, \
            +[](DataType& obj, decltype(DataType::NAME)&& value)->void {obj.NAME = value; });}

#define ACCESSOR(NAME, GETTER, SETTER) \
    static auto getAccessor(ct::_counter_<__COUNTER__ - REFLECT_COUNT_START - 1>) -> decltype(makeAccessor(#NAME, GETTER, SETTER)) { return makeAccessor(#NAME, GETTER, SETTER); }

#define MEMBER_FUNCTION(NAME, FPTR) \
    static auto getAccessor(ct::_counter_<__COUNTER__ - REFLECT_COUNT_START - 1>) -> decltype(makeAccessor(#NAME, FPTR)) { return makeAccessor(#NAME, FPTR); }

#define REFLECT_END \
    static constexpr const int REFLECT_COUNT_END = __COUNTER__; \
    static constexpr const int REFLECT_COUNTER = REFLECT_COUNT_END - REFLECT_COUNT_START; \
    }




