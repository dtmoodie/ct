#pragma once

#define DETECT_CONSTEXPR(NAME, FUNC) \
template<typename Trait> \
struct Detect##NAME \
{ \
    template<size_t Value = Trait::FUNC> \
    constexpr static std::true_type do_call(int) { return std::true_type(); } \
    constexpr static std::false_type do_call(...) { return std::false_type(); } \
    static const bool value = do_call(0); \
}
