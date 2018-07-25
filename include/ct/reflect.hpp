#pragma once
#include <ct/Indexer.hpp>
#include <ct/Hash.hpp>
#include <utility>
#include <cstdint>

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

        constexpr Getter(D(T::*getter)() const): m_getter(getter){}

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

        constexpr Getter(D(*getter)(const T&)): m_getter(getter){}
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
        constexpr Setter(void(T::*setter)(D)):m_setter(setter){}

        void set(T& obj, D&& data) const
        {
            (obj.*m_setter)(std::forward(data));
        }

        AccessToken<T, D> set(T& obj) const
        {
            return AccessToken<T, D>(obj, m_setter);
        }

    private:
        void(T::*m_setter)(D);
    };

    template<class T, class D>
    struct Setter<D&(T::*)()>
    {
        using SetType = D&;
        constexpr Setter(D&(T::*setter)()): m_setter(setter){}

        void set(T& obj, D&& data) const
        {
            (obj.*m_setter)() = std::move(data);
        }

        D& set(T& obj) const
        {
            return (obj.*m_setter)();
        }
    private:
        D&(T::*m_setter)();
    };

    template<class T, class D>
    struct Setter<D&(*)(T&)>
    {
        using SetType = D&;

        constexpr Setter(D&(*setter)(T&)): m_setter(setter){}

        constexpr void set(T& obj, D&& data) const
        {
            m_setter(obj) = std::move(data);
        }

        constexpr D& set(T& obj) const
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

        constexpr Setter(void(*setter)(T&, D)):m_setter(setter){}

        constexpr void set(T& obj, D&& data) const
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

        constexpr Accessor(T0 getter, T1 setter):
            Getter<T0>(getter), Setter<T1>(setter){}
    };

    template<class T0, class GetterTraits>
    struct Accessor<T0, void, GetterTraits, void>: public Getter<T0>
    {
        using GetterTraits_t = GetterTraits;
        using SetterTraits_t = void;
        using SetType = void;

        constexpr Accessor(T0 getter):
            Getter<T0>(getter){}
    };

    template<class GetterTraits = DefaultGetterTraits, class SetterTraits = DefaultSetterTraits, class T0, class T1>
    constexpr Accessor<T0, T1, GetterTraits, SetterTraits> makeAccessor(T0 getter, T1 setter)
    {
        return Accessor<T0, T1, GetterTraits, SetterTraits>(getter, setter);
    }

    template<class GetterTraits = DefaultGetterTraits, class T0>
    constexpr Accessor<T0, void, GetterTraits, void> makeAccessor(T0 getter)
    {
        return Accessor<T0, void, GetterTraits, void>(getter);
    }

    template<class T>
    struct Reflect{static const bool SPECIALIZED = false;};

    template<class T, class U = void>
    using enable_if_reflected = typename std::enable_if<Reflect<T>::SPECIALIZED, U>::type;

    template<class T, class U = void>
    using enable_if_not_reflected = typename std::enable_if<!Reflect<T>::SPECIALIZED, U>::type;


    template<class T, index_t I>
    using AccessorType = decltype(ct::Reflect<T>::getAccessor(Indexer<I>{}));

    template<class T, index_t I>
    struct GetterType
    {
        using accessor_type = AccessorType<T, I>;
        using type = typename accessor_type::GetType;
    };

    template<class T, index_t I>
    struct SetterType
    {
        using accessor_type = AccessorType<T, I>;
        using type = typename accessor_type::SetType;
    };

    template<class T, index_t I>
    struct GetterTraits
    {
        using accessor_type = AccessorType<T, I>;
        using type = typename accessor_type::GetterTraits_t;
    };
    template<index_t I, class T>
    constexpr const char* getName()
    {
        return Reflect<T>::getName(ct::Indexer<I>{});
    }

    template<index_t I, class T>
    typename GetterType<T, I>::type get(const T& obj)
    {
        auto accessor = Reflect<T>::getAccessor(Indexer<I>{});
        return accessor.get(obj);
    }

    template<index_t I, class T>
    typename SetterType<T, I>::type set(T& obj)
    {
        auto accessor = Reflect<T>::getAccessor(Indexer<I>{});
        return accessor.set(obj);
    }
}

#define REFLECT_BEGIN(TYPE) \
    template<> struct Reflect<TYPE>{ \
        using DataType = TYPE; \
        static constexpr const char* getName(){return #TYPE;} \
        static constexpr const bool SPECIALIZED = true; \
        static constexpr const index_t I0 = 0; \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;

#define REFLECT_DERIVED(TYPE, BASE) \
    template<> struct Reflect<TYPE>: private Reflect<BASE>{ \
        using DataType = TYPE; \
        static constexpr const char* getName(){return #TYPE;} \
        static constexpr const bool SPECIALIZED = true; \
        static constexpr const ct::index_t I0 = Reflect<BASE>::REFLECTION_COUNT; \
        static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__; \
        template<ct::index_t I> \
        static constexpr auto getAccessor(const ct::Indexer<I> idx) -> typename std::enable_if<I >= 0 && I < Reflect<BASE>::REFLECTION_COUNT, decltype(Reflect<BASE>::getAccessor(idx))>::type \
            {return Reflect<BASE>::getAccessor(idx);} \
        template<ct::index_t I> \
        static constexpr auto getName(const ct::Indexer<I> idx) -> typename std::enable_if<I >= 0 && I < Reflect<BASE>::REFLECTION_COUNT, const char*>::type {return Reflect<BASE>::getName(idx);}

#define REFLECT_TEMPLATED_START(TYPE) \
    template<class...Args> struct Reflect<TYPE<Args...>>{ \
        using DataType = TYPE<Args...>; \
        static constexpr const char* getName(){return #TYPE;} \
        static constexpr const bool SPECIALIZED = true; \
        static constexpr const index_t I0 = 0; \
        static constexpr const index_t REFLECT_COUNT_START = __COUNTER__;


#define REFLECT_INTERNAL_START(TYPE) \
    static constexpr ct::index_t INTERNALLY_REFLECTED = 1; \
    static constexpr const char* getName(){return #TYPE;} \
    static constexpr const ct::index_t I0 = 0; \
    static constexpr const ct::index_t REFLECT_COUNT_START = __COUNTER__; \
    using DataType = TYPE;

#define PUBLIC_ACCESS(NAME) PUBLIC_ACCESS_(NAME, __COUNTER__)

#define PUBLIC_ACCESS_(NAME, N) \
    static ct::Accessor<const decltype(DataType::NAME)&(*)(const DataType&), decltype(DataType::NAME)&(*)(DataType&)> \
        getAccessor(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>){\
            return {\
                [](const DataType& obj)-> const decltype(DataType::NAME)&{return obj.NAME;  }, \
                [](DataType& obj)-> decltype(DataType::NAME)&{ return obj.NAME; }};} \
    static constexpr const char* getName(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>){ return #NAME; }

#define REFLECT_INTERNAL_MEMBER(TYPE, NAME) \
    TYPE NAME; \
    public: PUBLIC_ACCESS(NAME)

#define ACCESSOR(NAME, GETTER, SETTER) ACCESSOR_(NAME, GETTER, SETER, __COUNTER__)

#define ACCESSOR_(NAME, GETTER, SETTER, N) \
    static constexpr auto getAccessor(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>) -> decltype(ct::makeAccessor(GETTER, SETTER)) { return ct::makeAccessor(GETTER, SETTER); } \
    static constexpr const char* getName(const ct::Indexer<I0 + N - REFLECT_COUNT_START - 1>) { return #NAME; }

#define MEMBER_FUNCTION(NAME, FPTR) \
    static auto getAccessor(const ct::Indexer<I0 + __COUNTER__ - REFLECT_COUNT_START - 1>) -> decltype(ct::makeAccessor<CalculatedValue>(FPTR)) { return ct::makeAccessor<CalculatedValue>(FPTR); }

#define REFLECT_END \
    static constexpr const index_t REFLECT_COUNT_END = __COUNTER__; \
    static constexpr const index_t REFLECTION_COUNT = I0 + REFLECT_COUNT_END - REFLECT_COUNT_START - 1; \
    static constexpr const index_t N = REFLECTION_COUNT - 1; \
    static constexpr ct::Indexer<N> end(){return ct::Indexer<N>{};} \
    }

#define REFLECT_INTERNAL_END  \
    static constexpr const ct::index_t REFLECT_COUNT_END = __COUNTER__; \
    static constexpr const ct::index_t REFLECTION_COUNT = I0 + REFLECT_COUNT_END - REFLECT_COUNT_START - 1; \
    static constexpr const index_t N = REFLECTION_COUNT - 1; \
    static constexpr ct::Indexer<N> end() { return ct::Indexer<N>{}; }

