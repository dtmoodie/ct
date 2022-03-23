#ifndef CT_REFLECT_METADATA_HPP
#define CT_REFLECT_METADATA_HPP
#include "../Indexer.hpp"
#include "../StringView.hpp"
#include "../type_traits.hpp"

#include <functional>

namespace ct
{

    struct Empty
    {
        template <class U>
        U* getMetadata(U* = nullptr)
        {
            return nullptr;
        }
    };

    template <class T, class... Ts>
    struct PackImpl : PackImpl<Ts...>
    {
        constexpr PackImpl(T arg, Ts... args) : PackImpl<Ts...>(args...), m_field(arg) {}

        template <class U>
        auto getMetadata(U*) -> EnableIf<std::is_same<T, U>::value, U*>
        {
            return &m_field;
        }

        template <class U>
        auto getMetadata(U* ptr) -> EnableIf<!std::is_same<T, U>::value, U*>
        {
            return PackImpl<Ts...>::getMetadata(ptr);
        }

      private:
        T m_field;
    };

    template <class T>
    struct PackImpl<T>
    {
        constexpr PackImpl(T arg) : m_field(arg) {}

        template <class U>
        auto getMetadata(U*) -> EnableIf<!std::is_same<T, U>::value, U*>
        {
            return nullptr;
        }

        template <class U>
        auto getMetadata(U*) -> EnableIf<std::is_same<T, U>::value, U*>
        {
            return &m_field;
        }

      private:
        T m_field;
    };

    template <class... T>
    struct Pack : PackImpl<T...>
    {
        constexpr Pack(T... args) : PackImpl<T...>(args...) {}

        template <class U>
        U* getMetadata()
        {
            return PackImpl<T...>::getMetadata(static_cast<U*>(nullptr));
        }
    };

    struct Description
    {
        constexpr Description(const char* desc) : m_desc(desc) {}

        template <class U>
        auto getMetadata(U* = nullptr) -> EnableIf<std::is_same<U, Description>::value, U*>
        {
            return this;
        }
        template <class U>
        auto getMetadata(U* = nullptr) -> EnableIf<!std::is_same<U, Description>::value, U*>
        {
            return nullptr;
        }

        const char* description() { return m_desc; }

        const char* m_desc;
    };

    template <class T>
    struct Initializer
    {
        constexpr Initializer(T (*ptr)(), StringView str) : m_ptr(ptr), m_str(str) {}

        T operator()() const { return m_ptr(); }

        T getInitialValue() const { return m_ptr(); }

        T (*m_ptr)();

        template <class U>
        auto getMetadata(U* = nullptr) -> EnableIf<std::is_same<U, Initializer<T>>::value, U*>
        {
            return this;
        }

        template <class U>
        auto getMetadata(U* = nullptr) -> EnableIf<!std::is_same<U, Initializer<T>>::value, U*>
        {
            return nullptr;
        }

        StringView m_str;
    };

    template <class T>
    constexpr Initializer<T> makeInitializer(T (*ptr)(), StringView str)
    {
        return Initializer<T>(ptr, str);
    }

    template <class... Ts>
    constexpr Pack<Ts...> makePack(Ts... args)
    {
        return Pack<Ts...>(args...);
    }

    template <class T>
    struct MetaDataType
    {
        using type = void;
    };

} // namespace ct

#endif // CT_REFLECT_METADATA_HPP
