#pragma once

namespace ct
{
    namespace reflect
    {
        namespace detail
        {
            template<class T> 
            constexpr uint32_t hashDataType()
            {
                return ct::detail::ctcrc32(__FUNCSIG__);
            }

            template<class T>
            constexpr const char* dataTypeName()
            {
                return __FUNCSIG__;
            }

            template<class T, int IDX>
            constexpr std::uint32_t hashMember()
            {
                typedef decltype(ReflectData<T>::get(std::declval<T>(), ct::_counter_<IDX>())) DType;

                return ct::combineHash<uint32_t>(ct::detail::ctcrc32(ReflectData<T>::getName(ct::_counter_<IDX>())), hashDataType<std::decay_t<DType>>());
            }

            template<class T, int IDX>
            constexpr std::uint32_t hashMembers(ct::_counter_<IDX> count)
            {
                return ct::combineHash<std::uint32_t>(hashMembers<T>(ct::_counter_<IDX-1>()), hashMember<T, IDX>());
            }

            template<class T>
            constexpr std::uint32_t hashMembers(ct::_counter_<0> count)
            {
                return hashMember<T, 0>();
            }
        }
    }
}