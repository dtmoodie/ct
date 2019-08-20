#ifndef CT_MEMBER_PROPERTY_POINTER_HPP
#define CT_MEMBER_PROPERTY_POINTER_HPP
#include "../StringView.hpp"
#include "../types.hpp"
#include "AccessToken.hpp"
#include "metadata.hpp"
#include "pointer_traits.hpp"

namespace ct
{
    //////////////////////////////////////////////////
    /// Get overloads
    //////////////////////////////////////////////////

    template <class DTYPE, class CTYPE, class DERIVED>
    DTYPE get(DTYPE (CTYPE::*ptr)() const, const DERIVED& obj)
    {
        return (obj.*ptr)();
    }

    template <class DTYPE, class CTYPE, class DERIVED>
    DTYPE get(DTYPE (*ptr)(const CTYPE&), const DERIVED& obj)
    {
        return ptr(obj);
    }

    //////////////////////////////////////////////////
    /// Set overloads
    //////////////////////////////////////////////////
    // Member setter
    template <class BASE, class DERIVED, class SET_TYPE, class SET_TYPE2>
    void set(void (BASE::*ptr)(SET_TYPE), DERIVED& obj, SET_TYPE2 val)
    {
        (obj.*ptr)(val);
    }

    template <class BASE, class DERIVED, class SET_TYPE, class SET_TYPE2>
    void set(SET_TYPE (BASE::*ptr)(), DERIVED& obj, SET_TYPE2 val)
    {
        (obj.*ptr)() = val;
    }

    // explicit this setter
    template <class BASE, class DERIVED, class SET_TYPE, class SET_TYPE2>
    void set(void (*ptr)(BASE&, SET_TYPE), DERIVED& obj, const SET_TYPE2& val)
    {
        ptr(obj, val);
    }

    template <class BASE, class DERIVED, class SET_TYPE, class SET_TYPE2>
    void set(SET_TYPE (*ptr)(BASE&), DERIVED& obj, const SET_TYPE2& val)
    {
        ptr(obj) = val;
    }

    template <class BASE, class DERIVED, class GET_PTR, class SET_TYPE>
    SET_TYPE setMutable(SET_TYPE (BASE::*ptr)(), GET_PTR, DERIVED& obj)
    {
        return (obj.*ptr)();
    }

    template <class BASE, class DERIVED, class GET_PTR, class SET_TYPE>
    SET_TYPE setMutable(SET_TYPE (*ptr)(BASE&), GET_PTR, DERIVED& obj)
    {
        return ptr(obj);
    }

    // access token setters
    template <class DTYPE, class BASE, class DERIVED, class GET_PTR>
    AccessToken<void (BASE::*)(DTYPE)> setMutable(void (BASE::*set_ptr)(DTYPE), GET_PTR get_ptr, DERIVED& obj)
    {
        return AccessToken<void (BASE::*)(DTYPE)>(obj, set_ptr, get(get_ptr, obj));
    }

    template <class DTYPE, class BASE, class DERIVED, class GET_PTR>
    AccessToken<void (*)(BASE&, DTYPE)> setMutable(void (*set_ptr)(BASE&, DTYPE), GET_PTR get_ptr, DERIVED& obj)
    {
        return AccessToken<void (*)(BASE&, DTYPE)>(obj, set_ptr, get(get_ptr, obj));
    }

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS = 0, class METADATA = metadata::Empty>
    struct MemberPropertyPointer
    {
        StringView m_name;
        GET_PTR m_getter;
        SET_PTR m_setter;
        METADATA m_metadata;

        using Class_t = typename InferPointerType<GET_PTR>::Class_t;
        using Data_t = typename InferPointerType<GET_PTR>::Data_t;
        constexpr static const Flag_t DataFlags = FLAGS | Flags::READABLE | Flags::WRITABLE;

        constexpr MemberPropertyPointer(StringView name,
                                        GET_PTR getter,
                                        SET_PTR setter,
                                        const METADATA metadata = METADATA())
            : m_name(name), m_getter(getter), m_setter(setter), m_metadata(metadata)
        {
        }

        template <class DERIVED>
        auto get(const DERIVED& obj) const -> decltype(ct::get(m_getter, obj))
        {
            return ct::get(m_getter, obj);
        }

        template <class DERIVED, class T>
        void set(DERIVED& obj, const T& val) const
        {
            ct::set(m_setter, obj, val);
        }

        template <class DERIVED>
        auto set(DERIVED& obj) const -> decltype(ct::setMutable(m_setter, m_getter, obj))
        {
            return ct::setMutable(m_setter, m_getter, obj);
        }
        METADATA getMetadata() const { return m_metadata; }
    };

    template <class GET_PTR, Flag_t FLAGS, class METADATA>
    struct MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS, METADATA>
    {
        StringView m_name;
        GET_PTR m_getter;
        METADATA m_metadata;

        using Class_t = typename InferPointerType<GET_PTR>::Class_t;
        using Data_t = typename InferPointerType<GET_PTR>::Data_t;
        constexpr static const Flag_t DataFlags = FLAGS | Flags::READABLE;

        constexpr MemberPropertyPointer(StringView name, GET_PTR getter, const METADATA metadata = METADATA())
            : m_name(name), m_getter(getter), m_metadata(metadata)
        {
        }

        template <class DERIVED>
        auto get(const DERIVED& obj) const -> decltype(ct::get(m_getter, obj))
        {
            return ct::get(m_getter, obj);
        }
        METADATA getMetadata() const { return m_metadata; }
    };

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS, class METADATA>
    struct GetType<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>>
    {
        using type = typename MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>::Data_t;
    };

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS, class METADATA>
    struct SetType<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>>
    {
        using type = typename InferSetterType<SET_PTR>::type;
    };

    template <class T>
    struct IsMemberPropertyPointer
    {
        constexpr static const bool value = false;
        constexpr static const bool has_setter = false;
    };

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS>
    struct IsMemberPropertyPointer<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>>
    {
        constexpr static const bool value = true;
        constexpr static const bool has_setter = true;
    };

    template <class GET_PTR, Flag_t FLAGS>
    struct IsMemberPropertyPointer<MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS>>
    {
        constexpr static const bool value = true;
        constexpr static const bool has_setter = false;
    };

    template <Flag_t FLAGS = 0, class GET_PTR, class SET_PTR>
    constexpr MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS | Flags::READABLE | Flags::WRITABLE>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const SET_PTR set)
    {
        return MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS | Flags::READABLE | Flags::WRITABLE>(name, get, set);
    }

    template <Flag_t FLAGS = 0, class GET_PTR, class SET_PTR, class METADATA>
    constexpr MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS | Flags::READABLE | Flags::WRITABLE, METADATA>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const SET_PTR set, const METADATA metadata)
    {
        return MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS | Flags::READABLE | Flags::WRITABLE, METADATA>(
            name, get, set, metadata);
    }

    template <Flag_t FLAGS = 0, class GET_PTR>
    constexpr MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS | Flags::READABLE>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const std::nullptr_t)
    {
        return MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS | Flags::READABLE>(name, get);
    }

    template <Flag_t FLAGS = 0, class GET_PTR, class METADATA>
    constexpr MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS | Flags::READABLE, METADATA>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const std::nullptr_t, const METADATA metadata)
    {
        return MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS | Flags::READABLE, METADATA>(name, get, metadata);
    }
}

#endif // CT_MEMBER_PROPERTY_POINTER_HPP
