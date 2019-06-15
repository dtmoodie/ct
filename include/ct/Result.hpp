#ifndef CT_RESULT_HPP
#define CT_RESULT_HPP
#include "StringView.hpp"

namespace ct
{
    template <class T>
    struct Result
    {
        constexpr Result(const StringView msg) : m_msg(msg) {}
        constexpr Result(T val) : m_value(val) {}
        constexpr operator T() const { return value(); }
        constexpr bool success() const { return m_msg.empty(); }
        constexpr T value() const { return success() ? m_value : throw std::runtime_error(m_msg.toString()); }
        constexpr bool operator==(T v) const { return value() == v; }
        constexpr bool operator!=(T v) const { return value() != v; }
        T m_value;
        StringView m_msg;
    };
}
#endif // CT_RESULT_HPP
