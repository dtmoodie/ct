#pragma once

#include <cereal/cereal.hpp>

namespace ct
{
    namespace reflect
    {
        namespace detail
        {
            template <class AR, class T>
            static constexpr inline void serializeHelper(AR& ar, T& data, _counter_<0>)
            {
                ar(cereal::make_nvp(getName<0, T>(), get<0>(data)));
            }

            template <int I, class AR, class T>
            static constexpr inline void serializeHelper(AR& ar, T& data, _counter_<I>)
            {
                serializeHelper(ar, data, _counter_<I - 1>());
                ar(cereal::make_nvp(getName<I, T>(), get<I>(data)));
            }
        }
    }
}