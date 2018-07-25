#pragma once
#include <type_traits>

namespace ct
{
template<class Sig>
struct AccessToken;

template<class T, class T1>
struct AccessToken<void(T::*)(T1)>
{
    AccessToken(T& obj, void(T::*setter)(T1), T1 init): m_data(init), m_obj(obj), m_setter(setter){}
    AccessToken(T& obj, void(T::*setter)(T1)): m_obj(obj), m_setter(setter){}

    ~AccessToken()
    {
        (m_obj.*m_setter)(std::move(m_data));
    }

    operator T1&(){return m_data;}
    template<class AR>
    void load(AR& ar)
    {
        ar(m_data);
    }
private:
    typename std::decay<T1>::type m_data;
    T& m_obj;
    void(T::*m_setter)(T1);
};

template<class T, class T1>
struct AccessToken<void(*)(T&, T1)>
{
    AccessToken(T& obj, void(*setter)(T&, T1), T1 init): m_data(init), m_obj(obj), m_setter(setter){}
    AccessToken(T& obj, void(*setter)(T&, T1)): m_obj(obj), m_setter(setter){}

    ~AccessToken()
    {
        m_setter(m_obj, std::move(m_data));
    }

    operator T1&(){return m_data;}
    template<class AR>
    void load(AR& ar)
    {
        ar(m_data);
    }
private:
    typename std::decay<T1>::type m_data;
    T& m_obj;
    void(*m_setter)(T&, T1);
};

}
