#ifndef CT_ARRAY_VIEW_HPP
#define CT_ARRAY_VIEW_HPP
#include <ct/type_traits.hpp>

#include <cstdint>
#include <iostream>

namespace ct
{
    template <class T>
    struct TArrayView
    {
        TArrayView(T* ptr = nullptr, size_t sz = 0);
        TArrayView(T* begin, T* end);

        const T& operator[](size_t idx) const;
        T& operator[](size_t idx);

        bool operator==(const TArrayView& other) const;

        bool operator!=(const TArrayView& other) const;

        size_t size() const;

        T* data();
        const T* data() const;

        T* begin();
        const T* begin() const;
        T* end();
        const T* end() const;

        TArrayView slice(ssize_t begin, ssize_t end = 0);
        TArrayView subView(ssize_t begin, size_t count = 0);
        TArrayView<const T> slice(ssize_t begin, ssize_t end = 0) const;
        TArrayView<const T> subView(ssize_t begin, size_t count = 0) const;

        size_t revIndex(ssize_t) const;

      private:
        T* m_data;
        size_t m_size;
    };

    template <class T>
    struct TArrayView<const T>
    {
        TArrayView(const T* ptr = nullptr, size_t sz = 0);
        TArrayView(const T* begin, const T* end);

        const T& operator[](size_t idx) const;

        bool operator==(const TArrayView& other) const;

        bool operator!=(const TArrayView& other) const;

        size_t size() const;

        const T* data() const;

        const T* begin() const;
        const T* end() const;

        TArrayView<const T> slice(ssize_t begin, ssize_t end = 0) const;
        TArrayView<const T> subView(ssize_t begin, size_t count = 0) const;

        size_t revIndex(ssize_t) const;

      private:
        const T* m_data;
        size_t m_size;
    };

    template <class T>
    TArrayView<T>::TArrayView(T* ptr, size_t sz) : m_data(ptr), m_size(sz)
    {
    }

    template <class T>
    TArrayView<T>::TArrayView(T* begin, T* end) : m_data(begin), m_size(end - begin)
    {
    }

    template <class T>
    const T& TArrayView<T>::operator[](size_t idx) const
    {
        return m_data[idx];
    }

    template <class T>
    T& TArrayView<T>::operator[](size_t idx)
    {
        return m_data[idx];
    }

    template <class T>
    bool TArrayView<T>::operator==(const TArrayView<T>& other) const
    {
        if (other.m_data == m_data && other.m_size == m_size)
        {
            return true;
        }
        if (other.m_size != m_size)
        {
            return false;
        }

        for (size_t i = 0; i < m_size; ++i)
        {
            if (m_data[i] != other[i])
            {
                return false;
            }
        }
        return true;
    }

    template <class T>
    TArrayView<T> TArrayView<T>::slice(ssize_t begin, ssize_t end)
    {
        T* ptr_begin = m_data + revIndex(begin);
        T* ptr_end = m_data + (end == 0 ? m_size : revIndex(end));
        return TArrayView<T>(ptr_begin, ptr_end);
    }

    template <class T>
    TArrayView<T> TArrayView<T>::subView(ssize_t begin, size_t count)
    {
        if (count == 0)
        {
            return slice(begin);
        }
        return slice(begin, begin + count);
    }

    template <class T>
    TArrayView<const T> TArrayView<T>::slice(ssize_t begin, ssize_t end) const
    {
        const T* ptr_begin = m_data + revIndex(begin);
        const T* ptr_end = m_data + (end == 0 ? m_size : revIndex(end));
        return TArrayView<const T>(ptr_begin, ptr_end);
    }

    template <class T>
    TArrayView<const T> TArrayView<T>::subView(ssize_t begin, size_t count) const
    {
        if (count == 0)
        {
            return slice(begin);
        }
        return slice(begin, begin + count);
    }

    template <class T>
    size_t TArrayView<T>::revIndex(ssize_t idx) const
    {
        if (idx < 0)
        {
            return m_size - idx;
        }
        return idx;
    }

    template <class T>
    bool TArrayView<T>::operator!=(const TArrayView<T>& other) const
    {
        return !(*this == other);
    }

    template <class T>
    size_t TArrayView<T>::size() const
    {
        return m_size;
    }

    template <class T>
    T* TArrayView<T>::data()
    {
        return m_data;
    }

    template <class T>
    const T* TArrayView<T>::data() const
    {
        return m_data;
    }

    template <class T>
    T* TArrayView<T>::begin()
    {
        return m_data;
    }

    template <class T>
    const T* TArrayView<T>::begin() const
    {
        return m_data;
    }

    template <class T>
    T* TArrayView<T>::end()
    {
        return m_data + m_size;
    }

    template <class T>
    const T* TArrayView<T>::end() const
    {
        return m_data + m_size;
    }

    ///////////////////////////////////////////////////////////////////////

    template <class T>
    TArrayView<const T>::TArrayView(const T* ptr, size_t sz) : m_data(ptr), m_size(sz)
    {
    }

    template <class T>
    TArrayView<const T>::TArrayView(const T* begin, const T* end) : m_data(begin), m_size(end - begin)
    {
    }

    template <class T>
    const T& TArrayView<const T>::operator[](size_t idx) const
    {
        return m_data[idx];
    }

    template <class T>
    bool TArrayView<const T>::operator==(const TArrayView<const T>& other) const
    {
        if (other.m_data == m_data && other.m_size == m_size)
        {
            return true;
        }
        if (other.m_size != m_size)
        {
            return false;
        }

        for (size_t i = 0; i < m_size; ++i)
        {
            if (m_data[i] != other[i])
            {
                return false;
            }
        }
        return true;
    }

    template <class T>
    TArrayView<const T> TArrayView<const T>::slice(ssize_t begin, ssize_t end) const
    {
        const T* ptr_begin = m_data + revIndex(begin);
        const T* ptr_end = m_data + (end == 0 ? m_size : revIndex(end));
        return TArrayView<const T>(ptr_begin, ptr_end);
    }

    template <class T>
    TArrayView<const T> TArrayView<const T>::subView(ssize_t begin, size_t count) const
    {
        if (count == 0)
        {
            return slice(begin);
        }
        return slice(begin, begin + count);
    }

    template <class T>
    size_t TArrayView<const T>::revIndex(ssize_t idx) const
    {
        if (idx < 0)
        {
            return m_size - idx;
        }
        return idx;
    }

    template <class T>
    bool TArrayView<const T>::operator!=(const TArrayView<const T>& other) const
    {
        return !(*this == other);
    }

    template <class T>
    size_t TArrayView<const T>::size() const
    {
        return m_size;
    }

    template <class T>
    const T* TArrayView<const T>::data() const
    {
        return m_data;
    }

    template <class T>
    const T* TArrayView<const T>::begin() const
    {
        return m_data;
    }

    template <class T>
    const T* TArrayView<const T>::end() const
    {
        return m_data + m_size;
    }

    template <class T>
    TArrayView<T> makeArrayView(TArrayView<T> view, size_t)
    {
        return view;
    }

    template <class T>
    TArrayView<T> makeArrayView(T* ptr, size_t size)
    {
        return TArrayView<T>{ptr, size};
    }

    template <class T>
    std::ostream& operator<<(std::ostream& os, const TArrayView<T>& view)
    {
        os << "[";
        for (size_t i = 0; i < view.size(); ++i)
        {
            if (i != 0)
            {
                os << " ";
            }
            os << view[i];
        }

        os << "]";
        return os;
    }

    template <class T>
    struct ReferenceType<TArrayView<T>>
    {
        using Type = TArrayView<T>;
        using ConstType = TArrayView<const T>;
    };
}

#endif // CT_ARRAY_VIEW_HPP
