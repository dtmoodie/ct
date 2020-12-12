#ifndef CT_TYPENAME_HPP
#define CT_TYPENAME_HPP
#ifdef CT_HAVE_CONSTEXPR_NAME
#include "CompileTimeString.hpp"
#endif
#include "StringView.hpp"
#include "config.hpp"
#include "macros.hpp"
#include "reflect_forward.hpp"

#include <cstdint>
#include <map>

namespace ct
{
    namespace detail
    {
        constexpr StringView parseClassNameGCC(const StringView name)
        {
            return name.slice(ssize_t(name.rfind('=') + 2), ssize_t(name.size() - 1));
        }

        constexpr StringView
        parseClassNameMSVCHelper(const StringView name, size_t struct_pos, size_t /*TODO class_pos*/)
        {
            return name
                .slice((struct_pos != StringView::npos ? ssize_t(struct_pos + 7) : ssize_t(name.find("C<") + 2)),
                       ssize_t(name.rfind('>') - 1))
                .strip();
        }

        constexpr StringView parseClassNameMSVC(const StringView name)
        {
            return parseClassNameMSVCHelper(name, name.find("struct"), name.find("class"));
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
    template <class T, class A>
    struct GetName<std::vector<T, A>>
    {
#ifdef CT_HAVE_CONSTEXPR_NAME
        static constexpr const auto prefix = makeCTS("std::vector<");
        static constexpr const auto prefix_len = prefix.size();
        static constexpr const auto postfix = makeCTS(">");
        static constexpr const auto postfix_len = postfix.size();

        static constexpr const auto substring_ = ct::Reflect<T>::getTypeName();
        static constexpr const auto substring_len = substring_.size();
        static constexpr const auto substring = makeCTS<substring_len + 1>(substring_);

        static constexpr const size_t total_len = prefix_len + substring_len + postfix_len;
        static constexpr const CompileTimeString<total_len - 1> name = (prefix + substring) + postfix;

        static constexpr StringView funcName() { return ""; }
        static constexpr StringView getName()
        {
            return StringView(name.data, prefix_len + substring_len + postfix_len - 2);
        }
#else
        static constexpr StringView funcName() { return ""; }
        static const StringView getName()
        {
            static const std::string name = "std::vector<" + std::string(ct::GetName<T>::getName()) + ">";
            return StringView(name.data(), name.size());
        }
#endif
    };
#ifdef CT_HAVE_CONSTEXPR_NAME
    template <class T, class A>
    constexpr const CompileTimeString<GetName<std::vector<T, A>>::total_len - 1> GetName<std::vector<T, A>>::name;
#endif

    template <class K, class V, class C, class A>
    struct GetName<std::map<K, V, C, A>>
    {
#ifdef CT_HAVE_CONSTEXPR_NAME
        static constexpr const auto prefix = makeCTS("std::map<");
        static constexpr const auto prefix_len = prefix.size();

        static constexpr const auto deliminator = makeCTS(", ");
        static constexpr const auto deliminator_len = deliminator.size();

        static constexpr const auto postfix = makeCTS(">");
        static constexpr const auto postfix_len = postfix.size();

        static constexpr const auto substring_a_ = ct::Reflect<K>::getName();
        static constexpr const auto substring_a_len = substring_a_.size();
        static constexpr const auto substring_a = makeCTS<substring_a_len + 1>(substring_a_);

        static constexpr const auto substring_b_ = ct::Reflect<V>::getName();
        static constexpr const auto substring_b_len = substring_b_.size();
        static constexpr const auto substring_b = makeCTS<substring_b_len + 1>(substring_b_);

        static constexpr const size_t total_len =
            prefix_len + substring_a_len + deliminator_len + substring_b_len + postfix_len;
        static constexpr const CompileTimeString<total_len - 2> name =
            ((((prefix + substring_a) + deliminator) + substring_b) + postfix);

        static constexpr StringView funcName() { return ""; }
        static constexpr StringView getName() { return StringView(name.data, total_len - 3); }
#else
        static constexpr StringView funcName() { return ""; }
        static const StringView getName()
        {
            static const std::string name = "std::map<" + std::string(ct::GetName<K>::getName()) + ", " +
                                            std::string(ct::GetName<V>::getName()) + ">";
            return StringView(name.data(), name.size());
        }
#endif
    };
#ifdef CT_HAVE_CONSTEXPR_NAME
    template <class K, class V, class C, class A>
    constexpr const CompileTimeString<GetName<std::map<K, V, C, A>>::total_len - 2> GetName<std::map<K, V, C, A>>::name;
#endif

    template <class K, class V>
    struct GetName<std::pair<K, V>>
    {
#ifdef CT_HAVE_CONSTEXPR_NAME
        static constexpr const auto prefix = makeCTS("std::pair<");
        static constexpr const auto prefix_len = prefix.size();

        static constexpr const auto deliminator = makeCTS(", ");
        static constexpr const auto deliminator_len = deliminator.size();

        static constexpr const auto postfix = makeCTS(">");
        static constexpr const auto postfix_len = postfix.size();

        static constexpr const auto substring_a_ = ct::Reflect<K>::getName();
        static constexpr const auto substring_a_len = substring_a_.size();
        static constexpr const auto substring_a = makeCTS<substring_a_len + 1>(substring_a_);

        static constexpr const auto substring_b_ = ct::Reflect<V>::getName();
        static constexpr const auto substring_b_len = substring_b_.size();
        static constexpr const auto substring_b = makeCTS<substring_b_len + 1>(substring_b_);

        static constexpr const size_t total_len =
            prefix_len + substring_a_len + deliminator_len + substring_b_len + postfix_len;
        static constexpr const CompileTimeString<total_len - 2> name =
            ((((prefix + substring_a) + deliminator) + substring_b) + postfix);

        static constexpr StringView funcName() { return ""; }
        static constexpr StringView getName() { return StringView(name.data, total_len - 3); }
#else
        static StringView funcName() { return ""; }
        static StringView getName()
        {
            static const std::string name = "std::pair<" + std::string(ct::Reflect<K>::getTypeName()) + ", " +
                                            std::string(ct::Reflect<V>::getTypeName()) + ">";
            return StringView(name.data(), name.size());
        }

#endif
    };
#ifdef CT_HAVE_CONSTEXPR_NAME
    template <class K, class V>
    constexpr const CompileTimeString<GetName<std::pair<K, V>>::total_len - 2> GetName<std::pair<K, V>>::name;
#endif

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
    DECL_NAME(void);
    DECL_NAME(char);
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
