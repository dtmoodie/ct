#ifndef CT_TYPENAME_HPP
#define CT_TYPENAME_HPP
#include "CompileTimeString.hpp"
#include "StringView.hpp"
#include "config.hpp"

#include <cstdint>

namespace ct
{
    namespace detail
    {
        constexpr StringView parseClassNameGCC(const StringView name)
        {
            return name.slice(name.rfind('=') + 2, name.size() - 1);
        }

        constexpr StringView parseClassNameMSVCHelper(const StringView name, size_t struct_pos)
        {
            return name
                .slice((struct_pos != StringView::npos ? struct_pos + 7 : name.find("C<") + 2), name.rfind('>') - 1)
                .strip();
        }

        constexpr StringView parseClassNameMSVC(const StringView name)
        {
            return parseClassNameMSVCHelper(name, name.find("struct"));
        }
    } // namespace detail

    // This works at compile time on gcc 5.4 but not 4.8 :(
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55425
    // Which is why CT_CONSTEXPR_NAME is constexpr on 5.4 and not 4.8
    // It futher is then broken again until like gcc 9. :/
    template <class T>
    struct GetNameGCC
    {
        static CT_CONSTEXPR_NAME const char* funcName() { return CT_FUNCTION_NAME; }
        static CT_CONSTEXPR_NAME StringView getName() { return detail::parseClassNameGCC(funcName()); }
    };

    template <class T>
    struct GetNameMSVC
    {
        static constexpr StringView funcName() { return StringView(CT_FUNCTION_NAME); }
        static constexpr StringView getName() { return detail::parseClassNameMSVC(funcName()); }
    };
#ifdef _MSC_VER
    template <class T>
    struct GetName : GetNameMSVC<T>
    {
    };
#else

    template <class T>
    struct GetName : GetNameGCC<T>
    {
    };
#endif

    // not generically ready for prime time
    /*template<class T, class A>
    struct GetName<std::vector<T, A>>
    {
        static constexpr const auto prefix = makeCTS("std::vector<");
        static constexpr const auto prefix_len = prefix.size();
        static constexpr const auto postfix = makeCTS(">");
        static constexpr const auto postfix_len = postfix.size();

        static constexpr const auto substring_ = ct::GetName<T>::getName();
        static constexpr const auto substring_len = substring_.size();
        static constexpr const auto substring = makeCTS<substring_len + 1>(substring_);

        static constexpr const size_t total_len = prefix_len + substring_len + postfix_len;
        static constexpr const CompileTimeString<total_len - 1> name = (prefix + substring) + postfix;
        static constexpr const auto test = (prefix + substring);
        static constexpr StringView funcName() { return ""; }
        static constexpr const StringView getName()
        {
            return StringView(name.data, prefix_len + substring_len + postfix_len - 2);
        }
    };*/

} // namespace ct

#define DECL_NAME_1(SYM)                                                                                               \
    template <>                                                                                                        \
    struct GetName<SYM>                                                                                                \
    {                                                                                                                  \
        static constexpr const char* funcName() { return ""; }                                                         \
        static constexpr StringView getName() { return #SYM; }                                                         \
    }

#define DECL_NAME_2(SYM, NAME)                                                                                         \
    template <>                                                                                                        \
    struct GetName<SYM>                                                                                                \
    {                                                                                                                  \
        static constexpr const char* funcName() { return ""; }                                                         \
        static constexpr StringView getName() { return #NAME; }                                                        \
    }

#ifdef _MSC_VER
#define DECL_NAME(...) CT_PP_CAT(CT_PP_OVERLOAD(DECL_NAME_, __VA_ARGS__)(__VA_ARGS__), CT_PP_EMPTY())
#else
#define DECL_NAME(...) CT_PP_OVERLOAD(DECL_NAME_, __VA_ARGS__)(__VA_ARGS__)
#endif

namespace ct
{
    DECL_NAME(float);
    DECL_NAME(double);
    DECL_NAME(uint8_t);
    DECL_NAME(int8_t);
    DECL_NAME(uint16_t);
    DECL_NAME(int16_t);
    DECL_NAME(uint32_t);
    DECL_NAME(int32_t);
    DECL_NAME(uint64_t);
    DECL_NAME(int64_t);
    DECL_NAME(std::string);
} // namespace ct

#endif // CT_TYPENAME_HPP
