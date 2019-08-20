#ifndef CT_REFLECT_MEMBER_FUNCTION_POINTER_HPP
#define CT_REFLECT_MEMBER_FUNCTION_POINTER_HPP
#include "../Indexer.hpp"
#include "../StringView.hpp"
#include "../VariadicTypedef.hpp"
#include "../bind.hpp"
#include "../flags.hpp"
#include "../types.hpp"
#include "pointer_traits.hpp"

namespace ct
{

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
        constexpr static const Flag_t DataFlags = FLAGS;

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
        constexpr StringView name() const { return m_name; }
        template <index_t I>
        constexpr auto getPtr() const -> decltype(std::get<I>(m_ptrs))
        {
            return std::get<I>(m_ptrs);
        }
    };

    template <class T, Flag_t FLAGS, class... PTRS>
    struct MemberFunctionPointers<T, FLAGS, void, PTRS...>
    {
        StringView m_name;
        std::tuple<MemberFunction<T, PTRS>...> m_ptrs;
        constexpr static const Flag_t DataFlags = FLAGS;
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

        constexpr StringView name() const { return m_name; }
        template <index_t I>
        constexpr auto getPtr() const -> decltype(std::get<I>(m_ptrs))
        {
            return std::get<I>(m_ptrs);
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

    template <class T, Flag_t FLAGS = 0, class METADATA = void, class... PTRS>
    struct StaticFunctions
    {
        StringView m_name;
        METADATA m_metadata;
        std::tuple<StaticFunction<T, PTRS>...> m_ptrs;
        constexpr static const Flag_t DataFlags = FLAGS;

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
        constexpr StringView name() const { return m_name; }
        template <index_t I>
        constexpr auto getPtr() const -> decltype(std::get<I>(m_ptrs))
        {
            return std::get<I>(m_ptrs);
        }
    };

    template <class T, Flag_t FLAGS, class... PTRS>
    struct StaticFunctions<T, FLAGS, void, PTRS...>
    {
        StringView m_name;
        std::tuple<StaticFunction<T, PTRS>...> m_ptrs;
        constexpr static const Flag_t DataFlags = FLAGS;

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
        constexpr StringView name() const { return m_name; }
        template <index_t I>
        constexpr auto getPtr() const -> decltype(std::get<I>(m_ptrs))
        {
            return std::get<I>(m_ptrs);
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

    template <class T, Flag_t FLAGS = Flags::DO_NOT_SERIALIZE, class... ARGS>
    constexpr MemberFunctionPointers<T, FLAGS | Flags::INVOKABLE, void, ARGS...>
    makeMemberFunctionPointers(const char* name, const ARGS... args)
    {
        return MemberFunctionPointers<T, FLAGS | Flags::INVOKABLE, void, ARGS...>(name, args...);
    }

    template <class T, Flag_t FLAGS = Flags::DO_NOT_SERIALIZE, class METADATA, class... ARGS>
    constexpr MemberFunctionPointers<T, FLAGS | Flags::INVOKABLE, METADATA, ARGS...>
    makeMemberFunctionPointersWithMetadata(const char* name, const METADATA metadata, const ARGS... args)
    {
        return MemberFunctionPointers<T, FLAGS | Flags::INVOKABLE, METADATA, ARGS...>(name, metadata, args...);
    }

    template <class T, Flag_t FLAGS = Flags::DO_NOT_SERIALIZE, class... ARGS>
    constexpr StaticFunctions<T, FLAGS | Flags::INVOKABLE, void, ARGS...> makeStaticFunctionPointers(StringView name,
                                                                                                     const ARGS... args)
    {
        return StaticFunctions<T, FLAGS | Flags::INVOKABLE, void, ARGS...>(name, args...);
    }

    template <class T, class METADATA, Flag_t FLAGS = Flags::DO_NOT_SERIALIZE, class... ARGS>
    constexpr StaticFunctions<T, FLAGS | Flags::INVOKABLE, METADATA, ARGS...>
    makeStaticFunctionPointersWithMetadata(StringView name, const METADATA metadata, const ARGS... args)
    {
        return StaticFunctions<T, FLAGS | Flags::INVOKABLE, METADATA, ARGS...>(name, metadata, args...);
    }
}

#endif // CT_REFLECT_MEMBER_FUNCTION_POINTER_HPP
