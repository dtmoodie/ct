#ifndef CT_MEMBER_OBJECT_POINTER_HPP
#define CT_MEMBER_OBJECT_POINTER_HPP
#include "../bind.hpp"
#include "../flags.hpp"
#include "../types.hpp"
#include "../Indexer.hpp"
#include "../StringView.hpp"
#include "../VariadicTypedef.hpp"
#include "../type_traits.hpp"

#include "metadata.hpp"
#include "pointer_traits.hpp"

#include <cstring>
#include <tuple>

namespace ct
{

    template <typename T, typename U>
    constexpr size_t memberOffset(U T::*member)
    {
#if defined(_MSC_VER) || defined(__clang__)
        // Have to test if this works on GCC
        return *(unsigned int*)(&member);
#else
        return (char*)&((T*)nullptr->*member) - (char*)nullptr;
#endif
    }

    template <class PTR, Flag_t FLAGS = 0, class METADATA = Empty>
    struct MemberObjectPointer;

    template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA>
    struct MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        constexpr static const int64_t DataFlags = FLAGS | Flags::READABLE | Flags::WRITABLE;

        constexpr MemberObjectPointer(StringView name, Data_t Class_t::*ptr, const METADATA metadata = METADATA())
            : m_name(name), m_ptr(ptr), m_metadata(metadata)
        {
        }

        template <class DERIVED>
        const Data_t& get(DERIVED& obj) const
        {
            static_assert(std::is_same<CTYPE, DERIVED>::value || std::is_base_of<CTYPE, DERIVED>::value,
                          "Passed in object must either be of class type or derived from it");
            return obj.*m_ptr;
        }

        template <class DERIVED>
        Data_t& set(DERIVED& obj) const
        {
            static_assert(std::is_same<CTYPE, DERIVED>::value || std::is_base_of<CTYPE, DERIVED>::value,
                          "Passed in object must either be of class type or derived from it");
            return obj.*m_ptr;
        }

        template <class DERIVED>
        void set(DERIVED& obj, const Data_t& val) const
        {
            static_assert(std::is_same<CTYPE, DERIVED>::value || std::is_base_of<CTYPE, DERIVED>::value,
                          "Passed in object must either be of class type or derived from it");
            obj.*m_ptr = val;
        }

        template <class DERIVED>
        void set(DERIVED& obj, Data_t&& val) const
        {
            static_assert(std::is_same<CTYPE, DERIVED>::value || std::is_base_of<CTYPE, DERIVED>::value,
                          "Passed in object must either be of class type or derived from it");
            obj.*m_ptr = std::move(val);
        }

        constexpr StringView  getName() const { return m_name; }

        METADATA getMetadata() const { return m_metadata; }

        StringView m_name;
        Data_t Class_t::*m_ptr;
        METADATA m_metadata;
    };

    template <class M, class T, Flag_t F>
    const M* metadata(const MemberObjectPointer<T, F, M>& ptr)
    {
        return &ptr.m_metadata;
    }

    template <class M, class T, Flag_t F>
    M* metadata(MemberObjectPointer<T, F, M>& ptr)
    {
        return &ptr.m_metadata;
    }

    template <class M, class T, Flag_t F, class M2>
    EnableIf<!std::is_same<M, M2>::value, const M*> metadata(const MemberObjectPointer<T, F, M2>&)
    {
        return nullptr;
    }

    template <class M, class T, Flag_t F, class M2>
    EnableIf<!std::is_same<M, M2>::value, M*> metadata(MemberObjectPointer<T, F, M2>&)
    {
        return nullptr;
    }

    template <class PTR_TYPE>
    constexpr Flag_t flags()
    {
        using type = typename std::decay<PTR_TYPE>::type;
        return type::DataFlags;
    }

    template <Flag_t FLAGS = 0, class PTR>
    constexpr MemberObjectPointer<PTR, FLAGS | Flags::READABLE | Flags::WRITABLE>
    makeMemberObjectPointer(const char* name, const PTR ptr)
    {
        return MemberObjectPointer<PTR, FLAGS | Flags::READABLE | Flags::WRITABLE>(name, ptr);
    }

    template <Flag_t FLAGS = 0, class METADATA, class PTR>
    constexpr MemberObjectPointer<PTR, FLAGS | Flags::READABLE | Flags::WRITABLE, METADATA>
    makeMemberObjectPointer(const char* name, const PTR ptr, const METADATA metadata)
    {
        return MemberObjectPointer<PTR, FLAGS | Flags::READABLE | Flags::WRITABLE, METADATA>(name, ptr, metadata);
    }

    template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA>
    struct MetaDataType<MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>>
    {
        using type = METADATA;
    };
}

#endif // CT_MEMBER_OBJECT_POINTER_HPP
