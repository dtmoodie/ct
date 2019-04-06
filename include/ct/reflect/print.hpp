#ifndef CT_REFLECT_PRINT_HPP
#define CT_REFLECT_PRINT_HPP
#include "../reflect.hpp"
#include <ostream>
#include <typeinfo>

namespace ct
{
    struct PrintOptions
    {
        static const bool print_name = true;
        static const char name_separator = ':';
        static const char value_separator = ' ';
        static const bool print_calculated_values = false;
        static const char object_begin = '(';
        static const char object_end = ')';
        static const char array_begin = '[';
        static const char array_end = ']';
        static const bool error_on_nonprintable = true;
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
        static std::ostream& onUnprintable(std::ostream& os, const char* name, const T& data)
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
        os << "  Field  0x" << ct::pointerValue(ptr.m_ptr) << ct::Reflect<T>::getName() << " " << ptr.m_name
           << std::endl;
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
} // namespace ct

#include <cstdint>
#include <ct/type_traits.hpp>
#include <memory>

namespace std
{
    template <class T>
    auto operator<<(ostream& os, const T& obj) -> ct::EnableIfReflected<T, ostream&>
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
}

namespace ct
{
    // implementation
    template <class T, int I>
    struct CanWrite
    {
        using DType = typename FieldGetType<T, I>::type;
        enum
        {
            value = StreamWritable<typename std::decay<DType>::type>::value
        };
    };

    template <class T, class O, int I>
    struct ShouldWrite
    {
        using Ptr_t = PtrType<T, I>;
        using DType = typename GetType<Ptr_t>::type;
        constexpr static const bool is_calculated = IsMemberFunctionPointers<Ptr_t>::value;
        constexpr static const bool is_writable = CanWrite<T, I>::value;
        constexpr static const bool value =
            is_writable && ((is_calculated && O::print_calculated_values) || !is_calculated);

        static_assert(is_writable || !O::error_on_nonprintable,
                      "Either need to be able to write this data type or need to disable it in options");
    };

    template <int I, class Options = PrintOptions, class T>
    auto printParam(std::ostream& os, const T& obj)
        -> EnableIf<ShouldWrite<T, Options, I>::value && !IsMemberFunction<T, I>::value>
    {
        // If you get a "no type named 'type' in struct std::enable_if<false, void> here, it's because the type is not
        // stream writable and your print options do not allow that via the error_on_nonprintable flag

        auto accessor = Reflect<T>::getPtr(ct::Indexer<I>{});

        if (Options::print_name)
        {
            os << accessor.m_name << Options::name_separator;
        }

        os << accessor.get(obj);
    }

    template <class T, index_t I, class U = void>
    using EnableIfNoArgs = EnableIf<CountArgs<T, I>::NUM_ARGS == 0 && ConstFunction<T, I>::value, U>;

    template <class T, index_t I, class U = void>
    using EnableIfArgs = EnableIf<CountArgs<T, I>::NUM_ARGS >= 1 && ConstFunction<T, I>::value, U>;

    template <int I, class Options, class T>
    auto printMemberFunctionResult(std::ostream& os, const T& obj) -> EnableIfNoArgs<T, I>
    {
        auto accessor = Reflect<T>::getPtr(ct::Indexer<I>{});
        if (Options::print_name)
        {
            os << accessor.m_name << Options::name_separator;
        }

        os << accessor.template invoke<0>(obj);
    }

    template <int I, class Options, class T>
    auto printMemberFunctionResult(std::ostream&, const T&) -> EnableIf<!ConstFunction<T, I>::value>
    {
    }

    template <int I, class Options, class T>
    auto printMemberFunctionResult(std::ostream&, const T&) -> EnableIfArgs<T, I>
    {
    }

    template <int I, class Options = PrintOptions, class T>
    auto printParam(std::ostream& os, const T& obj)
        -> EnableIf<ShouldWrite<T, Options, I>::value && IsMemberFunction<T, I>::value>
    {
        // If you get a "no type named 'type' in struct std::enable_if<false, void> here, it's because the type is not
        // stream writable and your print options do not allow that via the error_on_nonprintable flag
        printMemberFunctionResult<I, Options, T>(os, obj);
    }

    template <int I, class Options = PrintOptions, class T>
    auto printParam(std::ostream& os, const T& data) -> EnableIf<!ShouldWrite<T, Options, I>::value>
    {
        if (!ShouldWrite<T, Options, I>::is_writable && !Options::error_on_nonprintable)
        {
            auto accessor = Reflect<T>::getPtr(ct::Indexer<I>{});
            Options::onUnprintable(os, Reflect<T>::getName(ct::Indexer<I>{}), accessor.invoke(data));
        }
    }

    template <int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& os, const T& obj) -> EnableIf<!IsMemberFunction<T, I>::value>
    {
        printParam<I, Options>(os, obj);
    }

    template <int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream& os, const T& obj)
        -> EnableIf<IsMemberFunction<T, I>::value && Options::print_calculated_values>
    {
        printMemberFunctionResult<I, Options>(os, obj);
    }

    template <int I, class Options = PrintOptions, class T>
    auto printValue(std::ostream&, const T&)
        -> EnableIf<IsMemberFunction<T, I>::value && !Options::print_calculated_values>
    {
    }

    template <class Options = PrintOptions, class T>
    void printStructHelper(std::ostream& os, const T& obj, const ct::Indexer<0U>)
    {
        printValue<0, Options>(os, obj);
    }

    template <class Options = PrintOptions, index_t I, class T>
    void printStructHelper(std::ostream& os, const T& obj, const ct::Indexer<I> idx)
    {
        printStructHelper<Options>(os, obj, --idx);
        os << Options::value_separator;
        printValue<I, Options>(os, obj);
    }

    template <class Options, class T>
    auto printStruct(std::ostream& os, const T& obj) -> ct::EnableIfReflected<T>
    {
        os << Options::object_begin;
        printStructHelper<Options>(os, obj, Reflect<T>::end());
        os << Options::object_end;
    }

    template <class Options = PrintOptions, class T>
    auto printStruct(std::ostream& os, const std::shared_ptr<T>& obj) -> ct::EnableIfReflected<T>
    {
        if (obj)
        {

            os << Options::object_begin;
            printStructHelper<Options>(os, *obj, Reflect<T>::end());
            os << Options::object_end;
        }
    }

    // https://cukic.co/2019/02/19/tmp-testing-and-debugging-templates/
    // Only really useful on clang
    template <class... TS>
    struct[[deprecated]] CompilerPrintTypes{};
}
#endif
