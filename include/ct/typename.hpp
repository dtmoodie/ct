#ifndef CT_TYPENAME_HPP
#define CT_TYPENAME_HPP
#include "StringView.hpp"
#include "config.hpp"

namespace ct
{
    namespace detail
    {
        constexpr StringView parseClassNameGCC(const StringView name)
        {
            return name.slice(name.rfind('=') + 2, name.size() - 1);
        }

		// This isn't the greatest solution but in msvc if there is a templated argument then it inserts an additional whitespace after the name
		constexpr StringView parseClassNameMSVCMultiTemplate(const StringView name, ssize_t first, ssize_t last)
		{
            return name.slice(first, (name[static_cast<size_t>(last-1)] == ' ') ? last - 1 : last );
		}

        constexpr StringView parseClassNameMSVC(const StringView name)
        {
            return parseClassNameMSVCMultiTemplate(
                name, ct::findFirst(name.data(), ' ') + 1, ct::findLast(name.data(), '>'));
        }
    }

    // This works at compile time on gcc 5.4 but not 4.8 :(
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55425
    // Which is why CT_CONSTEXPR_NAME is constexpr on 5.4 and not 4.8
    // It futher is then broken again until like gcc 9. :/
    template <class T>
    struct GetNameGCC
    {
        static CT_CONSTEXPR_NAME const char* funcName() { return CT_FUNCTION_NAME; }
        static CT_CONSTEXPR_NAME StringView  getName() { return detail::parseClassNameGCC(funcName()); }
    };

    template <class T>
    struct GetNameMSVC
    {
        static constexpr StringView funcName() { return StringView(CT_FUNCTION_NAME); }
        static constexpr StringView  getName() { return detail::parseClassNameMSVC(funcName()); }
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
}

#define DECL_NAME_1(SYM)                                                                                              \
    template <>                                                                                                        \
    struct GetName<SYM>                                                                                               \
    {                                                                                                                  \
        static constexpr StringView  getName() { return #SYM; }                                                        \
    }

#define DECL_NAME_2(SYM, NAME)                                                                                         \
    template <>                                                                                                        \
    struct GetName<SYM>                                                                                                \
    {                                                                                                                  \
        static constexpr StringView  getName() { return #NAME; }                                                        \
    }

#ifdef _MSC_VER
#define DECL_NAME(...) CT_PP_CAT(CT_PP_OVERLOAD(DECL_NAME_, __VA_ARGS__)(__VA_ARGS__), CT_PP_EMPTY())
#else
#define DECL_NAME(...) CT_PP_OVERLOAD(DECL_NAME_, __VA_ARGS__)(__VA_ARGS__)
#endif

#endif // CT_TYPENAME_HPP
