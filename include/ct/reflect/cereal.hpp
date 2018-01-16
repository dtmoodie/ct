#pragma once

#include <cereal/cereal.hpp>
#include <ct/reflect/detail/cereal.hpp>
#include <ct/reflect/reflect_data.hpp>
namespace ct
{
    namespace reflect
    {
        template <class AR, class T>
        enable_if_reflected<T> serialize(AR& ar, T& data)
        {
            detail::serializeHelper(ar, data, _counter_<ReflectData<T>::N - 1>());
        }
    }
}

namespace cereal
{
    template <class AR, class T>
    ct::reflect::enable_if_reflected<T> serialize(AR& ar, T& data)
    {
        ct::reflect::serialize(ar, data);
    }
}
