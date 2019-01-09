#pragma once
namespace ct
{
    template <class AR, class T>
    void loadStruct(AR& ar, T& obj);

    template <class AR, class T>
    void saveStruct(AR& ar, const T& obj);
}

/////////////////////////////////////////////////////////////
/// implementation
/////////////////////////////////////////////////////////////

#include <cereal/cereal.hpp>

#include <ct/TypeTraits.hpp>
#include <ct/reflect.hpp>

namespace ct
{

    template <class AR, class T, index_t I>
    auto loadValue(AR& ar, T& obj) ->
        typename std::enable_if<!std::is_same<typename ct::SetterType<T, I>::type, void>::value>::type
    {
        auto accessor = Reflect<T>::getAccessor(ct::Indexer<I>{});
        ar(cereal::make_nvp(
            Reflect<T>::getName(ct::Indexer<I>{}),
            static_cast<typename ReferenceType<typename decltype(accessor)::Set_t>::Type>(accessor.set(obj))));
    }

    template <class AR, class T, index_t I>
    auto loadValue(AR&, T&) ->
        typename std::enable_if<std::is_same<typename ct::SetterType<T, I>::type, void>::value>::type
    {
    }

    template <class AR, class T>
    void loadStructHelper(AR& ar, T& obj, const ct::Indexer<0>)
    {
        loadValue<AR, T, 0>(ar, obj);
    }

    template <index_t I, class AR, class T>
    void loadStructHelper(AR& ar, T& obj, const ct::Indexer<I> idx)
    {
        loadStructHelper(ar, obj, --idx);
        loadValue<AR, T, I>(ar, obj);
    }

    template <class AR, class T>
    void loadStruct(AR& ar, T& obj)
    {
        loadStructHelper(ar, obj, Reflect<T>::end());
    }

    template <class AR, class T, index_t I>
    auto saveValue(AR&, const T&, const ct::Indexer<I> idx) ->
        typename std::enable_if<std::is_same<typename GetterTraits<T, I>::type, CalculatedValue>::value>::type
    {
    }

    template <class AR, class T, index_t I>
    auto saveValue(AR& ar, const T& obj, const ct::Indexer<I> idx) ->
        typename std::enable_if<!std::is_same<typename GetterTraits<T, I>::type, CalculatedValue>::value>::type
    {
        auto accessor = Reflect<T>::getAccessor(idx);
        ar(cereal::make_nvp(Reflect<T>::getName(ct::Indexer<I>{}), accessor.get(obj)));
    }

    template <class AR, class T>
    void saveStructHelper(AR& ar, const T& obj, const ct::Indexer<0> idx)
    {
        saveValue<AR, T, 0>(ar, obj, idx);
    }

    template <index_t I, class AR, class T>
    void saveStructHelper(AR& ar, const T& obj, const ct::Indexer<I> idx)
    {
        saveStructHelper(ar, obj, --idx);
        saveValue<AR, T, I>(ar, obj, idx);
    }

    template <class AR, class T>
    void saveStruct(AR& ar, const T& obj)
    {
        saveStructHelper(ar, obj, Reflect<T>::end());
    }
}

namespace cereal
{

    template <class AR, class T>
    auto saveDispatcher(AR& ar, const T& data) ->
        typename std::enable_if<(ct::is_reference_type<typename ct::GetterType<T, 0>::type>::value == false ||
                                 ct::Reflect<T>::N > 0)>::type
    {
        ct::saveStruct(ar, data);
    }

    template <class AR, class T>
    auto saveDispatcher(AR& ar, const T& data) ->
        typename std::enable_if<(ct::is_reference_type<typename ct::GetterType<T, 0>::type>::value == true &&
                                 ct::Reflect<T>::N == 0)>::type
    {
        auto accessor = ct::Reflect<T>::getAccessor(ct::Indexer<0>{});
        ar(accessor.get(data));
    }

    template <class AR, class T>
    auto save(AR& ar, const T& data) -> ct::enable_if_reflected<T>
    {
        ct::saveStruct(ar, data);
    }

    template <class AR, class T>
    auto loadDispatcher(AR& ar, T& data) ->
        typename std::enable_if<(ct::is_reference_type<typename ct::GetterType<T, 0>::type>::value == false ||
                                 ct::Reflect<T>::N > 0)>::type
    {
        ct::loadStruct(ar, data);
    }

    template <class AR, class T>
    auto loadDispatcher(AR& ar, T& data) ->
        typename std::enable_if<(ct::is_reference_type<typename ct::GetterType<T, 0>::type>::value == true &&
                                 ct::Reflect<T>::N == 0)>::type
    {
        ct::loadValue<AR, T, 0>(ar, data);
    }

    template <class AR, class T>
    auto load(AR& ar, T& data) -> ct::enable_if_reflected<T>
    {
        ct::loadStruct(ar, data);
    }
}
