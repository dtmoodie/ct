#ifndef CT_EXT_DATA_TABLE_STORAGE_HPP
#define CT_EXT_DATA_TABLE_STORAGE_HPP
#include "DataTableArrayIterator.hpp"

#include <ct/types/TArrayView.hpp>
#include <ct/types.hpp>

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

            DataTableArrayIterator<T> data(size_t idx) { return {&m_data[idx], 1}; }

            DataTableArrayIterator<const T> data(size_t idx) const { return {&m_data[idx], 1}; }

            void reserve(size_t size) { m_data.reserve(size); }

            void push_back(const T& val) { m_data.push_back(val); }

            size_t size() const { return m_data.size(); }
            void resizeSubarray(size_t) {}
          private:
            std::vector<T> m_data;
        };

        template <class T>
        struct DataTableStorage<TArrayView<T>>
        {
            TArrayView<T> operator[](size_t idx)
            {
                return {&m_data[idx * m_stride], m_stride};
            }

            TArrayView<const T> operator[](size_t idx) const
            {
                return {&m_data[idx * m_stride], m_stride};
            }

            DataTableArrayIterator<T> data(size_t idx)
            {
                return {&m_data[idx * m_stride], m_stride};
            }

            DataTableArrayIterator<const T> data(size_t idx) const
            {
                return {&m_data[idx * m_stride], m_stride};
            }

            void reserve(size_t size) { m_data.reserve(size * m_stride); }
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

          private:
            std::vector<T> m_data;
            size_t m_stride = 0;
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

            template <index_t I>
            auto get() const -> decltype(std::get<I>(m_data))
            {
                return std::get<I>(m_data);
            }
            DefaultStoragePolicy()
            {
                static_assert(std::is_lvalue_reference<decltype(this->template get<0>())>::value, "Expect to be returning a reference");
            }
        };
    }
}

#endif // CT_EXT_DATA_TABLE_STORAGE_HPP
