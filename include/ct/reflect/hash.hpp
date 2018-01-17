#pragma once
#include <ct/Hash.hpp>
#include <ct/String.hpp>
#include <ct/reflect/detail/hash.hpp>
#include <ct/reflect/reflect_data.hpp>
namespace ct
{
    namespace reflect
    {
        template<class T>
        constexpr std::uint32_t hashMembers()
        {
            return detail::hashMembers<T>(ct::_counter_<ReflectData<T>::N - 1>());
        }

        template<class T>
        constexpr std::uint32_t classHash()
        {
            return ct::combineHash<std::uint32_t>(ct::detail::ctcrc32(ReflectData<T>::getName()), hashMembers<T>());
        }
    }
}
