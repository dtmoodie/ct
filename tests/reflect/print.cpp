#include "Reflect.hpp"
#include "common.hpp"
#include <ct/reflect/print-container-inl.hpp>
#include <ct/reflect/print.hpp>
#include <iostream>

template<class T, class C, ct::Flag_t FLAGS>
void printField(const ct::MemberObjectPointer<T C::*, FLAGS> ptr, std::ostream& os)
{
    os << "  Field  0x" << ct::pointerValue(ptr.m_ptr) << ct::Reflect<T>::getName() << " " << ptr.m_name << std::endl;

}

template<class T>
void printTypes(const ct::VariadicTypedef<T>, std::ostream& os)
{
    os << ct::Reflect<T>::getName() << ", ";
}

template<class T, class ... T1>
void printTypes(const ct::VariadicTypedef<T, T1...>, std::ostream& os)
{
    os << ct::Reflect<T>::getName() << ", ";
    printTypes(ct::VariadicTypedef<T1...>{}, os);
}

template<class T>
void printSignaturesHelper(const std::tuple<T>*, std::ostream& os)
{
    os << "  " << typeid(T).name();
}

template<class T, class ... PTRS>
void printSignaturesHelper(const std::tuple<T, PTRS...>*, std::ostream& os)
{
    os << "  " << typeid(T).name();
    printSignaturesHelper(static_cast<std::tuple<PTRS...>*>(nullptr), os);
}

template<class ... PTRS>
void printSignatures(const std::tuple<PTRS...>, std::ostream& os)
{
    printSignaturesHelper(static_cast<std::tuple<PTRS...>*>(nullptr), os);
}

template<ct::Flag_t FLAGS, class ... PTRS>
void printField(const ct::MemberFunctionPointers<FLAGS, PTRS...> ptr, std::ostream& os)
{
    os << "  Member Function  " << ptr.m_name << std::endl;
    os << "  ";
    printSignatures(ptr.m_ptrs, os);
    os << std::endl;
}

template<ct::Flag_t FLAGS, class GETTER, class SETTER>
void printField(const ct::MemberPropertyPointer<GETTER, SETTER, FLAGS> properties, std::ostream& os)
{
    os << "  Member Property  " << properties.m_name << std::endl;
}


template<class T>
void printStructInfoHelper(std::ostream& os, const ct::Indexer<0> idx)
{
    auto ptr = ct::Reflect<T>::getPtr(idx);
    printField(ptr, os);
}

template<class T, ct::index_t I>
void printStructInfoHelper(std::ostream& os, const ct::Indexer<I> idx)
{
    auto ptr = ct::Reflect<T>::getPtr(idx);
    printField(ptr, os);
    printStructInfoHelper<T>(os, --idx);
}

template<class T>
void printStructInfo(std::ostream& os)
{
    printStructInfoHelper<T>(os, ct::Reflect<T>::end());
}

struct StaticPrintStruct
{
    template<class T>
    ct::EnableIfReflected<T> test(const T&)
    {
        printStructInfo<T>(std::cout);
    }

    template<class T>
    ct::DisableIfReflected<T> test(const T&)
    {

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
        //printStructInfo<T>(std::cout);
    }
};

int main()
{
    Printer printer;
    testTypes(printer);

    //printTypes(ct::Reflect<DerivedC>::VisitationList_t{}, std::cout);
}
