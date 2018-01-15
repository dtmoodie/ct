#pragma once

namespace ct
{
    namespace reflect
    {
        namespace detail
        {
            template<class T, int IDX>
            constexpr uint32_t hashMember()
            {
                return ct::detail::ctcrc32(ReflectData<T>::getName(ct::_counter_<IDX>()));
            }

            template<class T, int IDX>
            constexpr uint32_t hashMembers(ct::_counter_<IDX> count)
            {
                return ct::combineHash(hashMembers<T>(ct::_counter_<IDX-1>()), hashMember<T, IDX>());
            }

            template<class T>
            constexpr uint32_t hashMembers(ct::_counter_<0> count)
            {
                return hashMember<T, 0>();
            }
        }
    }
}