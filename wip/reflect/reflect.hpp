#pragma once
#include <utility>
#include <ct/detail/counter.hpp>

namespace ct
{
    template<class T, class T1>
    struct AccessToken
    {
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
        T1 m_data;
        T& m_obj;
        void(T::*m_setter)(T1);
    };

    template<class T>
    struct Getter;

    struct DefaultGetterTraits{};
    struct DefaultSetterTraits{};

    struct CalculatedValue{};


    template<class T, class D>
    struct Getter<D(T::*)() const>
    {
        using GetType = typename std::decay<D>::type;
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
        using GetType = typename std::decay<D>::type;
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

    template<class T0, class T1, class GetterTraits = DefaultGetterTraits, class SetterTraits = DefaultSetterTraits>
    struct Accessor: public Getter<T0>, public Setter<T1>
    {
        using GetterTraits_t = GetterTraits;
        using SetterTraits_t = SetterTraits;

        Accessor(const char* name, T0 getter, T1 setter):
            m_name(name), Getter<T0>(getter), Setter<T1>(setter){}
        const char* getName() const{return m_name;}
    private:
        const char* m_name;
    };

    template<class T0, class GetterTraits>
    struct Accessor<T0, void, GetterTraits, void>: public Getter<T0>
    {
        using GetterTraits_t = GetterTraits;
        using SetterTraits_t = void;
        using SetType = void;

        Accessor(const char* name, T0 getter):
            m_name(name), Getter<T0>(getter){}
        const char* getName() const{return m_name;}
    private:
        const char* m_name;
    };

    template<class GetterTraits = DefaultGetterTraits, class SetterTraits = DefaultSetterTraits, class T0, class T1>
    Accessor<T0, T1, GetterTraits, SetterTraits> makeAccessor(const char* name, T0 getter, T1 setter)
    {
        return Accessor<T0, T1, GetterTraits, SetterTraits>(name, getter, setter);
    }

    template<class GetterTraits = DefaultGetterTraits, class T0>
    Accessor<T0, void, GetterTraits, void> makeAccessor(const char* name, T0 getter)
    {
        return Accessor<T0, void, GetterTraits, void>(name, getter);
    }

    template<class T>
    struct Reflect{static const bool SPECIALIZED = false;};

    template<class T, class U = void>
    using enable_if_reflected = typename std::enable_if<Reflect<T>::SPECIALIZED, U>::type;
}

#define REFLECT_BEGIN(TYPE) \
    template<> struct Reflect<TYPE>{ \
        using DataType = TYPE; \
        static constexpr const char* getName(){return #TYPE;} \
        static const bool SPECIALIZED = true; \
        static const int I0 = 0; \
        static constexpr int REFLECT_COUNT_START = __COUNTER__;

#define REFLECT_DERIVED(TYPE, BASE) \
    template<> struct Reflect<TYPE>: private Reflect<BASE>{ \
        using DataType = TYPE; \
        static constexpr const char* getName(){return #TYPE;} \
        static const bool SPECIALIZED = true; \
        static constexpr int I0 = Reflect<BASE>::REFLECTION_COUNT; \
        static constexpr int REFLECT_COUNT_START = __COUNTER__; \
        template<int I> \
        static auto getAccessor(ct::_counter_<I> idx) -> typename std::enable_if<I >= 0 && I < Reflect<BASE>::REFLECTION_COUNT, decltype(Reflect<BASE>::getAccessor(idx))>::type \
            {return Reflect<BASE>::getAccessor(idx);}


#define PUBLIC_ACCESS(NAME) \
    static ct::Accessor<const decltype(DataType::NAME)&(*)(const DataType&), decltype(DataType::NAME)&(*)(DataType&)> \
        getAccessor(ct::_counter_<I0 + __COUNTER__ - REFLECT_COUNT_START - 1>){\
            return {#NAME, \
                [](const DataType& obj)-> const decltype(DataType::NAME)&{return obj.NAME;  }, \
                [](DataType& obj)-> decltype(DataType::NAME)&{ return obj.NAME; }};}

#define ACCESSOR(NAME, GETTER, SETTER) \
    static auto getAccessor(ct::_counter_<I0 + __COUNTER__ - REFLECT_COUNT_START - 1>) -> decltype(ct::makeAccessor(#NAME, GETTER, SETTER)) { return ct::makeAccessor(#NAME, GETTER, SETTER); }

#define MEMBER_FUNCTION(NAME, FPTR) \
    static auto getAccessor(ct::_counter_<I0 + __COUNTER__ - REFLECT_COUNT_START - 1>) -> decltype(ct::makeAccessor<CalculatedValue>(#NAME, FPTR)) { return ct::makeAccessor<CalculatedValue>(#NAME, FPTR); }

#define REFLECT_END \
    static constexpr const int REFLECT_COUNT_END = __COUNTER__; \
    static constexpr const int REFLECTION_COUNT = I0 + REFLECT_COUNT_END - REFLECT_COUNT_START - 1; \
    static constexpr ct::_counter_<REFLECTION_COUNT-1> end(){return ct::_counter_<REFLECTION_COUNT-1>{};} \
    }