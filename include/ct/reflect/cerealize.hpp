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
    template<class T, index_t I>
    struct CountSerializableFieldsHelper
    {
        constexpr static const uint32_t value = ShouldSerialize<T, I>::value ? 1 : 0;
        constexpr static const uint32_t count = CountSerializableFieldsHelper<T, I-1>::count + value;
    };

    template<class T>
    struct CountSerializableFieldsHelper<T, 0>
    {
        constexpr static const uint32_t value = ShouldSerialize<T, 0>::value ? 1 : 0;
        constexpr static const uint32_t count = value;
    };
    template<class T>
    struct CountSerializableFields
    {
        constexpr static const uint32_t value = CountSerializableFieldsHelper<T, Reflect<T>::N>::count;
    };


    template <class AR, class T, index_t I>
    auto loadValue(AR& ar, T& obj) ->
        typename std::enable_if<ShouldSerialize<T, I>::value>::type
    {
        auto accessor = Reflect<T>::getPtr(ct::Indexer<I>{});
        ar(cereal::make_nvp(
            Reflect<T>::getName(ct::Indexer<I>{}),
            static_cast<typename ReferenceType<typename SetType<decltype(accessor)>::type>::Type>(set(accessor, obj))));
    }

    template <class AR, class T, index_t I>
    auto loadValue(AR&, T&) ->
        typename std::enable_if<!ShouldSerialize<T, I>::value>::type
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
    auto saveValue(AR&, const T&, const ct::Indexer<I> ) ->
        typename std::enable_if<!ShouldSerialize<T, I>::value>::type
    {
    }

    template <class AR, class T, index_t I>
    auto saveValue(AR& ar, const T& obj, const ct::Indexer<I> idx) ->
        typename std::enable_if<ShouldSerialize<T, I>::value>::type
    {
        auto accessor = Reflect<T>::getPtr(idx);
        ar(cereal::make_nvp(Reflect<T>::getName(ct::Indexer<I>{}), get(accessor, obj)));
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
    auto save(AR& ar, const T& data) -> ct::EnableIfReflected<T>
    {
        ct::saveStruct(ar, data);
    }

    template <class AR, class T>
    auto load(AR& ar, T& data) -> ct::EnableIfReflected<T>
    {
        ct::loadStruct(ar, data);
    }
}
