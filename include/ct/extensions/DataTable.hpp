#ifndef CT_EXTENSIONS_DATA_TABLE_HPP
#define CT_EXTENSIONS_DATA_TABLE_HPP
#include "../reflect.hpp"
#include "../reflect_traits.hpp"
#include "../type_traits.hpp"
#include "../types/TArrayView.hpp"
#include "datatable/DataTableArrayIterator.hpp"
#include "datatable/DataTableStorage.hpp"
#include "datatable/DataTableBase.hpp"

#include <cassert>
#include <tuple>
#include <vector>

namespace ct
{
    namespace ext
    {
        template <class U, template <class...> class STORAGE_POLICY = DefaultStoragePolicy>
        struct DataTable : public DataTableBase<U, STORAGE_POLICY, typename ct::GlobMemberObjects<U>::types>
        {
            using Super = typename DataTableBase<U, STORAGE_POLICY, typename ct::GlobMemberObjects<U>::types>::Super;

            void push_back(const U& data);

            template <class T>
            T& access(T U::*mem_ptr, const size_t idx);

            template <class T>
            const T& access(T U::*mem_ptr, const size_t idx) const;

            template<class T>
            TArrayView<T> access(TArrayView<T> U::*mem_ptr, const size_t idx);

            template<class T>
            TArrayView<const T> access(TArrayView<T> U::*mem_ptr, const size_t idx) const;

            template <class T>
            T* begin(T U::*mem_ptr);

            template <class T>
            const T* begin(T U::*mem_ptr) const;

            template <class T>
            T* end(T U::*mem_ptr);

            template <class T>
            const T* end(T U::*mem_ptr) const;

            U access(const size_t idx);

            void reserve(const size_t size);

            template <class T>
            void resizeSubarray(T U::*mem_ptr, size_t size);

            // Currently not const since DataTableArray returns a pointer into the data table which is potentially
            // mutable
            // Need to work some template magic to make it possible to return a const ptr :/
            U operator[](size_t idx);

            template <class T>
            const DataTableStorage<T>& storage(T U::*mem_ptr) const;

            template <class T>
            DataTableStorage<T>& storage(T U::*mem_ptr);

            size_t size() const override;
        };


        // IMPLEMENTATION
        template<class U, template<class...> class STORAGE_POLICY>
        void DataTable<U, STORAGE_POLICY>::push_back(const U& data) { this->push(data, ct::Reflect<U>::end()); }


        template<class U, template<class...> class STORAGE_POLICY>
        template <class T>
        T& DataTable<U, STORAGE_POLICY>::access(T U::*mem_ptr, const size_t idx)
        {
            auto p = this->ptr(memberOffset(mem_ptr), idx, Reflect<U>::end());
            return p.template as<T>();
        }

        template<class U, template<class...> class STORAGE_POLICY>
        template <class T>
        const T& DataTable<U, STORAGE_POLICY>::access(T U::*mem_ptr, const size_t idx) const
        {
            auto p = this->ptr(memberOffset(mem_ptr), idx, Reflect<U>::end());
            return p.template as<T>();
        }

        template<class U, template<class...> class STORAGE_POLICY>
        template<class T>
        TArrayView<T> DataTable<U, STORAGE_POLICY>::access(TArrayView<T> U::*mem_ptr, const size_t idx)
        {
            auto p = this->ptr(memberOffset(mem_ptr), idx, Reflect<U>::end());
            return *p;
        }

        template<class U, template<class...> class STORAGE_POLICY>
        template<class T>
        TArrayView<const T> DataTable<U, STORAGE_POLICY>::access(TArrayView<T> U::*mem_ptr, const size_t idx) const
        {
            auto p = this->ptr(memberOffset(mem_ptr), idx, Reflect<U>::end());
            return *p;
        }

        template<class U, template<class...> class STORAGE_POLICY>
        template <class T>
        T* DataTable<U, STORAGE_POLICY>::begin(T U::*mem_ptr)
        {
            auto p = this->ptr(memberOffset(mem_ptr), 0, Reflect<U>::end());
            return p.template ptr<T>();
        }

        template<class U, template<class...> class STORAGE_POLICY>
        template <class T>
        const T* DataTable<U, STORAGE_POLICY>::begin(T U::*mem_ptr) const
        {
            auto p = this->ptr(memberOffset(mem_ptr), 0, Reflect<U>::end());
            return p.template ptr<T>();
        }

        template<class U, template<class...> class STORAGE_POLICY>
        template <class T>
        T* DataTable<U, STORAGE_POLICY>::end(T U::*mem_ptr)
        {
            auto p = this->ptr(memberOffset(mem_ptr), Super::template get<0>().size(), Reflect<U>::end());
            return p.template ptr<T>();
        }

        template<class U, template<class...> class STORAGE_POLICY>
        template <class T>
        const T* DataTable<U, STORAGE_POLICY>::end(T U::*mem_ptr) const
        {
            auto p = this->ptr(memberOffset(mem_ptr), Super::template get<0>().size(), Reflect<U>::end());
            return p.template ptr<T>();
        }

        template<class U, template<class...> class STORAGE_POLICY>
        U DataTable<U, STORAGE_POLICY>::access(const size_t idx)
        {
            U out;
            this->populateData(out, Reflect<U>::end(), idx);
            return out;
        }

        template<class U, template<class...> class STORAGE_POLICY>
        void DataTable<U, STORAGE_POLICY>::reserve(const size_t size) { this->reserveImpl(size, Reflect<U>::end()); }

        template<class U, template<class...> class STORAGE_POLICY>
        template <class T>
        void DataTable<U, STORAGE_POLICY>::resizeSubarray(T U::*mem_ptr, size_t size)
        {
            this->resizeSubarrayImpl(memberOffset(mem_ptr), size, Reflect<U>::end());
        }

        template<class U, template<class...> class STORAGE_POLICY>
        U DataTable<U, STORAGE_POLICY>::operator[](size_t idx) { return access(idx); }

        template<class U, template<class...> class STORAGE_POLICY>
        template <class T>
        const DataTableStorage<T>& DataTable<U, STORAGE_POLICY>::storage(T U::*mem_ptr) const
        {
            const void* out = this->template storageImpl<DataTableStorage<T>>(memberOffset(mem_ptr), Reflect<U>::end());
            assert(out != nullptr);
            return *static_cast<const DataTableStorage<T>*>(out);
        }

        template<class U, template<class...> class STORAGE_POLICY>
        template <class T>
        DataTableStorage<T>& DataTable<U, STORAGE_POLICY>::storage(T U::*mem_ptr)
        {
            void* out = this->template storageImpl<DataTableStorage<T>>(memberOffset(mem_ptr), Reflect<U>::end());
            assert(out != nullptr);
            return *static_cast<DataTableStorage<T>*>(out);
        }

        template<class U, template<class...> class STORAGE_POLICY>
        size_t DataTable<U, STORAGE_POLICY>::size() const
        {
            return Super::template get<0>().size();
        }

    }
}
#include "datatable/print.hpp"

#endif // CT_EXTENSIONS_DATA_TABLE_HPP
