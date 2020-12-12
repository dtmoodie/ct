#ifndef CT_REFLECT_ACCESS_TOKEN_HPP
#define CT_REFLECT_ACCESS_TOKEN_HPP

#include "../type_traits.hpp"

#include <type_traits>
#include <utility>

namespace ct
{
    template <class Sig>
    struct AccessToken;

    template <class T, class T1, class R>
    struct AccessToken<R (T::*)(T1)>
    {
        using type = typename std::decay<T1>::type;
        AccessToken(T& obj, R (T::*setter)(T1), T1 init) : m_data(init), m_obj(obj), m_setter(setter) {}
        AccessToken(T& obj, R (T::*setter)(T1)) : m_obj(obj), m_setter(setter) {}

        ~AccessToken() { (m_obj.*m_setter)(std::move(m_data)); }

        operator type&() { return m_data; }
        type* operator&() { return &m_data; }

        AccessToken& operator=(typename std::decay<T1>::type data)
        {
            m_data = std::move(data);
            return *this;
        }

        template <class AR>
        void load(AR& ar)
        {
            ar(m_data);
        }

      private:
        type m_data;
        T& m_obj;
        R (T::*m_setter)(T1);
    };

    template <class T, class T1, class R>
    struct AccessToken<R (*)(T&, T1)>
    {
        using type = typename std::decay<T1>::type;
        AccessToken(T& obj, R (*setter)(T&, T1), T1 init) : m_data(init), m_obj(obj), m_setter(setter) {}
        AccessToken(T& obj, R (*setter)(T&, T1)) : m_obj(obj), m_setter(setter) {}

        ~AccessToken() { m_setter(m_obj, std::move(m_data)); }

        operator type&() { return m_data; }

        type* operator&() { return &m_data; }

        AccessToken& operator=(typename std::decay<T1>::type data)
        {
            m_data = std::move(data);
            return *this;
        }

        template <class AR>
        void load(AR& ar)
        {
            ar(m_data);
        }

      private:
        type m_data;
        T& m_obj;
        R (*m_setter)(T&, T1);
    };

    template <class T>
    struct ReferenceType<AccessToken<T>>
    {
        using Type = AccessToken<T>;
        using ConstType = const AccessToken<T>;
    };
} // namespace ct
#endif // CT_REFLECT_ACCESS_TOKEN_HPP
