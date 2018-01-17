#pragma once
#include <ct/Hash.hpp>
#include <ct/detail/counter.hpp>
#include <ct/reflect/reflect_data.hpp>

#include <cstdint>
#include <type_traits>
namespace ct
{
    namespace reflect
    {
        namespace detail
        {
            template<class T>
            constexpr uint32_t hashDataType(T* /*ptr*/ )
            {
                return 0;
            }

#define HASH_TYPE(type) constexpr uint32_t hashDataType(type*) {return ctcrc32(#type); }
            HASH_TYPE(float);
            HASH_TYPE(double);
            HASH_TYPE(int);
            HASH_TYPE(unsigned char);
            HASH_TYPE(char);
            HASH_TYPE(unsigned int);
            HASH_TYPE(short);
            HASH_TYPE(unsigned short);
            HASH_TYPE(size_t);

            template<class T> 
            constexpr uint32_t hashDataType()
            {
                return hashDataType(static_cast<T*>(nullptr));
            }

            template<class T>
            constexpr const char* dataTypeName()
            {
#ifdef _MSC_VER
                return __FUNCSIG__;

#else
                return __PRETTY_FUNCTION__;
#endif
            }

            template<class T, int IDX>
            constexpr std::uint32_t hashMember()
            {
                typedef decltype(ReflectData<T>::get(std::declval<T>(), ct::_counter_<IDX>())) DType;

                return ct::combineHash<uint32_t>(
                            ct::detail::ctcrc32(ReflectData<T>::getName(ct::_counter_<IDX>())),
                            hashDataType<std::decay_t<DType>>());
            }

            template<class T>
            constexpr std::uint32_t hashMembers(ct::_counter_<0> /*count*/)
            {
                return hashMember<T, 0>();
            }

            template<class T, int IDX>
            constexpr std::uint32_t hashMembers(ct::_counter_<IDX> /*count*/)
            {
                return ct::combineHash<std::uint32_t>(hashMembers<T>(ct::_counter_<IDX-1>()), hashMember<T, IDX>());
            }


        }
    }
}
