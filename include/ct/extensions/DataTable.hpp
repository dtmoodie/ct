#ifndef CT_EXTENSIONS_DATA_TABLE_HPP
#define CT_EXTENSIONS_DATA_TABLE_HPP
#include "../reflect.hpp"
#include "../type_traits.hpp"
#include "../types/TArrayView.hpp"

#include <tuple>
#include <vector>
#include <cassert>

namespace ct
{
    namespace ext
    {

        template <class T>
        struct DataTableStorage
        {
            T& operator[](size_t idx) { return m_data[idx]; }

            const T& operator[](size_t idx) const { return m_data[idx]; }

            T* data(size_t idx) { return &m_data[idx]; }

            const T* data(size_t idx) const { return &m_data[idx]; }

            void reserve(size_t size) { m_data.reserve(size); }

            void push_back(const T& val) { m_data.push_back(val); }

            size_t size() const { return m_data.size(); }
            void resizeSubarray(size_t) {}
          private:
            std::vector<T> m_data;
        };

        // Dynamic Array of fixed size per element in the data table
        // IE each element in the data table has an array that is of the same size
        // as all other elements in that table, size is determined dynamically.

        template <class T>
        struct DataTableArrayIterator
        {

            TArrayView<T> operator*() { return {begin, stride}; }

            TArrayView<T> operator->() { return {begin, stride}; }

            TArrayView<const T> operator*() const { return {begin, stride}; }

            TArrayView<const T> operator->() const { return {begin, stride}; }

            DataTableArrayIterator& operator++()
            {
                begin += stride;
                return *this;
            }

            DataTableArrayIterator& operator--()
            {
                begin -= stride;
                return *this;
            }

            DataTableArrayIterator operator++(int32_t val) { return {begin + stride * val, stride}; }

            DataTableArrayIterator operator--(int32_t val) { return {begin - stride * val, stride}; }

            T* begin;
            size_t stride;
        };

        template <class T>
        struct DataTableArrayIterator<const T>
        {
            TArrayView<const T> operator*() const { return {begin, stride}; }

            TArrayView<const T> operator->() const { return {begin, stride}; }

            DataTableArrayIterator& operator++()
            {
                begin += stride;
                return *this;
            }

            DataTableArrayIterator& operator--()
            {
                begin -= stride;
                return *this;
            }

            DataTableArrayIterator operator++(int32_t val) { return {begin + stride * val, stride}; }

            DataTableArrayIterator operator--(int32_t val) { return {begin - stride * val, stride}; }

            T* begin;
            size_t stride;
        };

        template <class T>
        struct DataTableStorage<TArrayView<T>>
        {
            TArrayView<T> operator[](size_t idx) { return {&m_data[idx * m_size], m_size}; }

            TArrayView<const T> operator[](size_t idx) const { return {&m_data[idx * m_size], m_size}; }

            DataTableArrayIterator<T> data(size_t idx) { return {&m_data[idx * m_size], m_size}; }

            DataTableArrayIterator<const T> data(size_t idx) const { return {&m_data[idx * m_size], m_size}; }

            void reserve(size_t size) { m_data.reserve(size * m_size); }
            void push_back(const TArrayView<T>& val)
            {
                if (m_size == 0 && m_data.size() == 0)
                {
                    resizeSubarray(val.size());
                }
                assert(val.size() == m_size);
                const auto idx = m_data.size();
                m_data.resize(m_data.size() + m_size);
                memcpy(&m_data[idx], val.data(), m_size * sizeof(T));
            }
            size_t size() const { return m_data.size(); }
            size_t stride() const { return m_size; }
            void resizeSubarray(size_t size)
            {
                if (m_size != size)
                {
                    if (m_size != 0)
                    {
                        const size_t elems = m_data.size() / m_size;
                        std::vector<T> data(elems * size);
                        for (size_t i = 0; i < elems; ++i)
                        {
                            memcpy(&data[i * size], &m_data[i * m_size], m_size * sizeof(T));
                        }
                        m_data = std::move(data);
                    }

                    m_size = size;
                }
            }

          private:
            std::vector<T> m_data;
            size_t m_size = 0;
        };

        template <class... Ts>
        struct DefaultStoragePolicy
        {
            std::tuple<DataTableStorage<Ts>...> m_data;
            template <index_t I>
            auto get() -> decltype(std::get<I>(m_data))
            {
                return std::get<I>(m_data);
            }
        };

        template <class DTYPE, template <class...> class STORAGE_POLICY, class T>
        struct DataTableBase
        {
        };

        template <class U, template <class...> class STORAGE_POLICY, class... Args>
        struct DataTableBase<U, STORAGE_POLICY, VariadicTypedef<Args...>> : public STORAGE_POLICY<Args...>
        {
            using Super = STORAGE_POLICY<Args...>;
            DataTableBase() { fillOffsets(Reflect<U>::end()); }

            void push_back(const U& data) { push(data, ct::Reflect<U>::end()); }

            template <class T>
            T& access(T U::*mem_ptr, const size_t idx)
            {
                void* p = ptr(memberOffset(mem_ptr), idx, Reflect<U>::end());
                return *static_cast<T*>(p);
            }

            template <class T>
            const T& access(T U::*mem_ptr, const size_t idx) const
            {
                const void* p = ptr(memberOffset(mem_ptr), idx, Reflect<U>::end());
                return *static_cast<const T*>(p);
            }

            template <class T>
            T* begin(T U::*mem_ptr)
            {
                void* p = ptr(memberOffset(mem_ptr), 0, Reflect<U>::end());
                return static_cast<T*>(p);
            }

            template <class T>
            const T* begin(T U::*mem_ptr) const
            {
                const void* p = ptr(memberOffset(mem_ptr), 0, Reflect<U>::end());
                return static_cast<const T*>(p);
            }

            template <class T>
            T* end(T U::*mem_ptr)
            {
                void* p = ptr(memberOffset(mem_ptr), Super::template get<0>().size(), Reflect<U>::end());
                return static_cast<T*>(p);
            }

            template <class T>
            const T* end(T U::*mem_ptr) const
            {
                const void* p = ptr(memberOffset(mem_ptr), Super::template get<0>().size(), Reflect<U>::end());
                return static_cast<const T*>(p);
            }

            U access(const size_t idx)
            {
                U out;
                populateData(out, Reflect<U>::end(), idx);
                return out;
            }

            void reserve(const size_t size) { reserveImpl(size, Reflect<U>::end()); }

            template <class T>
            void resizeSubarray(T U::*mem_ptr, size_t size)
            {
                resizeSubarrayImpl(memberOffset(mem_ptr), size, Reflect<U>::end());
            }

            // Currently not const since DataTableArray returns a pointer into the data table which is potentially
            // mutable
            // Need to work some template magic to make it possible to return a const ptr :/
            U operator[](size_t idx) { return access(idx); }

            template <class T>
            const DataTableStorage<T>& storage(T U::*mem_ptr) const
            {
                const void* out = storageImpl<DataTableStorage<T>>(memberOffset(mem_ptr), Reflect<U>::end());
                assert(out != nullptr);
                return *static_cast<const DataTableStorage<T>*>(out);
            }

            template <class T>
            DataTableStorage<T>& storage(T U::*mem_ptr)
            {
                void* out = storageImpl<DataTableStorage<T>>(memberOffset(mem_ptr), Reflect<U>::end());
                assert(out != nullptr);
                return *static_cast<DataTableStorage<T>*>(out);
            }

          private:
            void reserveImpl(const size_t size, const ct::Indexer<0>) { Super::template get<0>().reserve(size); }

            template <index_t I>
            void reserveImpl(const size_t size, const ct::Indexer<I> idx)
            {
                Super::template get<I>().reserve(size);
                reserveImpl(size, --idx);
            }

            void populateData(U& data, const ct::Indexer<0> idx, const size_t row)
            {
                const auto accessor = Reflect<U>::getPtr(idx);
                accessor.set(data, Super::template get<0>()[row]);
            }

            template <index_t I>
            void populateData(U& data, const ct::Indexer<I> idx, const size_t row)
            {
                const auto accessor = Reflect<U>::getPtr(idx);
                accessor.set(data, Super::template get<I>()[row]);
                populateData(data, --idx, row);
            }

            void push(const U& data, const ct::Indexer<0> idx)
            {
                const auto accessor = Reflect<U>::getPtr(idx);
                Super::template get<0>().push_back(accessor.get(data));
            }

            template <index_t I>
            void push(const U& data, const ct::Indexer<I> idx)
            {
                const auto accessor = Reflect<U>::getPtr(idx);
                Super::template get<I>().push_back(accessor.get(data));
                push(data, --idx);
            }

            void* ptr(const size_t offset, const size_t index, const ct::Indexer<0>)
            {
                if (offset == m_field_offsets[0])
                {
                    return (Super::template get<0>().data(index));
                }
                return nullptr;
            }

            template <index_t I>
            void* ptr(const size_t offset, const size_t index, const ct::Indexer<I> field_index)
            {
                if (offset == m_field_offsets[I])
                {
                    return (Super::template get<I>().data(index));
                }
                return ptr(offset, index, --field_index);
            }

            const void* ptr(const size_t offset, const size_t index, ct::Indexer<0>) const
            {
                if (offset == m_field_offsets[0])
                {
                    return (Super::template get<0>().data(index));
                }
                return nullptr;
            }

            template <index_t I>
            const void* ptr(const size_t offset, const size_t index, ct::Indexer<I> field_index) const
            {
                if (offset == m_field_offsets[I])
                {
                    return (Super::template get<I>().data(index));
                }
                return ptr(offset, index, --field_index);
            }

            template <class T>
            const void* storageImpl(const size_t offset, ct::Indexer<0>) const
            {
                if (std::is_same<typename std::decay<decltype(Super::template get<0>())>::type, T>::value &&
                    offset == m_field_offsets[0])
                {
                    return &Super::template get<0>();
                }
                return nullptr;
            }

            template <class T, index_t I>
            const void* storageImpl(const size_t offset, ct::Indexer<I> field_index) const
            {
                if (std::is_same<typename std::decay<decltype(Super::template get<I>())>::type, T>::value &&
                    offset == m_field_offsets[I])
                {
                    return &Super::template get<I>();
                }
                return storageImpl<T>(offset, --field_index);
            }

            template <class T>
            void* storageImpl(const size_t offset, ct::Indexer<0>)
            {
                if (std::is_same<typename std::decay<decltype(Super::template get<0>())>::type, T>::value &&
                    offset == m_field_offsets[0])
                {
                    return &Super::template get<0>();
                }
                return nullptr;
            }

            template <class T, index_t I>
            void* storageImpl(const size_t offset, ct::Indexer<I> field_index)
            {
                if (std::is_same<typename std::decay<decltype(Super::template get<I>())>::type, T>::value &&
                    offset == m_field_offsets[I])
                {
                    return &Super::template get<I>();
                }
                return storageImpl<T>(offset, --field_index);
            }

            void fillOffsets(const Indexer<0> idx)
            {
                const auto accessor = Reflect<U>::getPtr(idx);
                const auto field_offset = memberOffset(accessor.m_ptr);
                m_field_offsets[0] = field_offset;
            }

            template <index_t I>
            void fillOffsets(const Indexer<I> idx)
            {
                const auto accessor = Reflect<U>::getPtr(idx);
                const auto field_offset = memberOffset(accessor.m_ptr);
                m_field_offsets[I] = field_offset;
                fillOffsets(--idx);
            }

            void resizeSubarrayImpl(const size_t offset, const size_t size, ct::Indexer<0>)
            {
                if (offset == m_field_offsets[0])
                {
                    Super::template get<0>().resizeSubarray(size);
                    return;
                }
            }

            template <index_t I>
            void resizeSubarrayImpl(const size_t offset, const size_t size, ct::Indexer<I> idx)
            {
                if (offset == m_field_offsets[I])
                {
                    Super::template get<I>().resizeSubarray(size);
                    return;
                }
                resizeSubarrayImpl(offset, size, --idx);
            }

            // storage
            // std::tuple<DataTableStorage<Args>...> m_data;
            std::array<size_t, sizeof...(Args)> m_field_offsets;
        };

        template <class T, template <class...> class STORAGE_POLICY = DefaultStoragePolicy>
        struct DataTable : public DataTableBase<T, STORAGE_POLICY, typename ct::GlobMemberObjects<T>::types>
        {
        };
    }
}

#endif // CT_EXTENSIONS_DATA_TABLE_HPP
