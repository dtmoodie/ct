#ifndef CT_REFLECT_PRINT_HPP
#define CT_REFLECT_PRINT_HPP
#include "../EnumBitset.hpp"
#include "../StringView.hpp"
#include "../enum.hpp"
#include "../reflect.hpp"
#include "visitor.hpp"

#include <ostream>
#include <string>
#include <typeinfo>

namespace ct
{
    struct PrintOptions
    {
        static constexpr bool print_name = true;
        static constexpr char const* name_begin = "\0";
        static constexpr char const* name_end = ":";

        static constexpr char const* value_begin = ": ";
        static constexpr char const* value_end = " ";

        static constexpr bool print_calculated_values = false;
        static constexpr char const* object_begin = "(";
        static constexpr char const* object_end = ") ";

        static constexpr char const* array_begin = "[";
        static constexpr char const* array_end = "]";

        static constexpr bool error_on_nonprintable = true;

        static constexpr bool print_description = false;
        static constexpr char const* description_begin = " (";
        static constexpr char const* description_end = ")";

        static constexpr bool print_type = false;
        static constexpr char const* type_begin = "<";
        static constexpr char const* type_end = ">";

        static constexpr bool indent = false;
        // overload this function to print something to the ostream when a datatype is not printable
        template <class T>
        static std::ostream& onUnprintable(std::ostream& os, const char* /*name*/, const T& /*data*/)
        {
            return os;
        }
    };

    struct SkipUnprintable : public PrintOptions
    {
        static const bool error_on_nonprintable = false;
        template <class T>
        static std::ostream& onUnprintable(std::ostream& os, const char* name, const T&)
        {
            os << "Unable to print '" << name << "' no << operator available for " << typeid(T).name();
            return os;
        }
    };

    struct PrintAllOptions : public PrintOptions
    {
        static const bool print_calculated_values = true;
    };

    template <class Options = PrintOptions, class T>
    auto printStruct(std::ostream& os, const T& obj) -> ct::EnableIfReflected<T>;

    // Print a structs compile time infor without instantiating an instance of it
    template <class T, class C, ct::Flag_t FLAGS, class METADATA>
    void printField(const ct::MemberObjectPointer<T C::*, FLAGS, METADATA> ptr, std::ostream& os)
    {
        os << "  Field  0x" << ct::memberOffset(ptr.m_ptr) << ' ' << ct::Reflect<T>::getName() << ' ' << ptr.m_name;
        os << std::endl;
    }

    template <class T, class PTR>
    void printSignaturesHelper(const std::tuple<ct::MemberFunction<T, PTR>>*, std::ostream& os)
    {
        os << "  " << typeid(PTR).name();
    }

    template <class T, class PTR, class... PTRS>
    void printSignaturesHelper(const std::tuple<ct::MemberFunction<T, PTR>, ct::MemberFunction<T, PTRS>...>*,
                               std::ostream& os)
    {
        os << "  " << typeid(PTR).name();
        printSignaturesHelper(static_cast<std::tuple<ct::MemberFunction<T, PTRS>...>*>(nullptr), os);
    }

    template <class T, class... PTRS>
    void printSignatures(const std::tuple<ct::MemberFunction<T, PTRS>...>, std::ostream& os)
    {
        printSignaturesHelper(static_cast<std::tuple<ct::MemberFunction<T, PTRS>...>*>(nullptr), os);
    }

    template <class T, ct::Flag_t FLAGS, class... PTRS>
    void printField(const ct::MemberFunctionPointers<T, FLAGS, PTRS...> ptr, std::ostream& os)
    {
        os << "  Member Function  " << ptr.m_name << std::endl;
        os << "  ";
        printSignatures(ptr.m_ptrs, os);
        os << std::endl;
    }

    template <ct::Flag_t FLAGS, class GETTER, class SETTER>
    void printField(const ct::MemberPropertyPointer<GETTER, SETTER, FLAGS> properties, std::ostream& os)
    {
        os << "  Member Property  " << properties.m_name << std::endl;
    }

    template <class T, ct::Flag_t FLAGS, class METADATA, class... PTRS>
    void printField(const ct::StaticFunctions<T, FLAGS, METADATA, PTRS...> ptrs, std::ostream& os)
    {
        os << "  Static function  " << ptrs.m_name;
    }

    template <class T>
    void printStructInfoHelper(std::ostream& os, const ct::Indexer<0> idx)
    {
        auto ptr = ct::Reflect<T>::getPtr(idx);
        printField(ptr, os);
    }

    template <class T, ct::index_t I>
    void printStructInfoHelper(std::ostream& os, const ct::Indexer<I> idx)
    {
        printStructInfoHelper<T>(os, --idx);
        auto ptr = ct::Reflect<T>::getPtr(idx);
        printField(ptr, os);
    }

    template <class T>
    void printStructInfo(std::ostream& os)
    {
        printStructInfoHelper<T>(os, ct::Reflect<T>::end());
    }

    template <class OPTS>
    struct PrintVisitorParams : public ct::DefaultVisitorParams
    {
        constexpr static const bool ACCUMULATE_PATH = false;

        template <class T>
        constexpr static bool visitMemberFunctions(T)
        {
            return OPTS::print_calculated_values;
        }
        template <class T>
        constexpr static bool visitMemberFunction(T)
        {
            return OPTS::print_calculated_values;
        }
        template <class T>
        constexpr static bool visitStaticFunctions(T)
        {
            return OPTS::print_calculated_values;
        }
        template <class T>
        constexpr static bool visitStaticFunction(T)
        {
            return OPTS::print_calculated_values;
        }
    };

    template <class PRINT_OPTIONS = PrintOptions, class PARAMS = PrintVisitorParams<PRINT_OPTIONS>>
    struct PrintVisitor : public ct::VisitorBase<PrintVisitor<PRINT_OPTIONS, PARAMS>, PARAMS>
    {
        using Super_t = ct::VisitorBase<PrintVisitor<PRINT_OPTIONS, PARAMS>, PARAMS>;

        template <class T>
        ct::EnableIfReflected<T> visit(const T& obj, const std::string& name, std::ostream& os)
        {
            thread_local bool recursion_block = false;

            if (recursion_block)
            {
                os << PRINT_OPTIONS::name_begin << name << PRINT_OPTIONS::name_end;
                os << PRINT_OPTIONS::type_begin << ct::Reflect<T>::getName() << PRINT_OPTIONS::type_end;
                return;
            }
            recursion_block = true;
            /*if (!name.empty())
            {
                indent(os);
                os << PRINT_OPTIONS::name_begin << name << PRINT_OPTIONS::name_end;
            }*/

            os << PRINT_OPTIONS::object_begin;
            if (PRINT_OPTIONS::indent)
            {
                ++m_indent;
            }
            Super_t::recurseFields(obj, name, ct::Reflect<T>::end(), os);
            os << PRINT_OPTIONS::object_end;

            if (PRINT_OPTIONS::indent)
            {
                --m_indent;
            }
            recursion_block = false;
        }

        template <class T>
        ct::DisableIfReflected<T> visit(const T& val, const std::string&, std::ostream& os)
        {
            os << val;
        }

        template <ct::index_t I, class DTYPE>
        ct::DisableIfReflected<typename std::decay<DTYPE>::type>
        visitData(const DTYPE& data, const std::string& path, std::ostream& os)
        {
            indent(os);
            os << path << PRINT_OPTIONS::value_begin << data << PRINT_OPTIONS::value_end;
        }

        template <bool ENABLE, class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA, class T, index_t I>
        EnableIf<ENABLE> visitMemberObject(const T& obj,
                                           std::string path,
                                           MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA> ptr,
                                           Indexer<I>,
                                           std::ostream& os)
        {
            if (PARAMS::ACCUMULATE_PATH)
            {
                path += ptr.m_name.toString();
            }
            else
            {
                path = ptr.m_name.toString();
            }

            indent(os);
            os << path;
            if (PRINT_OPTIONS::print_description)
            {
                auto desc = getMetadata<Description, I, T>();
                os << PRINT_OPTIONS::description_begin;
                if (desc)
                {
                    os << getMetadata<Description, I, T>()->description();
                }
                else
                {
                    os << "no description provided";
                }
                os << PRINT_OPTIONS::description_end;
            }
            os << PRINT_OPTIONS::value_begin;
            visit(ptr.get(obj), path, os);
            os << PRINT_OPTIONS::value_end;
        }

        template <class R>
        ct::EnableIfReflected<R> visitReturn(R&& val, const std::string& path, std::ostream& os)
        {
            visit(val, path, os);
        }

        template <class R>
        ct::DisableIfReflected<R> visitReturn(R&& val, const std::string& path, std::ostream& os)
        {
            indent(os);
            os << path << PRINT_OPTIONS::value_begin << val << PRINT_OPTIONS::value_end;
        }

        template <ct::index_t I, class DTYPE>
        ct::EnableIfReflected<typename std::decay<DTYPE>::type>
        visitData(const DTYPE& data, const std::string& path, std::ostream& os)
        {
            Super_t::template visitData<I>(data, path, os);
        }

        template <class T, ct::index_t I, class GET_PTR, class SET_PTR, ct::Flag_t FLAGS, class METADATA>
        void visitProperty(T& obj,
                           std::string path,
                           ct::MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA> ptr,
                           ct::Indexer<I>,
                           std::ostream& os)
        {
            if (PARAMS::ACCUMULATE_PATH)
            {
                path += PARAMS::DELIMINATOR;
                path += ptr.m_name.toString();
            }
            else
            {
                path = ptr.m_name.toString();
            }
            if (PRINT_OPTIONS::indent)
            {
                ++m_indent;
            }
            visitData<I>(ptr.get(obj), path, os);
            if (PRINT_OPTIONS::indent)
            {
                --m_indent;
            }
        }

      private:
        void indent(std::ostream& os)
        {
            if (PRINT_OPTIONS::indent)
            {
                for (uint32_t i = 0; i < m_indent; ++i)
                {
                    os << ' ';
                }
            }
        }
        uint32_t m_indent = 0;
    };

    template <class T>
    void printEnums(std::ostream& os, ct::Indexer<0> idx)
    {
        auto mdata = ct::Reflect<T>::getPtr(idx);
        os << mdata << '\n';
    }

    template <class T, ct::index_t I>
    void printEnums(std::ostream& os, ct::Indexer<I> idx)
    {
        printEnums<T>(os, --idx);
        auto mdata = ct::Reflect<T>::getPtr(idx);
        os << mdata << '\n';
    }

    template <class T>
    void printEnums(std::ostream& ios)
    {
        printEnums<T>(ios, ct::Reflect<T>::end());
    }

    template <class T>
    std::ostream& printEnum(std::ostream& os, T v)
    {
        if (!printEnumHelper(os, v, ct::Reflect<T>::end(), std::is_base_of<ct::BitsetTag, T>::value))
        {
            if (!printEnumHelper(os, v, ct::Reflect<T>::end(), true))
            {
                os << "Invalid value";
            }
        }
        return os;
    }

} // namespace ct

#include <cstdint>
#include <ct/type_traits.hpp>
#include <memory>

namespace std
{
    template <class T>
    auto operator<<(ostream& os, const T& obj)
        -> ct::EnableIf<ct::IsReflected<T>::value && !ct::EnumChecker<T>::value, ostream&>
    {
        thread_local bool recursion_block = false;
        if (recursion_block)
        {
            return os;
        }
        recursion_block = true;
        ct::printStruct(os, obj);
        recursion_block = false;
        return os;
    }

    template <class T, class A>
    auto operator<<(ostream& os, const vector<T, A>& vec) -> ct::EnableIf<ct::StreamWritable<T>::value, ostream&>
    {
        if (vec.empty())
        {
            os << "[empty]";
        }
        else
        {
            os << "[";
            for (size_t i = 0; i < vec.size(); ++i)
            {
                if (i != 0)
                    os << ' ';
                os << vec[i];
            }
            os << ']';
        }
        return os;
    }

    template <class T, size_t N>
    ostream& operator<<(ostream& os, const std::array<T, N>& arr)
    {
        os << "[";
        for (size_t i = 0; i < N; ++i)
        {
            if (i != 0)
                os << ' ';
            os << arr[i];
        }
        os << ']';
        return os;
    }

    template <class E, class T, T V, uint16_t I>
    auto operator<<(ostream& os, ct::EnumValue<E, T, V, I>) -> ct::EnableIf<!std::is_enum<T>::value, ostream&>
    {
        os << ct::Reflect<E>::getPtr(ct::Indexer<I>()).name << " ";
        os << V;
        return os;
    }

    template <class E, class T, T V, uint16_t I>
    auto operator<<(ostream& os, ct::EnumValue<E, T, V, I>) -> ct::EnableIf<std::is_enum<T>::value, ostream&>
    {
        os << ct::Reflect<E>::getPtr(ct::Indexer<I>()).name << " ";
        os << int(V);
        return os;
    }

    template <class E, uint16_t V, uint16_t I>
    ostream& operator<<(ostream& os, ct::BitsetIndex<E, V, I>)
    {
        os << ct::Reflect<E>::getPtr(ct::Indexer<I>()).name << " ";
        os << int(V);
        return os;
    }

    template <class E, unsigned char V, uint16_t I>
    ostream& operator<<(ostream& os, ct::EnumValue<E, unsigned char, V, I>)
    {
        os << ct::Reflect<E>::getPtr(ct::Indexer<I>()).name << " ";
        os << int(V);
        return os;
    }

    template <class T>
    ostream& operator<<(ostream& os, ct::EnumField<T> v)
    {
        if (std::is_same<decltype(v.value()), uint8_t>::value)
        {
            os << v.name << " " << static_cast<int32_t>(v.value());
        }
        else
        {
            auto value = v.value();
            os << value;
        }

        return os;
    }

    template <class T>
    bool printEnumHelper(ostream& os, T v, ct::Indexer<0> idx, bool check_bitwise, bool multi_value = false)
    {
        const auto value = ct::Reflect<T>::getPtr(idx).value();
        if (value == v || (check_bitwise && (value & v)))
        {
            if (multi_value)
            {
                os << "|";
            }
            os << ct::Reflect<T>::getPtr(idx).name;
            return true;
        }
        else
        {
            return false;
        }
    }

    template <class T, ct::index_t I>
    bool printEnumHelper(ostream& os, T v, ct::Indexer<I> idx, bool check_bitwise, bool multi_value = false)
    {
        const auto value = ct::Reflect<T>::getPtr(idx).value();
        bool success = false;
        const bool check_bitwise_ =
            check_bitwise || std::is_base_of<ct::BitsetTag, typename std::decay<decltype(value)>::type>::value;
        if (value == v || (check_bitwise_ && (value & v)))
        {
            if (multi_value)
            {
                os << "|";
            }
            os << ct::Reflect<T>::getPtr(idx).name;
            if (!check_bitwise_)
            {
                return true;
            }
            multi_value = true;
            const auto mask = ~value;
            v = T(v & mask);
            success = true;
            check_bitwise = false;
        }
        return printEnumHelper(os, v, --idx, check_bitwise, multi_value) || success;
    }

    template <class T>
    ct::EnableIfIsEnum<T, ostream&> operator<<(ostream& os, T v)
    {
        return ct::printEnum(os, v);
    }
} // namespace std

#include <sstream>

namespace ct
{

    template <class Options, class T>
    auto printStruct(std::ostream& os, const T& obj) -> ct::EnableIfReflected<T>
    {

        PrintVisitor<Options> visitor;
        visitor.visit(obj, "", os);
    }

    template <class Options = PrintOptions, class T>
    auto printStruct(std::ostream& os, const std::shared_ptr<T>& obj) -> ct::EnableIfReflected<T>
    {
        if (obj)
        {
            PrintVisitor<Options> visitor;
            visitor.visit(*obj, "", os);
        }
    }

    // https://cukic.co/2019/02/19/tmp-testing-and-debugging-templates/
    // Only really useful on clang
    template <class... TS>
    struct [[deprecated]] CompilerPrintTypes {};

    template <class T>
    ct::EnableIfIsEnum<T, std::string> toString(T val)
    {
        std::stringstream ss;
        ct::printEnum(ss, val);
        return std::move(ss).str();
    }
} // namespace ct

#endif
