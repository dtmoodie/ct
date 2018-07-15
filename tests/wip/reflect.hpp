#pragma once
#include <utility>
#include <ct/detail/counter.hpp>

template<class T, class T1>
struct AccessToken
{
    AccessToken(T& obj, void(T::*setter)(T1)): m_obj(obj), m_setter(setter){}

    ~AccessToken(){(m_obj.*m_setter)(std::move(m_data));}

    operator T1&(){return m_data;}
    template<class AR>
    void load(AR& ar)
    {
        ar(m_data);
    }
private:
    T1 m_data;
    T& m_obj;
    void(T::*m_setter)(T1);
};

template<class T>
struct Getter;

template<class T, class D>
struct Getter<D(T::*)() const>
{
    Getter(D(T::*getter)() const): m_getter(getter){}

    D get(const T& obj) const
    {
        return (obj.*m_getter)();
    }
private:
    D(T::*m_getter)() const;
};

template<class T, class D>
struct Getter<D(*)(const T&)>
{
    Getter(D(*getter)(const T&)): m_getter(getter){}
    D get(const T& obj) const
    {
        return m_getter(obj);
    }
private:
    D(*m_getter)(const T&);
};

template<class T>
struct Setter;

template<class T, class D>
struct Setter<void(T::*)(D)>
{
    using SetType = D;
    Setter(void(T::*setter)(D)):m_setter(setter){}

    void set(T& obj, D&& data)
    {
        (obj.*m_setter)(std::forward(data));
    }
    AccessToken<T, D> set(T& obj)
    {
        return AccessToken<T, D>(obj, m_setter);
    }

private:
    void(T::*m_setter)(D);
};

template<class T, class D>
struct Setter<D&(T::*)()>
{
    using SetType = D;
    Setter(D&(T::*setter)()): m_setter(setter){}

    void set(T& obj, D&& data)
    {
        (obj.*m_setter)() = std::move(data);
    }
    D& set(T& obj)
    {
        return (obj.*m_setter)();
    }
private:
    D&(T::*m_setter)();
};

template<class T, class D>
struct Setter<D&(*)(T&)>
{
    using SetType = D;
    Setter(D&(*setter)(T&)): m_setter(setter){}

    void set(T& obj, D&& data)
    {
        m_setter(obj) = std::move(data);
    }
    D& set(T& obj)
    {
        return m_setter(obj);
    }
private:
    D&(*m_setter)(T&);
};

template<class T, class D>
struct Setter<void(*)(T&, D)>
{
    using SetType = D;
    Setter(void(*setter)(T&, D)):m_setter(setter){}

    void set(T& obj, D&& data)
    {
        m_setter(obj, std::forward(data));
    }
private:
    void(*m_setter)(T&, D);
};

template<class T0, class T1>
struct Accessor: public Getter<T0>, public Setter<T1>
{
    Accessor(const char* name, T0 getter, T1 setter):
        m_name(name), Getter<T0>(getter), Setter<T1>(setter){}
    const char* getName() const{return m_name;}
private:
    const char* m_name;
};

template<class T0>
struct Accessor<T0, void>: public Getter<T0>
{
    using SetType = void;
    Accessor(const char* name, T0 getter):
        m_name(name), Getter<T0>(getter){}
    const char* getName() const{return m_name;}
private:
    const char* m_name;
};

template<class T0, class T1>
Accessor<T0, T1> makeAccessor(const char* name, T0 getter, T1 setter)
{
    return Accessor<T0, T1>(name, getter, setter);
}

template<class T0>
Accessor<T0, void> makeAccessor(const char* name, T0 getter)
{
    return Accessor<T0, void>(name, getter);
}

template<class T>
struct Reflect;

#define REFLECT_BEGIN(TYPE) \
    template<> struct Reflect<TYPE>{ \
        using DataType = TYPE; \
        static constexpr const char* getName(){return #TYPE;} \
        static constexpr int REFLECT_COUNT_START = __COUNTER__;

#define PUBLIC_ACCESS(NAME) \
    static Accessor<const decltype(DataType::NAME)&(*)(const DataType&), decltype(DataType::NAME)&(*)(DataType&)> getAccessor(ct::_counter_<__COUNTER__ - REFLECT_COUNT_START - 1>){\
        return makeAccessor(#NAME, \
            +[](const DataType& obj)-> const decltype(DataType::NAME)&{return obj.NAME;  }, \
            +[](DataType& obj)-> decltype(DataType::NAME)&{ return obj.NAME; });}

#define ACCESSOR(NAME, GETTER, SETTER) \
    static auto getAccessor(ct::_counter_<__COUNTER__ - REFLECT_COUNT_START - 1>) -> decltype(makeAccessor(#NAME, GETTER, SETTER)) { return makeAccessor(#NAME, GETTER, SETTER); }

#define MEMBER_FUNCTION(NAME, FPTR) \
    static auto getAccessor(ct::_counter_<__COUNTER__ - REFLECT_COUNT_START - 1>) -> decltype(makeAccessor(#NAME, FPTR)) { return makeAccessor(#NAME, FPTR); }

#define REFLECT_END \
    static constexpr const int REFLECT_COUNT_END = __COUNTER__; \
    static constexpr const int REFLECTION_COUNT = REFLECT_COUNT_END - REFLECT_COUNT_START - 1; \
    static constexpr ct::_counter_<REFLECTION_COUNT-1> end(){return ct::_counter_<REFLECTION_COUNT-1>{};} \
    }




