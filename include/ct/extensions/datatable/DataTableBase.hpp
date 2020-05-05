#ifndef CT_EXT_DATA_TABLE_BASE_HPP
#define CT_EXT_DATA_TABLE_BASE_HPP
#include "IDataTable.hpp"
#include <array>
#include <ct/reflect.hpp>
#include <ct/type_traits.hpp>

namespace ct
{
    namespace ext
    {
        template <class DTYPE, template <class...> class STORAGE_POLICY, class T>
        struct DataTableBase;
        struct Component
        {
        };

        template <class T>
        struct SelectComponents;

        template <class T>
        struct SelectComponents<VariadicTypedef<T>>
        {
            using type = typename AppendIf<IsBase<Base<Component>, Derived<T>>::value, T, VariadicTypedef<>>::type;
        };

        template <class T, class... U>
        struct SelectComponents<VariadicTypedef<T, U...>>
        {
            using super = SelectComponents<VariadicTypedef<U...>>;
            using type = typename AppendIf<IsBase<Base<Component>, Derived<T>>::value, T, typename super::type>::type;
        };

        template <class U, template <class...> class STORAGE_POLICY, class... Args>
        struct DataTableBase<U, STORAGE_POLICY, VariadicTypedef<Args...>>
            : STORAGE_POLICY<Args...>, IDataTableImpl<U, DataTableBase<U, STORAGE_POLICY, VariadicTypedef<Args...>>>

        {
            using Storage = STORAGE_POLICY<Args...>;
            DataTableBase() { fillOffsets(Reflect<U>::end()); }

            template <class V>
            void populateDataRecurse(V& data, const size_t row, const ct::Indexer<0> idx)
            {
                const auto accessor = Reflect<V>::getPtr(idx);
                accessor.set(data, Storage::template get<0>()[row]);
            }

            template <class V, index_t I>
            void populateDataRecurse(V& data, const size_t row, const ct::Indexer<I> idx)
            {
                const auto accessor = Reflect<V>::getPtr(idx);
                accessor.set(data, Storage::template get<I>()[row]);
                populateDataRecurse(data, row, --idx);
            }

          protected:
            void reserveImpl(const size_t size, const ct::Indexer<0>) { Storage::template get<0>().reserve(size); }

            template <index_t I>
            void reserveImpl(const size_t size, const ct::Indexer<I> idx)
            {
                Storage::template get<I>().reserve(size);
                reserveImpl(size, --idx);
            }

            void push(const U& data, const ct::Indexer<0> idx)
            {
                const auto accessor = Reflect<U>::getPtr(idx);
                Storage::template get<0>().push_back(accessor.get(data));
            }

            template <index_t I>
            void push(const U& data, const ct::Indexer<I> idx)
            {
                const auto accessor = Reflect<U>::getPtr(idx);
                Storage::template get<I>().push_back(accessor.get(data));
                push(data, --idx);
            }

            DataTableArrayIterator<void> ptr(const size_t offset, const size_t index, const ct::Indexer<0>)
            {
                if (offset == m_field_offsets[0])
                {
                    return (Storage::template get<0>().data(index));
                }
                return {nullptr, 1};
            }

            template <index_t I>
            DataTableArrayIterator<void> ptr(const size_t offset, const size_t index, const ct::Indexer<I> field_index)
            {
                if (offset == m_field_offsets[I])
                {
                    return (Storage::template get<I>().data(index));
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
                    return (Storage::template get<0>().data(index));
                }
                return {nullptr, 1};
            }

            template <index_t I>
            DataTableArrayIterator<const void>
            ptr(const size_t offset, const size_t index, ct::Indexer<I> field_index) const
            {
                if (offset == m_field_offsets[I])
                {
                    return (Storage::template get<I>().data(index));
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
                if (std::is_same<typename std::decay<decltype(Storage::template get<0>())>::type, T>::value &&
                    offset == m_field_offsets[0])
                {
                    return &Storage::template get<0>();
                }
                return nullptr;
            }

            template <class T, index_t I>
            const void* storageImpl(const size_t offset, ct::Indexer<I> field_index) const
            {
                if (std::is_same<typename std::decay<decltype(Storage::template get<I>())>::type, T>::value &&
                    offset == m_field_offsets[I])
                {
                    return &Storage::template get<I>();
                }
                return storageImpl<T>(offset, --field_index);
            }

            template <class T>
            void* storageImpl(const size_t offset, ct::Indexer<0>)
            {
                if (std::is_same<typename std::decay<decltype(Storage::template get<0>())>::type, T>::value &&
                    offset == m_field_offsets[0])
                {
                    return &Storage::template get<0>();
                }
                return nullptr;
            }

            template <class T, index_t I>
            void* storageImpl(const size_t offset, ct::Indexer<I> field_index)
            {
                if (std::is_same<typename std::decay<decltype(Storage::template get<I>())>::type, T>::value &&
                    offset == m_field_offsets[I])
                {
                    return &Storage::template get<I>();
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
                    Storage::template get<0>().resizeSubarray(size);
                    return;
                }
            }

            template <index_t I>
            void resizeSubarrayImpl(const size_t offset, const size_t size, ct::Indexer<I> idx)
            {
                if (offset == m_field_offsets[I])
                {
                    Storage::template get<I>().resizeSubarray(size);
                    return;
                }
                resizeSubarrayImpl(offset, size, --idx);
            }

            std::array<size_t, sizeof...(Args)> m_field_offsets;
        };
    } // namespace ext
} // namespace ct
#endif // CT_EXT_DATA_TABLE_BASE_HPP
