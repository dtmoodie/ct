#include "Reflect.hpp"
#include "common.hpp"
#include <ct/reflect/print-container-inl.hpp>
#include <ct/reflect/print.hpp>
#include <ct/reflect/visitor.hpp>

#include <iostream>

struct StaticPrintStruct
{
    template <class T>
    ct::EnableIfReflected<T> test(const T&)
    {
        ct::printStructInfo<T>(std::cout);
    }

    template <class T>
    ct::DisableIfReflected<T> test(const T&)
    {
    }
};

struct PrintVisitorParams : public ct::DefaultVisitorParams
{
    constexpr static const bool ACCUMULATE_PATH = false;
};

struct PrintVisitor : public ct::VisitorBase<PrintVisitor, PrintVisitorParams>
{
    using Super_t = ct::VisitorBase<PrintVisitor, PrintVisitorParams>;

    template <class T, class... ARGS>
    ct::EnableIfReflected<T> visit(const T& obj, const std::string& name, ARGS&&... args)
    {
        thread_local bool recursion_block = false;

        if (recursion_block)
        {
            std::cout << name << ":" << ct::Reflect<T>::getName() << " ";
            return;
        }
        recursion_block = true;
        if (!name.empty())
        {
            std::cout << name << ":";
        }
        std::cout << "(";
        recurseFields(obj, name, ct::Reflect<T>::end(), std::forward<ARGS>(args)...);
        std::cout << ") ";

        recursion_block = false;
    }

    template <class T, class... ARGS>
    ct::DisableIfReflected<T> visit(const T&, const std::string&, ARGS&&...)
    {
    }

    template <ct::index_t I, class DTYPE, class... ARGS>
    ct::DisableIfReflected<typename std::decay<DTYPE>::type>
    visitData(const DTYPE& data, const std::string& path, ARGS&&...)
    {
        std::cout << path << ":" << data << ' ';
    }

    template <class R>
    ct::EnableIfReflected<R> visitReturn(R&& val, const std::string& path)
    {
        visit(val, path);
    }

    template <class R>
    ct::DisableIfReflected<R> visitReturn(R&& val, const std::string& path)
    {
        std::cout << path << ": " << val << ' ';
    }

    template <ct::index_t I, class DTYPE, class... ARGS>
    ct::EnableIfReflected<typename std::decay<DTYPE>::type>
    visitData(const DTYPE& data, const std::string& path, ARGS&&... args)
    {
        Super_t::template visitData<I>(data, path, std::forward<ARGS>(args)...);
    }

    template <class T, ct::index_t I, class GET_PTR, class SET_PTR, ct::Flag_t FLAGS, class METADATA, class... ARGS>
    void visitProperty(T& obj,
                       std::string path,
                       ct::MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA> ptr,
                       ct::Indexer<I>,
                       ARGS&&...)
    {
        if (VisitorParams::ACCUMULATE_PATH)
        {
            path += VisitorParams::DELIMINATOR;
            path += ptr.m_name.toString();
        }
        else
        {
            path = ptr.m_name.toString();
        }
        visitData<I>(ct::get(ptr, obj), path);
    }
};

struct Printer
{
    template <class T>
    void test(T& data)
    {
        std::cout << "\n====================\n";
        std::cout << ct::Reflect<T>::getName() << std::endl;
        ct::printStruct<ct::PrintAllOptions>(std::cout, data);
        std::cout << std::endl;
        StaticPrintStruct str;
        str.test(data);

        std::cout << std::endl;

        PrintVisitor visitor;
        std::cout << std::endl;
        visitor.visit(static_cast<const T&>(data), "");
        // printStructInfo<T>(std::cout);
    }
};

int main()
{
    Printer printer;
    testTypes(printer);

    std::cout << std::endl;
    ct::Reflect<DerivedC>::printHierarchy(std::cout);
}
