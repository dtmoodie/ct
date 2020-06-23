#ifndef CT_EXT_DATA_TABLE_STORAGE_HPP
#define CT_EXT_DATA_TABLE_STORAGE_HPP
#include "DataTableArrayIterator.hpp"

#include <ct/types.hpp>
#include <ct/types/TArrayView.hpp>

#include <minitensor/Tensor.hpp>

#include <tuple>
#include <vector>

namespace mt
{
    template <class T, ssize_t N>
    Tensor<const T, 1> tensorWrap(const ct::TArrayView<T, N>& data)
    {
        return Tensor<const T, 1>(data.data(), data.size());
    }

    template <class T, ssize_t N>
    Tensor<T, 1> tensorWrap(ct::TArrayView<T, N>& data)
    {
        return Tensor<T, 1>(data.data(), data.size());
    }
} // namespace mt

namespace ct
{
    namespace ext
    {
        template <class T>
        struct DataDimensionality
        {
            static constexpr const uint8_t value = 0;
            using DType = T;
        };

        template <class T>
        struct DataDimensionality<TArrayView<T>>
        {
            static constexpr const uint8_t value = DataDimensionality<T>::value + 1;
            using DType = T;
        };

        template <class T_>
        struct DataTableStorage
        {
            static constexpr const uint8_t data_dim = DataDimensionality<T_>::value;
            static constexpr const uint8_t storage_dim = data_dim + 1;

            using T = typename DataDimensionality<T_>::DType;

            DataTableStorage() {}

            auto operator[](size_t idx) -> decltype(std::declval<mt::Tensor<T, storage_dim>>()[idx])
            {
                mt::Tensor<T, storage_dim> view(m_data.data(), m_shape);
                return view[idx];
            }

            auto operator[](size_t idx) const -> decltype(std::declval<mt::Tensor<const T, storage_dim>>()[idx])
            {
                mt::Tensor<const T, storage_dim> view(m_data.data(), m_shape);
                return view[idx];
            }

            mt::Tensor<T, storage_dim> data(size_t idx = 0)
            {
                T* ptr = m_data.data();
                mt::Shape<storage_dim> out_shape = m_shape;
                ptr += out_shape.getStride(0) * idx;
                out_shape.setShape(0, out_shape[0] - idx);
                return mt::Tensor<T, storage_dim>(ptr, out_shape);
            }

            mt::Tensor<const T, storage_dim> data(size_t idx = 0) const
            {
                const T* ptr = m_data.data();
                mt::Shape<storage_dim> out_shape = m_shape;
                ptr += out_shape.getStride(0) * idx;
                out_shape.setShape(0, out_shape[0] - idx);
                return mt::Tensor<const T, storage_dim>(ptr, out_shape);
            }

            void reserve(size_t size)
            {
                auto tmp = m_shape;
                tmp.setShape(0, size);
                const auto num_elems = tmp.numElements();
                m_data.reserve(num_elems);
            }

            mt::Shape<storage_dim> shape() const { return m_shape; }

            void reshape(mt::Shape<storage_dim> shape)
            {
                m_shape = std::move(shape);
                m_data.resize(m_shape.numElements());
            }

            size_t size() const { return m_shape[0]; }

            void resize(size_t size)
            {
                m_shape.setShape(0, size);
                m_data.resize(m_shape.numElements());
            }

            void push_back(const T_& val)
            {
                auto input_view = mt::tensorWrap(val);
                resizeSubarray(input_view.getShape());
                const size_t current_elems = m_data.size();
                const size_t new_elems = input_view.getShape().numElements();
                const size_t new_size = current_elems + new_elems;
                m_data.resize(new_size);
                uint32_t new_index = m_shape[0];
                m_shape.setShape(0, new_index + 1);
                mt::Tensor<T, storage_dim> storage_view = this->data();
                input_view.copyTo(storage_view[new_index]);
                // storage_view[new_size - 1].assign(input_view);
                // m_data.push_back(val);
            }

            void push_back(T_&& val)
            {
                auto input_view = mt::tensorWrap(val);
                size_t new_size = m_data.size() + input_view.getShape().numElements();
                m_data.resize(new_size);
                uint32_t new_index = m_shape[0];
                m_shape.setShape(0, new_index + 1);
                mt::Tensor<T, storage_dim> storage_view = this->data();
                input_view.copyTo(storage_view[new_index]);
            }

            void resizeSubarray(mt::Shape<data_dim> subshape)
            {
                // TODO move stuff?
                for (uint8_t i = 1; i < storage_dim; ++i)
                {
                    m_shape.setShape(i, subshape[i - 1]);
                }
                m_shape.calculateStride();
            }

            template <uint8_t I>
            void resizeSubarray(mt::Shape<I>)
            {
            }

            void erase(uint32_t index)
            {
                if (data_dim > 1)
                {
                    const auto subdim_size = mt::stripOuterDim(m_shape).numElements();
                    m_data.erase(m_data.begin() + index * subdim_size, m_data.begin() + (index + 1) * subdim_size);
                }
                else
                {
                    m_data.erase(m_data.begin() + index);
                }
            }

            void clear()
            {
                m_shape.setShape(0, 0);
                m_data.clear();
            }

            void insert(uint32_t idx, const T_& val) { m_data.insert(m_data.begin() + idx, val); }
            void assign(uint32_t idx, const T_& val) { m_data.at(idx) = val; }

          private:
            std::vector<T> m_data;
            mt::Shape<storage_dim> m_shape;
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

    template <class T>
    struct ReflectImpl<ext::DataTableStorage<T>, void>
    {
        using DataType = ext::DataTableStorage<T>;
        using StorageType = typename DataType::T;
        using this_t = ReflectImpl<DataType, void>;

        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }
        static std::array<size_t, 2> shape(const DataType& data) { return {data.size(), data.stride()}; }

        static void reshape(DataType& data, const std::array<size_t, 2> shape)
        {
            data.resizeSubarray(shape[1]);
            data.resize(shape[0]);
        }

        static size_t size(const DataType& data) { return data.shape().numElements(); }

        static TArrayView<const StorageType> getData(const DataType& data)
        {
            auto tensor = data.data();
            return TArrayView<const StorageType>(tensor.data(), data.size());
        }

        static TArrayView<StorageType> getDataMutable(DataType& data)
        {
            auto tensor = data.data();
            return TArrayView<StorageType>(tensor.data(), data.size());
        }

        REFLECT_STUB
            PROPERTY(shape, &DataType::shape, &DataType::reshape)
            PROPERTY(size, &this_t::size)
            PROPERTY(data, &this_t::getData, &this_t::getDataMutable)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return Indexer<NUM_FIELDS - 1>(); }
    };

    /*template <class T>
    struct ReflectImpl<ext::DataTableStorage<TArrayView<T>>, void>
    {
        using DataType = ext::DataTableStorage<TArrayView<T>>;
        using this_t = ReflectImpl<DataType, void>;

        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }
        static std::array<size_t, 2> shape(const DataType& data) { return {data.size(), data.stride()}; }

        static void reshape(DataType& data, const std::array<size_t, 2> shape)
        {
            data.resizeSubarray(shape[1]);
            data.resize(shape[0]);
        }

        static size_t size(const DataType& data) { return data.shape().numElements(); }

        static TArrayView<const T> getData(const DataType& data)
        {
            auto dptr = data.data();
            const auto size = this_t::size(data);
            auto ptr = dptr.data();
            return TArrayView<const T>(ptr, size);
        }

        static TArrayView<T> getDataMutable(DataType& data)
        {
            auto dptr = data.data();
            const auto size = this_t::size(data);
            auto ptr = dptr.data();
            return TArrayView<T>(ptr, size);
        }

        REFLECT_STUB
            PROPERTY(shape, &this_t::shape, &this_t::reshape)
            PROPERTY(size, &this_t::size)
            PROPERTY(data, &this_t::getData, &this_t::getDataMutable)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return Indexer<NUM_FIELDS - 1>(); }
    };*/

    template <uint8_t N>
    struct ReflectImpl<mt::Shape<N>, void>
    {
        using DataType = mt::Shape<N>;
        using this_t = ReflectImpl<DataType, void>;
        static constexpr StringView getTypeName() { return GetName<DataType>::getName(); }

        static std::array<uint32_t, N> getData(const DataType& data)
        {
            std::array<uint32_t, N> out;
            for (uint8_t i = 0; i < N; ++i)
            {
                out[i] = data[i];
            }
            return out;
        }

        static void setData(DataType& data, std::array<uint32_t, N> vals)
        {
            for (uint8_t i = 0; i < N; ++i)
            {
                data.setShape(i, vals[i]);
            }
            data.calculateStride();
        }

        static size_t getSize(const DataType&) { return N; }

        REFLECT_STUB
            PROPERTY(size, &this_t::getSize)
            PROPERTY(data, &this_t::getData, &this_t::setData)
        REFLECT_INTERNAL_END;
        static constexpr Indexer<NUM_FIELDS - 1> end() { return Indexer<NUM_FIELDS - 1>(); }
    };

} // namespace ct

#endif // CT_EXT_DATA_TABLE_STORAGE_HPP
