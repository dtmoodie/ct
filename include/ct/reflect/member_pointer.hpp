#ifndef CT_MEMBER_POINTER_HPP
#define CT_MEMBER_POINTER_HPP

#include "../bind.hpp"
#include "access_token.hpp"
#include "metadata.hpp"

#include <ct/Indexer.hpp>
#include <ct/StringView.hpp>
#include <ct/VariadicTypedef.hpp>
#include <ct/type_traits.hpp>

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
        INVOKABLE = WRITABLE << 1,
        REQUIRED = INVOKABLE << 1,
        COMPILE_TIME_CONSTANT = REQUIRED << 1,

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

    template <class PTR, Flag_t FLAGS = NONE, class METADATA = metadata::Empty>
    struct MemberObjectPointer;

    template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA>
    struct MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        enum : int64_t
        {
            Flags = FLAGS | READABLE | WRITABLE
        };

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

        constexpr StringView getName() const { return m_name; }

        METADATA getMetadata() const { return m_metadata; }

        StringView m_name;
        Data_t Class_t::*m_ptr;
        METADATA m_metadata;
    };

    template <class M, class T, Flag_t F>
    const M* getMetadata(const MemberObjectPointer<T, F, M>& ptr)
    {
        return &ptr.m_metadata;
    }

    template <class M, class T, Flag_t F>
    M* getMetadata(MemberObjectPointer<T, F, M>& ptr)
    {
        return &ptr.m_metadata;
    }

    template <class M, class T, Flag_t F, class M2>
    EnableIf<!std::is_same<M, M2>::value, const M*> getMetadata(const MemberObjectPointer<T, F, M2>&)
    {
        return nullptr;
    }

    template <class M, class T, Flag_t F, class M2>
    EnableIf<!std::is_same<M, M2>::value, M*> getMetadata(MemberObjectPointer<T, F, M2>&)
    {
        return nullptr;
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

    // Visual studio has issues so we make an explicit version for when args is nothing
    template <class DTYPE, class CTYPE>
    struct InferPointerType<DTYPE (CTYPE::*)() const>
    {
        using Class_t = CTYPE;
        using Data_t = DTYPE;
        static constexpr const bool IMPLICIT_THIS = false;
        static constexpr const int NUM_ARGS = 0;
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

    template <class GET_PTR, class SET_PTR, Flag_t FLAGS = NONE, class METADATA = metadata::Empty>
    struct MemberPropertyPointer
    {
        StringView m_name;
        GET_PTR m_getter;
        SET_PTR m_setter;
        METADATA m_metadata;

        using Class_t = typename InferPointerType<GET_PTR>::Class_t;
        using Data_t = typename InferPointerType<GET_PTR>::Data_t;
        enum : int64_t
        {
            Flags = FLAGS | READABLE | WRITABLE
        };

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
        enum : int64_t
        {
            Flags = FLAGS | READABLE
        };

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
            -> decltype(std::get<I>(m_ptrs).invoke(obj, std::forward<ARGS>(args)...))
        {
            return std::get<I>(m_ptrs).invoke(obj, std::forward<ARGS>(args)...);
        }

        template <index_t I, class... ARGS>
        auto invoke(T& obj, ARGS&&... args) const
            -> decltype(std::get<I>(m_ptrs).invoke(obj, std::forward<ARGS>(args)...))
        {
            return std::get<I>(m_ptrs).invoke(obj, std::forward<ARGS>(args)...);
        }

        template <index_t I>
        auto bind(T* obj) const -> decltype(std::get<I>(m_ptrs).bind(obj))
        {
            return std::get<I>(m_ptrs).bind(obj);
        }

        template <index_t I>
        auto bind(const T* obj) const -> decltype(std::get<I>(m_ptrs).bind(obj))
        {
            return std::get<I>(m_ptrs).bind(obj);
        }

        METADATA getMetadata() const { return m_metadata; }
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
        auto invoke(const T& obj, ARGS&&... args) const ->
            typename std::decay<decltype(std::get<I>(m_ptrs))>::type::Ret_t
        {
            return std::get<I>(m_ptrs).invoke(obj, std::forward<ARGS>(args)...);
        }

        template <index_t I, class... ARGS>
        auto invoke(T& obj, ARGS&&... args) const -> typename std::decay<decltype(std::get<I>(m_ptrs))>::type::Ret_t
        {
            return std::get<I>(m_ptrs).invoke(obj, std::forward<ARGS>(args)...);
        }

        template <index_t I>
        auto bind(T* obj) const -> decltype(std::get<I>(m_ptrs).bind(obj))
        {
            return std::get<I>(m_ptrs).bind(obj);
        }

        template <index_t I>
        auto bind(const T* obj) const -> decltype(std::get<I>(m_ptrs).bind(obj))
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
        auto invoke(ARGS&&... args) -> decltype(std::get<I>(m_ptrs).invoke(std::forward<ARGS>(args)...))
        {
            return std::get<I>(m_ptrs).invoke(std::forward<ARGS>(args)...);
        }

        METADATA getMetadata() const { return m_metadata; }
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
        auto invoke(ARGS&&... args) -> typename std::decay<decltype(std::get<I>(m_ptrs))>::type::Ret_t
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
        using type = typename std::decay<PTR_TYPE>::type;
        return type::Flags;
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
    constexpr MemberFunctionPointers<T, FLAGS | INVOKABLE, void, ARGS...> makeMemberFunctionPointers(const char* name,
                                                                                                     const ARGS... args)
    {
        return MemberFunctionPointers<T, FLAGS | INVOKABLE, void, ARGS...>(name, args...);
    }

    template <class T, Flag_t FLAGS = DO_NOT_SERIALIZE, class METADATA, class... ARGS>
    constexpr MemberFunctionPointers<T, FLAGS | INVOKABLE, METADATA, ARGS...>
    makeMemberFunctionPointersWithMetadata(const char* name, const METADATA metadata, const ARGS... args)
    {
        return MemberFunctionPointers<T, FLAGS | INVOKABLE, METADATA, ARGS...>(name, metadata, args...);
    }

    template <class T, Flag_t FLAGS = DO_NOT_SERIALIZE, class... ARGS>
    constexpr StaticFunctions<T, FLAGS | INVOKABLE, void, ARGS...> makeStaticFunctionPointers(StringView name,
                                                                                              const ARGS... args)
    {
        return StaticFunctions<T, FLAGS | INVOKABLE, void, ARGS...>(name, args...);
    }

    template <class T, class METADATA, Flag_t FLAGS = DO_NOT_SERIALIZE, class... ARGS>
    constexpr StaticFunctions<T, FLAGS | INVOKABLE, METADATA, ARGS...>
    makeStaticFunctionPointersWithMetadata(StringView name, const METADATA metadata, const ARGS... args)
    {
        return StaticFunctions<T, FLAGS | INVOKABLE, METADATA, ARGS...>(name, metadata, args...);
    }

    template <Flag_t FLAGS = NONE, class PTR>
    constexpr MemberObjectPointer<PTR, FLAGS | READABLE | WRITABLE> makeMemberObjectPointer(const char* name,
                                                                                            const PTR ptr)
    {
        return MemberObjectPointer<PTR, FLAGS | READABLE | WRITABLE>(name, ptr);
    }

    template <Flag_t FLAGS = NONE, class METADATA, class PTR>
    constexpr MemberObjectPointer<PTR, FLAGS | READABLE | WRITABLE, METADATA>
    makeMemberObjectPointer(const char* name, const PTR ptr, const METADATA metadata)
    {
        return MemberObjectPointer<PTR, FLAGS | READABLE | WRITABLE, METADATA>(name, ptr, metadata);
    }

    template <Flag_t FLAGS = NONE, class GET_PTR, class SET_PTR>
    constexpr MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS | READABLE | WRITABLE>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const SET_PTR set)
    {
        return MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS | READABLE | WRITABLE>(name, get, set);
    }

    template <Flag_t FLAGS = NONE, class GET_PTR, class SET_PTR, class METADATA>
    constexpr MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS | READABLE | WRITABLE, METADATA>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const SET_PTR set, const METADATA metadata)
    {
        return MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS | READABLE | WRITABLE, METADATA>(name, get, set, metadata);
    }

    template <Flag_t FLAGS = NONE, class GET_PTR>
    constexpr MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS | READABLE>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const std::nullptr_t)
    {
        return MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS | READABLE>(name, get);
    }

    template <Flag_t FLAGS = NONE, class GET_PTR, class METADATA>
    constexpr MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS | READABLE, METADATA>
    makeMemberPropertyPointer(const char* name, const GET_PTR get, const std::nullptr_t, const METADATA metadata)
    {
        return MemberPropertyPointer<GET_PTR, std::nullptr_t, FLAGS | READABLE, METADATA>(name, get, metadata);
    }
}

#endif // CT_MEMBER_POINTER_HPP
