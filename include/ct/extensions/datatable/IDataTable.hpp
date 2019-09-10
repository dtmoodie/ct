#ifndef CT_EXT_IDATA_TABLE_HPP
#define CT_EXT_IDATA_TABLE_HPP
#include "DataTableArrayIterator.hpp"

#include <ct/reflect.hpp>

namespace ct
{
    namespace ext
    {

        template <class T>
        struct ElementView
        {
            T* begin() const { return m_begin; }
            T* end() const { return m_end; }

            T* m_begin;
            T* m_end;
        };

        template <class DTYPE>
        struct IDataTableBase
        {
            virtual ~IDataTableBase() = default;
            template <class T, class U>
            T* begin(T U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<T> begin(TArrayView<T> U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p;
            }

            template <class T, class U>
            const T* begin(T U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<const T> begin(TArrayView<T> U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p;
            }

            template <class T, class U>
            T* end(T U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<T> end(TArrayView<T> U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p;
            }

            template <class T, class U>
            const T* end(T U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<const T> end(TArrayView<T> U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p;
            }

            template <class T, class U>
            ElementView<T> view(T U::*mem_ptr)
            {
                return {begin(mem_ptr), end(mem_ptr)};
            }

            template <class T, class U>
            ElementView<const T> view(T U::*mem_ptr) const
            {
                return {begin(mem_ptr), end(mem_ptr)};
            }

            virtual size_t size() const = 0;

            void populateData(DTYPE& out, size_t idx) { populateDataRecurse(out, idx, Reflect<DTYPE>::end()); }
          private:
            virtual DataTableArrayIterator<void> ptr(size_t offset, size_t idx) = 0;
            virtual DataTableArrayIterator<const void> ptr(size_t offset, size_t idx) const = 0;

            template <index_t I>
            void populateData(DTYPE& out, size_t row, Indexer<I> idx)
            {
                auto ptr = Reflect<DTYPE>::getPtr(idx);
                auto data = begin(ptr.m_ptr);
                ptr.set(out, data[row]);
            }

            void populateDataRecurse(DTYPE& out, size_t row, Indexer<0> idx) { populateData(out, row, idx); }

            template <index_t I>
            void populateDataRecurse(DTYPE& out, size_t row, Indexer<I> idx)
            {
                populateData(out, row, idx);
                populateDataRecurse(out, row, --idx);
            }
        };

        template <class DTYPE, class BASES = typename Reflect<DTYPE>::BaseTypes>
        struct IDataTable;

        template <class DTYPE>
        struct IDataTable<DTYPE, VariadicTypedef<>> : public IDataTableBase<DTYPE>
        {
        };

        template <class DTYPE, class... BASES>
        struct IDataTable<DTYPE, VariadicTypedef<BASES...>> : virtual public IDataTable<BASES>...
        {
        };
    }
}
#endif // CT_EXT_IDATA_TABLE_HPP
