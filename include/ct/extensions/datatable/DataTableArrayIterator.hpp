#ifndef CT_EXT_DATA_TABLE_ARRAY_ITERATOR_HPP
#define CT_EXT_DATA_TABLE_ARRAY_ITERATOR_HPP
#include <ct/types/TArrayView.hpp>

namespace ct
{
    namespace ext
    {
        // Dynamic Array of fixed size per element in the data table
        // IE each element in the data table has an array that is of the same size
        // as all other elements in that table, size is determined dynamically.

        template <class T, class D>
        struct DataTableArrayIteratorConstBase
        {
            TArrayView<const T> operator*() const
            {
                return {static_cast<const D*>(this)->begin, static_cast<const D*>(this)->stride};
            }

            TArrayView<const T> operator->() const
            {
                return {static_cast<const D*>(this)->begin, static_cast<const D*>(this)->stride};
            }

            D operator++(int32_t val) const
            {
                return {static_cast<const D*>(this)->begin + static_cast<const D*>(this)->stride * val,
                        static_cast<const D*>(this)->stride};
            }

            D operator--(int32_t val) const
            {
                return {static_cast<const D*>(this)->begin - static_cast<const D*>(this)->stride * val,
                        static_cast<const D*>(this)->stride};
            }
        };

        template <class T, class D>
        struct DataTableArrayIteratorBase : DataTableArrayIteratorConstBase<T, D>
        {
            TArrayView<T> operator*() { return {static_cast<D*>(this)->begin, static_cast<D*>(this)->stride}; }

            TArrayView<T> operator->() { return {static_cast<D*>(this)->begin, static_cast<D*>(this)->stride}; }

            D& operator++()
            {
                static_cast<D*>(this)->begin += static_cast<const D*>(this)->stride;
                return static_cast<D&>(*this);
            }

            D& operator--()
            {
                static_cast<D*>(this)->begin -= static_cast<const D*>(this)->stride;
                return static_cast<D&>(*this);
            }
        };

        template <class T>
        struct DataTableArrayIterator;

        template <>
        struct DataTableArrayIterator<void> : DataTableArrayIteratorBase<void, DataTableArrayIterator<void>>
        {
            DataTableArrayIterator(void* ptr = nullptr, size_t step = 1) : begin(ptr), stride(step) {}

            template <class T>
            DataTableArrayIterator(DataTableArrayIterator<T> other)
                : begin{other.begin}, stride{other.stride * sizeof(T)}
            {
            }

            template <class T>
            T& as()
            {
                return static_cast<T*>(begin)[0];
            }

            template <class T>
            T* ptr()
            {
                return static_cast<T*>(begin);
            }

            void* begin;
            size_t stride;
        };

        template <>
        struct DataTableArrayIterator<const void>
            : DataTableArrayIteratorConstBase<void, DataTableArrayIterator<const void>>
        {
            DataTableArrayIterator(const void* ptr = nullptr, size_t step = 1) : begin(ptr), stride(step) {}

            template <class T>
            DataTableArrayIterator(DataTableArrayIterator<T> other)
                : begin{other.begin}, stride{other.stride * sizeof(T)}
            {
            }

            template <class T>
            const T& as() const
            {
                return *static_cast<const T*>(begin)[0];
            }

            template <class T>
            const T* ptr() const
            {
                return static_cast<const T*>(begin);
            }

            const void* begin;
            size_t stride;
        };

        template <class T>
        struct DataTableArrayIterator : DataTableArrayIteratorBase<T, DataTableArrayIterator<T>>
        {
            DataTableArrayIterator(T* ptr = nullptr, size_t step = 1) : begin(ptr), stride(step) {}

            DataTableArrayIterator(DataTableArrayIterator<void> other)
                : begin{static_cast<T*>(other.begin)}, stride{other.stride / sizeof(T)}
            {
            }

            TArrayView<T> array(size_t idx) const { return {&begin[idx * stride], stride}; }

            TArrayView<T> operator[](size_t idx) const { return array(idx); }

            T* begin;
            size_t stride;
        };

        template <class T>
        struct DataTableArrayIterator<const T> : DataTableArrayIteratorConstBase<T, DataTableArrayIterator<const T>>
        {
            DataTableArrayIterator(const T* ptr = nullptr, size_t step = 1) : begin(ptr), stride(step) {}

            DataTableArrayIterator(DataTableArrayIterator<const void> itr)
                : begin(static_cast<const T*>(itr.begin)), stride(itr.stride / sizeof(T))
            {
            }

            TArrayView<const T> array(size_t idx) const { return {&begin[idx * stride], stride}; }

            TArrayView<const T> operator[](size_t idx) const { return array(idx); }

            const T* begin;
            size_t stride;
        };
    } // namespace ext
} // namespace ct
#endif // CT_EXT_DATA_TABLE_ARRAY_ITERATOR_HPP
