#ifndef CT_MEMBER_POINTER_HPP
#define CT_MEMBER_POINTER_HPP

#include "../bind.hpp"
#include "access_token.hpp"
#include <ct/Indexer.hpp>
#include <ct/StringView.hpp>
#include <ct/VariadicTypedef.hpp>

#include <cstdint>
#include <cstring>
#include <tuple>

namespace ct
{
    using Flag_t = uint64_t;
    enum Flags : Flag_t
    {
        NONE = 0,
        DO_NOT_SERIALIZE = 1,
        READABLE = DO_NOT_SERIALIZE << 1,
        WRITABLE = READABLE << 1,
        REQUIRED = WRITABLE << 1,

        // reserve the first 8 bits of the flag field for ct flags
        CT_RESERVED_FLAG_BITS = 8
    };

    template <typename T, typename U>
    constexpr size_t pointerValue(U T::*member)
    {
#ifdef _MSC_VER
        // Have to test if this works on GCC
        return *(unsigned int*)(&member);
#else
        return (char*)&((T*)nullptr->*member) - (char*)nullptr;
#endif
    }

    template <class T>
    struct GetType;

    template <class T>
    struct SetType;

    template <class PTR, Flag_t FLAGS = NONE, class METADATA = void>
    struct MemberObjectPointer;

    template <class DTYPE, class CTYPE, Flag_t FLAGS>
    struct MemberObjectPointer<DTYPE CTYPE::*, FLAGS, void>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        enum : int64_t
        {
            Flags = FLAGS | READABLE | WRITABLE
        };

        constexpr MemberObjectPointer(StringView name, Data_t Class_t::*ptr) : m_name(name), m_ptr(ptr) {}

        StringView m_name;
        Data_t Class_t::*m_ptr;
    };

    template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA>
    struct MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        enum : int64_t
        {
            Flags = FLAGS | READABLE | WRITABLE
        };

        constexpr MemberObjectPointer(StringView name, Data_t Class_t::*ptr, const METADATA metadata)
            : m_name(name), m_ptr(ptr), m_metadata(metadata)
        {
        }

        StringView m_name;
        Data_t Class_t::*m_ptr;
        METADATA m_metadata;
    };

    template <class DTYPE, class CTYPE1, class CTYPE2, class METADATA, Flag_t FLAGS>
    DTYPE& get(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS, METADATA> ptr, CTYPE2& obj)
    {
        static_assert(std::is_same<CTYPE1, CTYPE2>::value || std::is_base_of<CTYPE1, CTYPE2>::value,
                      "Passed in object must either be of pointer type or derived from it");
        return obj.*(ptr.m_ptr);
    }

    template <class DTYPE, class CTYPE1, Flag_t FLAGS>
    const void* getMetadata(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS, void>& ptr)
    {
        return nullptr;
    }

    template <class METADATA, class DTYPE, class CTYPE1, Flag_t FLAGS>
    const METADATA* getMetadata(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS, METADATA>& ptr)
    {
        return &ptr.m_metadata;
    }

    template <class METADATA, class DTYPE, class CTYPE1, class MDATA, Flag_t FLAGS>
    auto getMetadata(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS, MDATA>&) ->
        typename std::enable_if<!std::is_same<METADATA, MDATA>::value, const METADATA*>::type
    {
        return nullptr;
    }

    template <class DTYPE, class CTYPE1, class CTYPE2, class METADATA, Flag_t FLAGS>
    const DTYPE& get(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS, METADATA> ptr, const CTYPE2& obj)
    {
        static_assert(std::is_same<CTYPE1, CTYPE2>::value || std::is_base_of<CTYPE1, CTYPE2>::value,
                      "Passed in object must either be of pointer type or derived from it");
        return obj.*(ptr.m_ptr);
    }

    template <class DTYPE, class CTYPE1, class CTYPE2, class METADATA, Flag_t FLAGS>
    DTYPE& set(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS, METADATA> ptr, CTYPE2& obj)
    {
        static_assert(std::is_same<CTYPE1, CTYPE2>::value || std::is_base_of<CTYPE1, CTYPE2>::value,
                      "Passed in object must either be of pointer type or derived from it");
        return obj.*(ptr.m_ptr);
    }

    template <class DTYPE, class CTYPE1, class CTYPE2, class METADATA, Flag_t FLAGS>
    void set(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS, METADATA> ptr, CTYPE2& obj, const DTYPE& val)
    {
        static_assert(std::is_same<CTYPE1, CTYPE2>::value || std::is_base_of<CTYPE1, CTYPE2>::value,
                      "Passed in object must either be of pointer type or derived from it");
        (obj.*(ptr.m_ptr)) = val;
    }

    template <class DTYPE, class CTYPE1, class CTYPE2, class METADATA, Flag_t FLAGS>
    void set(const MemberObjectPointer<DTYPE CTYPE1::*, FLAGS, METADATA> ptr, CTYPE2& obj, DTYPE&& val)
    {
        static_assert(std::is_same<CTYPE1, CTYPE2>::value || std::is_base_of<CTYPE1, CTYPE2>::value,
                      "Passed in object must either be of pointer type or derived from it");
        (obj.*(ptr.m_ptr)) = val;
    }

    template <class DTYPE, class CTYPE, class METADATA, Flag_t FLAGS>
    struct GetType<MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>>
    {
        using type = const typename MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>::Data_t&;
    };

    template <class DTYPE, class CTYPE, class METADATA, Flag_t FLAGS>
    struct SetType<MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>>
    {
        using type = typename MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>::Data_t&;
    };

    template <class T>
    struct IsMemberObjectPointer
    {
        constexpr static const bool value = false;
    };

    template <class DTYPE, class CTYPE, class METADATA, Flag_t FLAGS>
    struct IsMemberObjectPointer<MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>>
    {
        constexpr static const bool value = true;
    };

    template <class PTR>
    struct InferPointerType;

    template <class DTYPE, class CTYPE, class... ARGS>
    struct InferPointerType<DTYPE (CTYPE::*)(ARGS...)>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        static constexpr const bool IMPLICIT_THIS = false;
        static constexpr const int NUM_ARGS = sizeof...(ARGS);
    };

    template <class DTYPE, class CTYPE, class... ARGS>
    struct InferPointerType<DTYPE (CTYPE::*)(ARGS...) const>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        static constexpr const bool IMPLICIT_THIS = false;
        static constexpr const int NUM_ARGS = sizeof...(ARGS);
    };

    template <class DTYPE, class CTYPE, class... ARGS>
    struct InferPointerType<DTYPE (*)(CTYPE, ARGS...)>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        static constexpr const bool IMPLICIT_THIS = true;
        static constexpr const int NUM_ARGS = sizeof...(ARGS);
    };

    template <class DTYPE, class... ARGS>
    struct InferPointerType<DTYPE (*)(ARGS...)>
    {
        using Class_t = void;
        using Data_t = DTYPE;
        static constexpr const bool IMPLICIT_THIS = false;
        static constexpr const int NUM_ARGS = sizeof...(ARGS);
    };

    template <class... TYPES>
    struct InferClassType;
    template <class T>
    struct InferClassType<T>
    {
        using Class_t = typename InferPointerType<T>::Class_t;
    };

    template <class T, class... TYPES>
    struct InferClassType<T, TYPES...>
    {
        using Class_t = typename InferPointerType<T>::Class_t;
    };

    template <class PTR>
    struct InferSetterType;

    template <class DTYPE, class CTYPE>
    struct InferSetterType<DTYPE (CTYPE::*)()>
    {
        using type = DTYPE;
    };

    template <class DTYPE, class CTYPE>
    struct InferSetterType<void (CTYPE::*)(DTYPE)>
    {
        using type = DTYPE;
    };

    // Explicit this override
    template <class DTYPE, class CTYPE>
    struct InferSetterType<DTYPE (*)(CTYPE&)>
    {
        using type = DTYPE;
    };

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS = NONE, class METADATA = void>
    struct MemberPropertyPointer
    {
        using Class_t = typename InferPointerType<GET_PTR>::Class_t;
        using Data_t = typename InferPointerType<GET_PTR>::Data_t;
        enum : int64_t
        {
            Flags = FLAGS | READABLE | WRITABLE
        };

        constexpr MemberPropertyPointer(StringView name, GET_PTR getter, SET_PTR setter, const METADATA metadata)
            : m_name(name), m_getter(getter), m_setter(setter), m_metadata(metadata)
        {
        }

        StringView m_name;
        GET_PTR m_getter;
        SET_PTR m_setter;
        METADATA m_metadata;
    };

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS>
    struct MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, void>
    {
        using Class_t = typename InferPointerType<GET_PTR>::Class_t;
        using Data_t = typename InferPointerType<GET_PTR>::Data_t;
        enum : int64_t
        {
            Flags = FLAGS | READABLE | WRITABLE
        };

        constexpr MemberPropertyPointer(StringView name, GET_PTR getter, SET_PTR setter)
            : m_name(name), m_getter(getter), m_setter(setter)
        {
        }

        StringView m_name;
        GET_PTR m_getter;
        SET_PTR m_setter;
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

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS, class METADATA>
    typename GetType<MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>>::type
    get(const MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA> ptr,
        const typename MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>::Class_t& obj)
    {
        return (obj.*ptr.m_getter)();
    }

    template <class DTYPE, class CTYPE, class SET_PTR, Flag_t FLAGS, class METADATA>
    typename GetType<MemberPropertyPointer<DTYPE (*)(CTYPE), SET_PTR, FLAGS, METADATA>>::type
    get(const MemberPropertyPointer<DTYPE (*)(CTYPE), SET_PTR, FLAGS, METADATA> ptr,
        const typename MemberPropertyPointer<DTYPE (*)(CTYPE), SET_PTR, FLAGS, METADATA>::Class_t& obj)
    {
        return ptr.m_getter(obj);
    }

    // traditional setter
    template <class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS, class METADATA>
    void set(const MemberPropertyPointer<GET_PTR, void (CLASS::*)(SET_TYPE), FLAGS, METADATA> ptr,
             typename MemberPropertyPointer<GET_PTR, void (CLASS::*)(SET_TYPE), FLAGS, METADATA>::Class_t& obj,
             const SET_TYPE& val)
    {
        (obj.*ptr.m_setter)(val);
    }

    // Implicit this version
    template <class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS, class METADATA>
    void set(const MemberPropertyPointer<GET_PTR, void (*)(CLASS&, SET_TYPE), FLAGS, METADATA> ptr,
             CLASS& obj,
             const SET_TYPE& val)
    {
        ptr.m_setter(obj, val);
    }

    // Mutable ref setter
    template <class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS, class METADATA>
    void set(const MemberPropertyPointer<GET_PTR, SET_TYPE (CLASS::*)(), FLAGS, METADATA> ptr,
             typename MemberPropertyPointer<GET_PTR, SET_TYPE (CLASS::*)(), FLAGS, METADATA>::Class_t& obj,
             const SET_TYPE& val)
    {
        (obj.*ptr.m_setter)() = val;
    }

    // Implicit this version
    template <class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS, class METADATA>
    void set(const MemberPropertyPointer<GET_PTR, SET_TYPE (*)(CLASS&), FLAGS, METADATA> ptr,
             CLASS& obj,
             const SET_TYPE& val)
    {
        ptr.m_setter(obj) = val;
    }

    template <class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS, class METADATA>
    AccessToken<void (CLASS::*)(SET_TYPE)>
    set(const MemberPropertyPointer<GET_PTR, void (CLASS::*)(SET_TYPE), FLAGS, METADATA> ptr, CLASS& obj)
    {
        return AccessToken<void (CLASS::*)(SET_TYPE)>(obj, ptr.m_setter, get(ptr, obj));
    }

    template <class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS, class METADATA>
    SET_TYPE set(const MemberPropertyPointer<GET_PTR, SET_TYPE (*)(CLASS&), FLAGS, METADATA> ptr, CLASS& obj)
    {
        return ptr.m_setter(obj);
    }

    template <class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS, class METADATA>
    AccessToken<void (*)(CLASS&, SET_TYPE)>
    set(const MemberPropertyPointer<GET_PTR, void (*)(CLASS&, SET_TYPE), FLAGS, METADATA> ptr, CLASS& obj)
    {
        return AccessToken<void (*)(CLASS&, SET_TYPE)>(obj, ptr.m_setter, get(ptr, obj));
    }

    template <class GET_PTR, class CLASS, class SET_TYPE, Flag_t FLAGS, class METADATA>
    SET_TYPE set(const MemberPropertyPointer<GET_PTR, SET_TYPE (CLASS::*)(), FLAGS, METADATA> ptr,
                 typename MemberPropertyPointer<GET_PTR, SET_TYPE (CLASS::*)(), FLAGS, METADATA>::Class_t& obj)
    {
        return (obj.*ptr.m_setter)();
    }

    template <class GET_PTR, Flag_t FLAGS, class METADATA>
    struct MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS, METADATA>
    {
        using Class_t = typename InferPointerType<GET_PTR>::Class_t;
        using Data_t = typename InferPointerType<GET_PTR>::Data_t;
        enum : int64_t
        {
            Flags = FLAGS | READABLE
        };

        constexpr MemberPropertyPointer(StringView name, GET_PTR getter, const METADATA metadata)
            : m_name(name), m_getter(getter), m_metadata(metadata)
        {
        }

        StringView m_name;
        GET_PTR m_getter;
        METADATA m_metadata;
    };

    template <class GET_PTR, Flag_t FLAGS>
    struct MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS, void>
    {
        using Class_t = typename InferPointerType<GET_PTR>::Class_t;
        using Data_t = typename InferPointerType<GET_PTR>::Data_t;
        enum : int64_t
        {
            Flags = FLAGS | READABLE
        };

        constexpr MemberPropertyPointer(StringView name, GET_PTR getter) : m_name(name), m_getter(getter) {}

        StringView m_name;
        GET_PTR m_getter;
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

    template <class T>
    struct MemberFunctionConstness
    {
    };

    template <class R, class... ARGS>
    struct MemberFunctionConstness<R (*)(ARGS...)>
    {
        static constexpr const bool value = true;
    };

    template <class R, class T, class... ARGS>
    struct MemberFunctionConstness<R (T::*)(ARGS...) const>
    {
        static constexpr const bool value = true;
    };

    template <class R, class T, class... ARGS>
    struct MemberFunctionConstness<R (T::*)(ARGS...)>
    {
        static constexpr const bool value = false;
    };

    template <class T, class P>
    struct MemberFunction;

    template <class T, class B, class R, class... ARGS>
    struct MemberFunction<T, R (B::*)(ARGS...) const>
    {
        static constexpr const bool IS_CONST = true;
        static constexpr const uint32_t NUM_ARGS = sizeof...(ARGS);

        using Class_t = T;
        using Ret_t = R;
        using Args_t = VariadicTypedef<ARGS...>;
        using Sig_t = R (B::*)(ARGS...) const;
        using BoundSig_t = R(ARGS...);

        constexpr MemberFunction(R (B::*ptr)(ARGS...) const) : m_ptr(ptr)
        {
            static_assert(std::is_base_of<B, T>::value, "Must derive");
        }

        R invoke(const T& obj, ARGS&&... args) const { return (obj.*m_ptr)(std::forward<ARGS>(args)...); }

        std::function<R(ARGS...)> bind(const T* obj) const
        {
            return ct::variadicBind(m_ptr, obj, make_int_sequence<sizeof...(ARGS)>{});
        }

        R (B::*m_ptr)(ARGS...) const;
    };

    template <class T, class B, class R, class... ARGS>
    struct MemberFunction<T, R (B::*)(ARGS...)>
    {
        static constexpr const bool IS_CONST = false;
        static constexpr const uint32_t NUM_ARGS = sizeof...(ARGS);

        using Class_t = T;
        using Ret_t = R;
        using Args_t = VariadicTypedef<ARGS...>;
        using Sig_t = R (T::*)(ARGS...);
        using BoundSig_t = R(ARGS...);

        constexpr MemberFunction(R (T::*ptr)(ARGS...)) : m_ptr(ptr)
        {
            static_assert(std::is_base_of<B, T>::value, "Must derive");
        }

        R invoke(T& obj, ARGS&&... args) const { return (obj.*m_ptr)(std::forward<ARGS>(args)...); }

        std::function<R(ARGS...)> bind(T* obj) const
        {
            return ct::variadicBind(m_ptr, obj, make_int_sequence<sizeof...(ARGS)>{});
        }

        R (T::*m_ptr)(ARGS...);
    };

    template <class T, class R, class... ARGS>
    struct MemberFunction<T, R (*)(T&, ARGS...)>
    {
        static constexpr const bool IS_CONST = false;
        static constexpr const uint32_t NUM_ARGS = sizeof...(ARGS);

        using Class_t = T;
        using Ret_t = R;
        using Args_t = VariadicTypedef<ARGS...>;
        using Sig_t = R (*)(T&, ARGS...);
        using BoundSig_t = R(ARGS...);

        constexpr MemberFunction(R (*ptr)(T&, ARGS...)) : m_ptr(ptr) {}

        R invoke(T& obj, ARGS&&... args) const { return m_ptr(obj, std::forward<ARGS>(args)...); }

        std::function<R(ARGS...)> bind(T* obj) const
        {
            return ct::variadicBind(m_ptr, std::ref(*obj), make_int_sequence<sizeof...(ARGS)>{});
        }

        R (*m_ptr)(T&, ARGS...);
    };

    template <class T, class R, class... ARGS>
    struct MemberFunction<T, R (*)(const T&, ARGS...)>
    {
        static constexpr const bool IS_CONST = true;
        static constexpr const uint32_t NUM_ARGS = sizeof...(ARGS);

        using Class_t = T;
        using Ret_t = R;
        using Args_t = VariadicTypedef<ARGS...>;
        using Sig_t = R (*)(const T&, ARGS...);
        using BoundSig_t = R(ARGS...);

        constexpr MemberFunction(R (*ptr)(const T&, ARGS...)) : m_ptr(ptr) {}

        R invoke(const T& obj, ARGS&&... args) const { return m_ptr(obj, std::forward<ARGS>(args)...); }

        std::function<R(ARGS...)> bind(const T* obj) const
        {
            return ct::variadicBind(m_ptr, std::cref(*obj), make_int_sequence<sizeof...(ARGS)>{});
        }

        R (*m_ptr)(const T&, ARGS...);
    };

    template <class T, class SIG>
    struct MemberFunctionConstness<MemberFunction<T, SIG>>
    {
        static constexpr const bool value = MemberFunction<T, SIG>::IS_CONST;
    };

    template <class T, class P>
    constexpr MemberFunction<T, P> makeMemberFunction(P ptr)
    {
        return MemberFunction<T, P>(ptr);
    }

    template <class T, Flag_t FLAGS, class METADATA, class... PTRS>
    struct MemberFunctionPointers
    {
        StringView m_name;
        METADATA m_metadata;
        std::tuple<MemberFunction<T, PTRS>...> m_ptrs;

        enum : int64_t
        {
            Flags = FLAGS
        };

        using Constness = VariadicTypedef<std::integral_constant<bool, MemberFunction<T, PTRS>::IS_CONST...>>;
        using Class_t = T;

        constexpr MemberFunctionPointers(StringView name, const METADATA metadata, const PTRS... ptrs)
            : m_name(name), m_metadata(metadata), m_ptrs(ptrs...)
        {
        }

        template <index_t I, class... ARGS>
        auto invoke(const T& obj, ARGS&&... args) const
        {
            return std::get<I>(m_ptrs).invoke(obj, std::forward<ARGS>(args)...);
        }

        template <index_t I, class... ARGS>
        auto invoke(T& obj, ARGS&&... args) const
        {
            return std::get<I>(m_ptrs).invoke(obj, std::forward<ARGS>(args)...);
        }

        template <index_t I>
        auto bind(T* obj) const
        {
            return std::get<I>(m_ptrs).bind(obj);
        }

        template <index_t I>
        auto bind(const T* obj) const
        {
            return std::get<I>(m_ptrs).bind(obj);
        }
    };

    template <class T, Flag_t FLAGS, class... PTRS>
    struct MemberFunctionPointers<T, FLAGS, void, PTRS...>
    {
        StringView m_name;
        std::tuple<MemberFunction<T, PTRS>...> m_ptrs;

        enum : int64_t
        {
            Flags = FLAGS
        };
        using Class_t = T;

        constexpr MemberFunctionPointers(StringView name, const PTRS... ptrs)
            : m_name(name), m_ptrs(makeMemberFunction<T>(ptrs)...)
        {
        }

        template <index_t I, class... ARGS>
        auto invoke(const T& obj, ARGS&&... args) const
        {
            return std::get<I>(m_ptrs).invoke(obj, std::forward<ARGS>(args)...);
        }

        template <index_t I, class... ARGS>
        auto invoke(T& obj, ARGS&&... args) const
        {
            return std::get<I>(m_ptrs).invoke(obj, std::forward<ARGS>(args)...);
        }

        template <index_t I>
        auto bind(T* obj) const
        {
            return std::get<I>(m_ptrs).bind(obj);
        }

        template <index_t I>
        auto bind(const T* obj) const
        {
            return std::get<I>(m_ptrs).bind(obj);
        }
    };

    template <class T>
    struct IsMemberFunctionPointers
    {
        constexpr static const bool value = false;
    };

    template <class T, Flag_t FLAGS, class METADATA, class... PTRS>
    struct IsMemberFunctionPointers<MemberFunctionPointers<T, FLAGS, METADATA, PTRS...>>
    {
        constexpr static const bool value = true;
    };

    template <class T, class PTR>
    struct StaticFunction;

    template <class T, class R, class... ARGS>
    struct StaticFunction<T, R (*)(ARGS...)>
    {
        using Args_t = VariadicTypedef<ARGS...>;
        using Ret_t = R;
        using Sig_t = R (*)(ARGS...);

        constexpr static const bool IS_CONST = true;
        constexpr static const uint32_t NUM_ARGS = sizeof...(ARGS);

        constexpr StaticFunction(R (*ptr)(ARGS...)) : m_ptr(ptr) {}

        R invoke(ARGS&&... args) { return m_ptr(std::forward<ARGS>(args)...); }
        R invoke(const T&, ARGS&&... args) { return m_ptr(std::forward<ARGS>(args)...); }

        R (*m_ptr)(ARGS...);
    };

    template <class T, Flag_t FLAGS = NONE, class METADATA = void, class... PTRS>
    struct StaticFunctions
    {
        StringView m_name;
        METADATA m_metadata;
        std::tuple<StaticFunction<T, PTRS>...> m_ptrs;

        enum : Flag_t
        {
            Flags = FLAGS
        };

        using Class_t = T;

        constexpr StaticFunctions(StringView name, METADATA metadata, const PTRS... ptrs)
            : m_name(name), m_metadata(metadata), m_ptrs(ptrs...)
        {
        }

        static constexpr uint32_t numOverloads() { return sizeof...(PTRS); }

        template <index_t I>
        static constexpr bool isConst(const Indexer<I>)
        {
            return true;
        }

        template <index_t I, class... ARGS>
        auto invoke(ARGS&&... args)
        {
            return std::get<I>(m_ptrs).invoke(std::forward<ARGS>(args)...);
        }
    };

    template <class T, Flag_t FLAGS, class... PTRS>
    struct StaticFunctions<T, FLAGS, void, PTRS...>
    {
        StringView m_name;
        std::tuple<StaticFunction<T, PTRS>...> m_ptrs;

        enum : Flag_t
        {
            Flags = FLAGS
        };

        using Class_t = T;

        constexpr StaticFunctions(StringView name, const PTRS... ptrs) : m_name(name), m_ptrs(ptrs...) {}

        static constexpr uint32_t numOverloads() { return sizeof...(PTRS); }

        template <index_t I>
        static constexpr bool isConst(const Indexer<I>)
        {
            return true;
        }

        template <index_t I, class... ARGS>
        auto invoke(ARGS&&... args)
        {
            return std::get<I>(m_ptrs).invoke(std::forward<ARGS>(args)...);
        }
    };

    template <class T>
    struct IsFunction
    {
        static constexpr const bool value = false;
    };

    template <class T, Flag_t FLAGS, class METADATA, class... PTRS>
    struct IsFunction<StaticFunctions<T, FLAGS, METADATA, PTRS...>>
    {
        static constexpr const bool value = true;
    };

    template <class T, Flag_t FLAGS, class METADATA, class... PTRS>
    struct IsFunction<MemberFunctionPointers<T, FLAGS, METADATA, PTRS...>>
    {
        static constexpr const bool value = true;
    };

    template <class T, Flag_t FLAGS, class METADATA, class... PTRS>
    struct GetType<MemberFunctionPointers<T, FLAGS, METADATA, PTRS...>>
    {
        using type = VariadicTypedef<typename InferPointerType<PTRS>::Data_t...>;
    };

    template <class PTR_TYPE>
    constexpr Flag_t getFlags()
    {
        return PTR_TYPE::Flags;
    }

    template <class T>
    struct MetaDataType
    {
        using type = void;
    };

    template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA>
    struct MetaDataType<MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>>
    {
        using type = METADATA;
    };

    template <class T, Flag_t FLAGS = DO_NOT_SERIALIZE, class... ARGS>
    constexpr MemberFunctionPointers<T, FLAGS, void, ARGS...> makeMemberFunctionPointers(const char* name,
                                                                                         const ARGS... args)
    {
        return MemberFunctionPointers<T, FLAGS, void, ARGS...>(name, args...);
    }

    template <class T, Flag_t FLAGS = DO_NOT_SERIALIZE, class METADATA, class... ARGS>
    constexpr MemberFunctionPointers<T, FLAGS, METADATA, ARGS...>
    makeMemberFunctionPointersWithMetadata(const char* name, const METADATA metadata, const ARGS... args)
    {
        return MemberFunctionPointers<T, FLAGS, METADATA, ARGS...>(name, metadata, args...);
    }

    template <class T, Flag_t FLAGS = DO_NOT_SERIALIZE, class... ARGS>
    constexpr StaticFunctions<T, FLAGS, void, ARGS...> makeStaticFunctionPointers(StringView name, const ARGS... args)
    {
        return StaticFunctions<T, FLAGS, void, ARGS...>(name, args...);
    }

    template <class T, class METADATA, Flag_t FLAGS = DO_NOT_SERIALIZE, class... ARGS>
    constexpr StaticFunctions<T, FLAGS, METADATA, ARGS...>
    makeStaticFunctionPointersWithMetadata(StringView name, const METADATA metadata, const ARGS... args)
    {
        return StaticFunctions<T, FLAGS, METADATA, ARGS...>(name, metadata, args...);
    }

    template <Flag_t FLAGS = NONE, class PTR>
    constexpr MemberObjectPointer<PTR, FLAGS> makeMemberObjectPointer(const char* name, const PTR ptr)
    {
        return MemberObjectPointer<PTR, FLAGS>(name, ptr);
    }

    template <Flag_t FLAGS = NONE, class METADATA, class PTR>
    constexpr MemberObjectPointer<PTR, FLAGS, METADATA>
    makeMemberObjectPointer(const char* name, const PTR ptr, const METADATA metadata)
    {
        return MemberObjectPointer<PTR, FLAGS, METADATA>(name, ptr, metadata);
    }

    template <Flag_t FLAGS = NONE, class GET_PTR, class SET_PTR>
    constexpr MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const SET_PTR set)
    {
        return MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS>(name, get, set);
    }

    template <Flag_t FLAGS = NONE, class GET_PTR, class SET_PTR, class METADATA>
    constexpr MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const SET_PTR set, const METADATA metadata)
    {
        return MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>(name, get, set, metadata);
    }

    template <Flag_t FLAGS = NONE, class GET_PTR>
    constexpr MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const std::nullptr_t)
    {
        return MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS>(name, get);
    }

    template <Flag_t FLAGS = NONE, class GET_PTR, class METADATA>
    constexpr MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS, METADATA>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const std::nullptr_t, const METADATA metadata)
    {
        return MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS, METADATA>(name, get, metadata);
    }
}

#endif // CT_MEMBER_POINTER_HPP
