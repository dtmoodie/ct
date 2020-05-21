#ifndef CT_EXT_DATA_TABLE_STORAGE_HPP
#define CT_EXT_DATA_TABLE_STORAGE_HPP
#include "DataTableArrayIterator.hpp"

#include <ct/types.hpp>
#include <ct/types/TArrayView.hpp>

#include <tuple>
#include <vector>

namespace ct
{
    namespace ext
    {
        template <class T>
        struct DataTableStorage
        {
            T& operator[](size_t idx) { return m_data[idx]; }

            const T& operator[](size_t idx) const { return m_data[idx]; }

            DataTableArrayIterator<T> data(size_t idx = 0) { return {&m_data[idx], 1}; }

            DataTableArrayIterator<const T> data(size_t idx = 0) const { return {&m_data[idx], 1}; }

            void reserve(size_t size) { m_data.reserve(size); }
            void resize(size_t size) { m_data.resize(size); }

            void push_back(const T& val) { m_data.push_back(val); }
            void push_back(T&& val) { m_data.push_back(std::move(val)); }

            size_t size() const { return m_data.size(); }
            void resizeSubarray(size_t) {}

            void erase(uint32_t index) { m_data.erase(m_data.begin() + index); }
            void clear() { m_data.clear(); }

          private:
            std::vector<T> m_data;
        };

        template <class T>
        struct DataTableStorage<TArrayView<T>>
        {
            TArrayView<T> operator[](size_t idx) { return {&m_data[idx * m_stride], m_stride}; }

            TArrayView<const T> operator[](size_t idx) const { return {&m_data[idx * m_stride], m_stride}; }

            DataTableArrayIterator<T> data(size_t idx = 0) { return {&m_data[idx * m_stride], m_stride}; }

            DataTableArrayIterator<const T> data(size_t idx = 0) const { return {&m_data[idx * m_stride], m_stride}; }

            void reserve(size_t size) { m_data.reserve(size * m_stride); }
            void resize(size_t size) { m_data.resize(size * m_stride); }
            void push_back(const TArrayView<T>& val)
            {
                if (m_stride == 0 && m_data.size() == 0)
                {
                    resizeSubarray(val.size());
                }
                assert(val.size() == m_stride);
                const auto idx = m_data.size();
                m_data.resize(m_data.size() + m_stride);
                memcpy(&m_data[idx], val.data(), m_stride * sizeof(T));
            }
            size_t size() const { return m_data.size(); }
            size_t stride() const { return m_stride; }
            void resizeSubarray(size_t size)
            {
                if (m_stride != size)
                {
                    if (m_stride != 0)
                    {
                        const size_t elems = m_data.size() / m_stride;
                        std::vector<T> data(elems * size);
                        for (size_t i = 0; i < elems; ++i)
                        {
                            memcpy(&data[i * size], &m_data[i * m_stride], m_stride * sizeof(T));
                        }
                        m_data = std::move(data);
                    }

                    m_stride = size;
                }
            }

            void erase(uint32_t index)
            {
                auto begin = m_data.begin() + index * m_stride;
                auto end = m_data.begin() + (index + 1) * m_stride;
                m_data.erase(begin, end);
            }

            void clear() { m_data.clear(); }

          private:
            std::vector<T> m_data;
            size_t m_stride = 0;
        };

        template <class... Ts>
        struct DefaultStoragePolicy
        {
            using type = std::tuple<DataTableStorage<Ts>...>;
            type m_data;

            static type init() { return {}; }

            template <index_t I>
            auto get() -> decltype(std::get<I>(m_data))
            {
                return std::get<I>(m_data);
            }

            template <index_t I>
            auto get() const -> decltype(std::get<I>(m_data))
            {
                return std::get<I>(m_data);
            }

            DefaultStoragePolicy()
            {
                static_assert(std::is_lvalue_reference<decltype(this->template get<0>())>::value,
                              "Expect to be returning a reference");
            }
        };

        template <class... Ts>
        struct SharedPtrStoragePolicy
        {
            using type = std::tuple<std::shared_ptr<DataTableStorage<Ts>>...>;
            type m_data;

            template <index_t I>
            auto get() -> decltype(*std::get<I>(m_data))
            {
                assert(std::get<I>(m_data));
                return *std::get<I>(m_data);
            }

            template <index_t I>
            auto get() const -> decltype(*std::get<I>(m_data))
            {
                assert(std::get<I>(m_data));
                return *std::get<I>(m_data);
            }

            SharedPtrStoragePolicy()
            {
                init(m_data);
                static_assert(std::is_lvalue_reference<decltype(this->template get<0>())>::value,
                              "Expect to be returning a reference");
            }

          private:
            static void init(type& data, Indexer<0>)
            {
                using Ptr_t = typename std::remove_reference<decltype(std::get<0>(data))>::type;
                using Storage_t = typename Ptr_t::element_type;
                std::get<0>(data) = std::make_shared<Storage_t>();
            }
            template <index_t I>
            static void init(type& data, Indexer<I> idx)
            {
                using Ptr_t = typename std::remove_reference<decltype(std::get<I>(data))>::type;
                using Storage_t = typename Ptr_t::element_type;
                std::get<I>(data) = std::make_shared<Storage_t>();
                auto next_idx = --idx;
                init(data, next_idx);
            }

            static void init(type& data) { init(data, Indexer<sizeof...(Ts) - 1>{}); }
        };
    } // namespace ext
} // namespace ct

#endif // CT_EXT_DATA_TABLE_STORAGE_HPP
