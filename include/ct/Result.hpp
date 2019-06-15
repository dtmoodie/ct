#ifndef CT_RESULT_HPP
#define CT_RESULT_HPP
#include "StringView.hpp"

namespace ct
{
    struct ErrorToken{};

    template <class T>
    struct Result
    {
        constexpr Result(const StringView msg, ErrorToken) : m_msg(msg) {}
        constexpr Result(T val) : m_value(std::move(val)) {}
        constexpr operator T() const { return value(); }
        constexpr bool success() const { return m_msg.empty(); }
        constexpr T value() const { return success() ? m_value : throw std::runtime_error(m_msg.toString()); }
        constexpr bool operator==(T v) const { return value() == v; }
        constexpr bool operator!=(T v) const { return value() != v; }
        T m_value;
        StringView m_msg;
    };

    template<class T>
    constexpr Result<T> success(T val)
    {
        return Result<T>(std::move(val));
    }

    template<class T>
    constexpr Result<T> success(Result<T> val)
    {
        return val;
    }

    template<class T>
    constexpr Result<T> error(StringView msg)
    {
        return Result<T>(msg, ErrorToken{});
    }
}
#endif // CT_RESULT_HPP
