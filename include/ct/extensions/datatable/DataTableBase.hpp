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
            using type = typename AppendIf<IsBase<Base<Component>, Derived<T>>::value, T, ct::VariadicTypedef<>>::type;
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
                const auto next = --idx;
                populateDataRecurse(data, row, next);
            }

          protected:
            void reserveImpl(const size_t size, const ct::Indexer<0>) { Storage::template get<0>().reserve(size); }

            template <index_t I>
            void reserveImpl(const size_t size, const ct::Indexer<I> idx)
            {
                auto& ref = Storage::template get<I>();
                ref.reserve(size);
                const auto next = --idx;
                reserveImpl(size, next);
            }

            void push(const U& data, const ct::Indexer<0> idx)
            {
                const auto accessor = Reflect<U>::getPtr(idx);
                auto& ref = Storage::template get<0>();
                ref.push_back(accessor.get(data));
            }

            template <index_t I>
            void push(const U& data, const ct::Indexer<I> idx)
            {
                const auto accessor = Reflect<U>::getPtr(idx);
                auto& ref = Storage::template get<I>();
                ref.push_back(accessor.get(data));
                const auto next = --idx;
                push(data, next);
            }

            mt::Tensor<void, 2> ptr(const size_t offset, const size_t index, const ct::Indexer<0>)
            {
                if (offset == m_field_offsets[0])
                {
                    auto& ref = Storage::template get<0>();
                    return ref.data(index);
                }
                return {nullptr, 1};
            }

            template <index_t I>
            mt::Tensor<void, 2> ptr(const size_t offset, const size_t index, const ct::Indexer<I> field_index)
            {
                if (offset == m_field_offsets[I])
                {
                    auto& ref = Storage::template get<I>();
                    return ref.data(index);
                }
                const auto next = --field_index;
                return ptr(offset, index, next);
            }

            mt::Tensor<void, 2> ptr(const size_t offset, const size_t index) override
            {
                const auto itr = ct::Reflect<U>::end();
                return ptr(offset, index, itr);
            }

            mt::Tensor<const void, 2> ptr(const size_t offset, const size_t index, ct::Indexer<0>) const
            {
                if (offset == m_field_offsets[0])
                {
                    auto& ref = Storage::template get<0>();
                    return ref.data(index);
                }
                return {nullptr, 1};
            }

            template <index_t I>
            mt::Tensor<const void, 2> ptr(const size_t offset, const size_t index, ct::Indexer<I> field_index) const
            {
                if (offset == m_field_offsets[I])
                {
                    auto& ref = Storage::template get<I>();
                    return ref.data(index);
                }
                const auto next = --field_index;
                return ptr(offset, index, next);
            }

            mt::Tensor<const void, 2> ptr(const size_t offset, const size_t index) const override
            {
                const auto itr = ct::Reflect<U>::end();
                return ptr(offset, index, itr);
            }

            template <class T>
            const void* storageImpl(const size_t offset, ct::Indexer<0>) const
            {
                using storage_type = typename std::decay<decltype(Storage::template get<0>())>::type;
                if (std::is_same<storage_type, T>::value && offset == m_field_offsets[0])
                {
                    return &Storage::template get<0>();
                }
                return nullptr;
            }

            template <class T, index_t I>
            const void* storageImpl(const size_t offset, ct::Indexer<I> field_index) const
            {
                using storage_type = typename std::decay<decltype(Storage::template get<I>())>::type;
                if (std::is_same<storage_type, T>::value && offset == m_field_offsets[I])
                {
                    return &Storage::template get<I>();
                }
                const auto next = --field_index;
                return storageImpl<T>(offset, next);
            }

            template <class T>
            void* storageImpl(const size_t offset, ct::Indexer<0>)
            {
                using storage_type = typename std::decay<decltype(Storage::template get<0>())>::type;
                if (std::is_same<storage_type, T>::value && offset == m_field_offsets[0])
                {
                    return &Storage::template get<0>();
                }
                return nullptr;
            }

            template <class T, index_t I>
            void* storageImpl(const size_t offset, ct::Indexer<I> field_index)
            {
                using storage_type = typename std::decay<decltype(Storage::template get<I>())>::type;
                if (std::is_same<storage_type, T>::value && offset == m_field_offsets[I])
                {
                    return &Storage::template get<I>();
                }
                const auto next = --field_index;
                return storageImpl<T>(offset, next);
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

            template <class SHAPE>
            void resizeSubarrayImpl(const size_t offset, const SHAPE shape, ct::Indexer<0>)
            {
                if (offset == m_field_offsets[0])
                {
                    Storage::template get<0>().resizeSubarray(shape);
                    return;
                }
            }

            template <index_t I, class SHAPE>
            void resizeSubarrayImpl(const size_t offset, const SHAPE shape, ct::Indexer<I> idx)
            {
                if (offset == m_field_offsets[I])
                {
                    Storage::template get<I>().resizeSubarray(shape);
                    return;
                }
                const auto next = --idx;
                resizeSubarrayImpl(offset, shape, next);
            }

            std::array<size_t, sizeof...(Args)> m_field_offsets;
        };
    } // namespace ext
} // namespace ct
#endif // CT_EXT_DATA_TABLE_BASE_HPP
