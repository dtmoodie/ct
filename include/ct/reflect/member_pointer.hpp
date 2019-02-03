#ifndef CT_MEMBER_POINTER_HPP
#define CT_MEMBER_POINTER_HPP

#include "access_token.hpp"
#include <ct/VariadicTypedef.hpp>

#include <cstdint>
#include <tuple>

namespace ct
{
    using Flag_t = uint64_t;
    enum Flags: Flag_t
    {
        NONE = 0,
        DO_NOT_SERIALIZE = 1,
        READABLE = 2,
        WRITABLE = 4,

        // reserve the first 8 bits of the flag field for ct flags
        CT_RESERVED_FLAG_BITS = 8
    };

    template<class T>
    struct GetType;

    template<class T>
    struct SetType;

    template<class PTR, Flag_t FLAGS = NONE>
    struct MemberObjectPointer;

    template<class DTYPE, class CTYPE, Flag_t FLAGS>
    struct MemberObjectPointer<DTYPE CTYPE::*, FLAGS>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        enum : int64_t {Flags = FLAGS | READABLE | WRITABLE};

        constexpr MemberObjectPointer(const char* name, Data_t Class_t::* ptr):
            m_name(name),
            m_ptr(ptr)
        {}


        const char* m_name;
        Data_t Class_t::* m_ptr;
    };

    template<class DTYPE, class CTYPE1, class CTYPE2, Flag_t FLAGS>
    DTYPE& get(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS> ptr, CTYPE2& obj )
    {
        static_assert(std::is_same<CTYPE1, CTYPE2>::value || std::is_base_of<CTYPE1, CTYPE2>::value, "Passed in object must either be of pointer type or derived from it");
        return obj.*(ptr.m_ptr);
    }

    template<class DTYPE, class CTYPE1, class CTYPE2, Flag_t FLAGS>
    const DTYPE& get(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS> ptr, const CTYPE2& obj )
    {
        static_assert(std::is_same<CTYPE1, CTYPE2>::value || std::is_base_of<CTYPE1, CTYPE2>::value, "Passed in object must either be of pointer type or derived from it");
        return obj.*(ptr.m_ptr);
    }

    template<class DTYPE, class CTYPE1, class CTYPE2,  Flag_t FLAGS>
    DTYPE& set(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS> ptr, CTYPE2& obj )
    {
        static_assert(std::is_same<CTYPE1, CTYPE2>::value || std::is_base_of<CTYPE1, CTYPE2>::value, "Passed in object must either be of pointer type or derived from it");
        return obj.*(ptr.m_ptr);
    }

    template<class DTYPE, class CTYPE1, class CTYPE2, Flag_t FLAGS>
    void set(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS> ptr, CTYPE2& obj, const DTYPE& val)
    {
        static_assert(std::is_same<CTYPE1, CTYPE2>::value || std::is_base_of<CTYPE1, CTYPE2>::value, "Passed in object must either be of pointer type or derived from it");
        (obj.*(ptr.m_ptr)) = val;
    }

    template<class DTYPE, class CTYPE1, class CTYPE2, Flag_t FLAGS>
    void set(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS> ptr, CTYPE2& obj, DTYPE&& val)
    {
        static_assert(std::is_same<CTYPE1, CTYPE2>::value || std::is_base_of<CTYPE1, CTYPE2>::value, "Passed in object must either be of pointer type or derived from it");
        (obj.*(ptr.m_ptr)) = val;
    }

    template<class DTYPE, class CTYPE, Flag_t FLAGS>
    struct GetType<MemberObjectPointer<DTYPE CTYPE::*, FLAGS>>
    {
        using type = typename MemberObjectPointer<DTYPE CTYPE::*, FLAGS>::Data_t&;
    };

    template<class DTYPE, class CTYPE, Flag_t FLAGS>
    struct SetType<MemberObjectPointer<DTYPE CTYPE::*, FLAGS>>
    {
        using type = typename MemberObjectPointer<DTYPE CTYPE::*, FLAGS>::Data_t&;
    };

    template<class T>
    struct IsMemberObjectPointer
    {
        constexpr static const bool value = false;
    };

    template<class DTYPE, class CTYPE, Flag_t FLAGS>
    struct IsMemberObjectPointer<MemberObjectPointer<DTYPE CTYPE::*, FLAGS>>
    {
        constexpr static const bool value = true;
    };

    template<class PTR>
    struct InferPointerType;

    template<class DTYPE, class CTYPE, class ... ARGS>
    struct InferPointerType<DTYPE(CTYPE::*)(ARGS...)>
    {
        using Class_t = CTYPE;
        using Data_t  = DTYPE;
        static constexpr const int NUM_ARGS = sizeof...(ARGS);
    };

    template<class DTYPE, class CTYPE, class ... ARGS>
    struct InferPointerType<DTYPE(CTYPE::*)(ARGS...) const>
    {
        using Class_t = CTYPE;
        using Data_t  = DTYPE;
        static constexpr const int NUM_ARGS = sizeof...(ARGS);
    };

    template<class ... TYPES>
    struct InferClassType;
    template<class T>
    struct InferClassType<T>
    {
        using Class_t = typename InferPointerType<T>::Class_t;
    };
    template<class T, class ... TYPES>
    struct InferClassType<T, TYPES...>
    {
        using Class_t = typename InferPointerType<T>::Class_t;
    };

    template<class PTR>
    struct InferSetterType;

    template<class DTYPE, class CTYPE>
    struct InferSetterType<DTYPE(CTYPE::*)()>
    {
        using type = DTYPE;
    };

    template<class DTYPE, class CTYPE>
    struct InferSetterType<void(CTYPE::*)(DTYPE)>
    {
        using type = DTYPE;
    };


    template<class GET_PTR, class SET_PTR, Flag_t FLAGS = NONE>
    struct MemberPropertyPointer
    {
        using Class_t = typename InferPointerType<GET_PTR>::Class_t;
        using Data_t = typename InferPointerType<GET_PTR>::Data_t;
        enum : int64_t {Flags = FLAGS | READABLE | WRITABLE};

        constexpr MemberPropertyPointer(const char* name, GET_PTR getter, SET_PTR setter):
            m_name(name),
            m_getter(getter),
            m_setter(setter)
        {}

        const char* m_name;
        GET_PTR m_getter;
        SET_PTR m_setter;
    };

    template<class GET_PTR, class SET_PTR, Flag_t FLAGS>
    struct GetType<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>>
    {
        using type = typename MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>::Data_t;
    };

    template<class GET_PTR, class SET_PTR, Flag_t FLAGS>
    struct SetType<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>>
    {
        using type = typename InferSetterType<SET_PTR>::type;
    };

    template<class GET_PTR, class SET_PTR, Flag_t FLAGS>
    typename GetType<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>>::type
    get(const MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS> ptr, const typename MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>::Class_t& obj)
    {
        return (obj.*ptr.m_getter)();
    }

    template<class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS>
    void set(const MemberPropertyPointer<GET_PTR, void(CLASS::*)(SET_TYPE), FLAGS> ptr,
             typename MemberPropertyPointer<GET_PTR, void(CLASS::*)(SET_TYPE), FLAGS>::Class_t& obj,
             const SET_TYPE& val)
    {
        (obj.*ptr.m_setter)(val);
    }

    template<class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS>
    void set(const MemberPropertyPointer<GET_PTR, SET_TYPE(CLASS::*)(), FLAGS> ptr,
             typename MemberPropertyPointer<GET_PTR, SET_TYPE(CLASS::*)(), FLAGS>::Class_t& obj,
             const SET_TYPE& val)
    {
        (obj.*ptr.m_setter)() = val;
    }

    template<class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS>
    AccessToken<void(CLASS::*)(SET_TYPE)> set(const MemberPropertyPointer<GET_PTR, void(CLASS::*)(SET_TYPE), FLAGS> ptr,
             typename MemberPropertyPointer<GET_PTR, void(CLASS::*)(SET_TYPE), FLAGS>::Class_t& obj)
    {
        return AccessToken<void (CLASS::*)(SET_TYPE)>(obj, ptr.m_setter, get(ptr, obj));
    }

    template<class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS>
    SET_TYPE set(const MemberPropertyPointer<GET_PTR, SET_TYPE(CLASS::*)(), FLAGS> ptr,
             typename MemberPropertyPointer<GET_PTR, SET_TYPE(CLASS::*)(), FLAGS>::Class_t& obj)
    {
        return (obj.*ptr.m_setter)();
    }

    template<class GET_PTR, Flag_t FLAGS>
    struct MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS>
    {
        using Class_t = typename InferPointerType<GET_PTR>::Class_t;
        using Data_t = typename InferPointerType<GET_PTR>::Data_t;
        enum : int64_t {Flags = FLAGS | READABLE};

        constexpr MemberPropertyPointer(const char* name, GET_PTR getter):
            m_name(name),
            m_getter(getter)
        {}

        const char* m_name;
        GET_PTR m_getter;
    };

    template<class T>
    struct IsMemberPropertyPointer
    {
        constexpr static const bool value = false;
        constexpr static const bool has_setter = false;
    };

    template<class GET_PTR, class SET_PTR, Flag_t FLAGS>
    struct IsMemberPropertyPointer<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>>
    {
        constexpr static const bool value = true;
        constexpr static const bool has_setter = true;
    };

    template<class GET_PTR, Flag_t FLAGS>
    struct IsMemberPropertyPointer<MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS>>
    {
        constexpr static const bool value = true;
        constexpr static const bool has_setter = false;
    };

    template<Flag_t FLAGS, class ... PTRS>
    struct MemberFunctionPointers
    {
        enum : int64_t {Flags = FLAGS};
        using Class_t = typename InferClassType<PTRS...>::Class_t;

        constexpr MemberFunctionPointers(const char* name, const PTRS ... ptrs):
            m_name(name), m_ptrs(ptrs...)
        {
        }

        const char* m_name;
        std::tuple<PTRS...> m_ptrs;
    };

    template<int I, Flag_t FLAGS, class ... PTRS, class ... ARGS>
    auto invoke(const MemberFunctionPointers<FLAGS, PTRS...> ptrs, const typename MemberFunctionPointers<FLAGS, PTRS...>::Class_t& obj, ARGS&&... args)
        -> decltype((obj.*std::get<I>(ptrs.m_ptrs))(std::forward<ARGS>(args)...))
    {
        return (obj.*std::get<I>(ptrs.m_ptrs))(std::forward<ARGS>(args)...);
    }

    template<Flag_t FLAGS, class ... PTRS>
    struct GetType<MemberFunctionPointers<FLAGS, PTRS...>>
    {
        using type = VariadicTypedef<typename InferPointerType<PTRS>::Data_t...>;
    };

    template<class T>
    struct IsMemberFunctionPointers
    {
        constexpr static const bool value = false;
    };

    template<Flag_t FLAGS, class ... PTRS>
    struct IsMemberFunctionPointers<MemberFunctionPointers<FLAGS, PTRS...>>
    {
        constexpr static const bool value = true;
    };

    template<class PTR_TYPE>
    constexpr int64_t getFlags()
    {
        return PTR_TYPE::Flags;
    }

    template<Flag_t FLAGS = DO_NOT_SERIALIZE, class ... ARGS>
    constexpr MemberFunctionPointers<FLAGS, ARGS...> makeMemberFunctionPointers(const char* name, const ARGS... args)
    {
        return MemberFunctionPointers<FLAGS, ARGS...>(name, args...);
    }

    template<Flag_t FLAGS = NONE, class PTR>
    constexpr MemberObjectPointer<PTR, FLAGS> makeMemberObjectPointer(const char* name, const PTR ptr)
    {
        return MemberObjectPointer<PTR, FLAGS>(name, ptr);
    }

    template<Flag_t FLAGS = NONE, class GET_PTR, class SET_PTR>
    constexpr MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS> makeMemberPropertyPointer(const char* name, const GET_PTR get, const SET_PTR set)
    {
        return MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>(name, get, set);
    }

    template<Flag_t FLAGS = NONE, class GET_PTR>
    constexpr MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS> makeMemberPropertyPointer(const char* name, const GET_PTR get, const std::nullptr_t)
    {
        return MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS>(name, get);
    }

}

#endif // CT_MEMBER_POINTER_HPP
