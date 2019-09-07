#ifndef CT_EXT_DATA_TABLE_BASE_HPP
#define CT_EXT_DATA_TABLE_BASE_HPP
#include "IDataTable.hpp"

namespace ct
{
    namespace ext
    {
    template <class DTYPE, template <class...> class STORAGE_POLICY, class T>
    struct DataTableBase;

    template <class U, template <class...> class STORAGE_POLICY, class... Args>
    struct DataTableBase<U, STORAGE_POLICY, VariadicTypedef<Args...>> : STORAGE_POLICY<Args...>, IDataTable<U>
    {
        using Super = STORAGE_POLICY<Args...>;
        DataTableBase() { fillOffsets(Reflect<U>::end()); }



      protected:
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

        DataTableArrayIterator<void> ptr(const size_t offset, const size_t index, const ct::Indexer<0>)
        {
            if (offset == m_field_offsets[0])
            {
                return (Super::template get<0>().data(index));
            }
            return {nullptr, 1};
        }

        template <index_t I>
        DataTableArrayIterator<void> ptr(const size_t offset, const size_t index, const ct::Indexer<I> field_index)
        {
            if (offset == m_field_offsets[I])
            {
                return (Super::template get<I>().data(index));
            }
            return ptr(offset, index, --field_index);
        }

        DataTableArrayIterator<void> ptr(const size_t offset, const size_t index) override
        {
            return ptr(offset, index, ct::Reflect<U>::end());
        }

        DataTableArrayIterator<const void> ptr(const size_t offset, const size_t index, ct::Indexer<0>) const
        {
            if (offset == m_field_offsets[0])
            {
                return (Super::template get<0>().data(index));
            }
            return {nullptr, 1};
        }

        template <index_t I>
        DataTableArrayIterator<const void> ptr(const size_t offset, const size_t index, ct::Indexer<I> field_index) const
        {
            if (offset == m_field_offsets[I])
            {
                return (Super::template get<I>().data(index));
            }
            return ptr(offset, index, --field_index);
        }

        DataTableArrayIterator<const void> ptr(const size_t offset, const size_t index) const override
        {
            return ptr(offset, index, ct::Reflect<U>::end());
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
    }
}
#endif // CT_EXT_DATA_TABLE_BASE_HPP
