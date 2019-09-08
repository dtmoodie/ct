#ifndef CT_EXT_DATA_TABLE_PRINT_HPP
#define CT_EXT_DATA_TABLE_PRINT_HPP
#include "IDataTable.hpp"
namespace ct
{
    namespace ext
    {
        template<class T, ct::index_t I>
        void printTableHeader(std::ostream& os, Indexer<I> idx)
        {
            if(I != 0)
            {
                os << ", ";
            }
            os << ct::Reflect<T>::getPtr(idx).getName();
        }


        template<class T>
        void printTableHeaderRecurse(std::ostream& os, Indexer<0> idx)
        {
            printTableHeader<T>(os, idx);
        }

        template<class T, ct::index_t I>
        void printTableHeaderRecurse(std::ostream& os, Indexer<I> idx)
        {
            printTableHeaderRecurse<T>(os, --idx);
            printTableHeader<T>(os, idx);
        }

        template<class T>
        void printTableHeader(std::ostream& os)
        {
            printTableHeaderRecurse<T>(os, ct::Reflect<T>::end());
        }

        template<class T, index_t I, class PTR>
        void printTableElement(std::ostream& os, const ext::IDataTable<T>& table, size_t i, Indexer<I>, PTR ptr)
        {
            auto begin = table.begin(ptr.m_ptr);
            if(I != 0)
            {
                os << ", ";
            }
            os << begin[i];
        }

        template<class T, index_t I, class DTYPE, class U, Flag_t FLAGS, class MDATA>
        void printTableElement(std::ostream& os, const ext::IDataTable<T>& table, size_t i, Indexer<I>, MemberObjectPointer<TArrayView<DTYPE> U::*, FLAGS, MDATA> ptr)
        {
            auto begin = table.begin(ptr.m_ptr);
            if(I != 0)
            {
                os << ", ";
            }
            os << begin.array(i);
        }

        template<class T, index_t I>
        void printTableElement(std::ostream& os, const ext::IDataTable<T>& table, size_t i, Indexer<I> idx)
        {
            printTableElement(os, table, i, idx, Reflect<T>::getPtr(idx));
        }

        template<class T>
        void printTableElementRecurse(std::ostream& os, const ext::IDataTable<T>& table, size_t i, Indexer<0> idx)
        {
            printTableElement(os, table, i, idx);
        }

        template<class T, index_t I>
        void printTableElementRecurse(std::ostream& os, const ext::IDataTable<T>& table, size_t i, Indexer<I> idx)
        {
            printTableElementRecurse(os, table, i, --idx);
            printTableElement(os, table, i, idx);
        }

        template<class T>
        void printTableElement(std::ostream& os, const ext::IDataTable<T>& table, size_t i)
        {
            printTableElementRecurse(os, table, i, Reflect<T>::end());
        }

        template<class T>
        void printTable(std::ostream& os, const ext::IDataTable<T>& table)
        {
            printTableHeader<T>(os);
            os << std::endl;
            auto size = table.size();
            for(size_t i = 0; i < size; ++i)
            {
                printTableElement(os, table, i);
                std::cout << std::endl;
            }
        }
    }
}


namespace std
{
    template<class T>
    ostream& operator << (ostream& os, const ct::ext::DataTable<T>& table)
    {
        ct::ext::printTable<T>(os, table);
        return os;
    }

    template<class T>
    ostream& operator << (ostream& os, const ct::ext::IDataTable<T>& table)
    {
        ct::ext::printTable<T>(os, table);
        return os;
    }
}

#endif // CT_EXT_DATA_TABLE_PRINT_HPP
