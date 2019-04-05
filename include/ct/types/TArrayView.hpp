#ifndef CT_ARRAY_VIEW_HPP
#define CT_ARRAY_VIEW_HPP
#include <ct/type_traits.hpp>

#include <cstdint>
#include <iostream>

namespace ct
{
    template <class T = uint8_t, class U>
    T* ptrCast(U* ptr)
    {
        return static_cast<T*>(static_cast<void*>(ptr));
    }

    template <class T = uint8_t, class U>
    const T* ptrCast(const U* ptr)
    {
        return static_cast<const T*>(static_cast<const void*>(ptr));
    }

    template <class T>
    struct TArrayView
    {
        TArrayView(const TArrayView<void>&);
        TArrayView(T* ptr = nullptr, size_t sz = 0);
        TArrayView(T* begin, T* end);

        const T& operator[](ssize_t idx) const;
        T& operator[](ssize_t idx);

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
        TArrayView(const TArrayView<T>&);
        TArrayView(const TArrayView<const void>&);
        TArrayView(const T* ptr = nullptr, size_t sz = 0);
        TArrayView(const T* begin, const T* end);

        const T& operator[](ssize_t idx) const;

        bool operator==(const TArrayView& other) const;

        bool operator!=(const TArrayView& other) const;

        TArrayView& operator=(const TArrayView<T>& other);

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

    template <>
    struct TArrayView<void>
    {
        inline TArrayView(void* ptr = nullptr, size_t sz = 0);
        inline TArrayView(void* begin, void* end);

        inline bool operator==(const TArrayView& other) const;

        inline bool operator!=(const TArrayView& other) const;

        template <class T>
        TArrayView& operator=(const TArrayView<T>& other);

        inline size_t size() const;

        inline void* data();
        inline const void* data() const;

        inline void* begin();
        inline const void* begin() const;
        inline void* end();
        inline const void* end() const;

        TArrayView slice(ssize_t begin, ssize_t end = 0);
        inline TArrayView subView(ssize_t begin, size_t count = 0);
        inline TArrayView<const void> slice(ssize_t begin, ssize_t end = 0) const;
        inline TArrayView<const void> subView(ssize_t begin, size_t count = 0) const;

        inline size_t revIndex(ssize_t) const;

      private:
        void* m_data;
        size_t m_size;
    };

    template <>
    struct TArrayView<const void>
    {
        template <class T>
        TArrayView(const TArrayView<const T>&);
        TArrayView(const TArrayView<const void>&) = default;
        inline TArrayView(const void* ptr = nullptr, size_t sz = 0);
        inline TArrayView(const void* begin, const void* end);

        inline bool operator==(const TArrayView& other) const;

        inline bool operator!=(const TArrayView& other) const;

        template <class T>
        TArrayView& operator=(const TArrayView<const T>& other);

        inline TArrayView& operator=(const TArrayView<const void>& other);

        inline size_t size() const;

        inline const void* data() const;

        inline const void* begin() const;
        inline const void* end() const;

        inline TArrayView<const void> slice(ssize_t begin, ssize_t end = 0) const;
        inline TArrayView<const void> subView(ssize_t begin, size_t count = 0) const;

        inline size_t revIndex(ssize_t) const;

      private:
        const void* m_data;
        size_t m_size;
    };

    ///////////////////////////////////////////////////////////
    /// Implementation
    ///////////////////////////////////////////////////////////

    template <class T>
    TArrayView<T>::TArrayView(const TArrayView<void>& other)
        : m_data(static_cast<T*>(other.data())), m_size(other.size() / sizeof(T))
    {
    }

    template <class T>
    TArrayView<T>::TArrayView(T* ptr, size_t sz) : m_data(ptr), m_size(sz)
    {
    }

    template <class T>
    TArrayView<T>::TArrayView(T* begin, T* end) : m_data(begin), m_size(end - begin)
    {
    }

    template <class T>
    const T& TArrayView<T>::operator[](ssize_t idx) const
    {
        return m_data[revIndex(idx)];
    }

    template <class T>
    T& TArrayView<T>::operator[](ssize_t idx)
    {
        return m_data[revIndex(idx)];
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
        return static_cast<size_t>(idx);
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
    // const specialzation

    template <class T>
    TArrayView<const T>::TArrayView(const TArrayView<const void>& other)
        : m_data(static_cast<T*>(other.data())), m_size(other.size() / sizeof(T))
    {
    }

    template <class T>
    TArrayView<const T>::TArrayView(const TArrayView<T>& other) : m_data(other.data()), m_size(other.size())
    {
    }

    template <class T>
    TArrayView<const T>::TArrayView(const T* ptr, size_t sz) : m_data(ptr), m_size(sz)
    {
    }

    template <class T>
    TArrayView<const T>::TArrayView(const T* begin, const T* end) : m_data(begin), m_size(end - begin)
    {
    }

    template <class T>
    const T& TArrayView<const T>::operator[](ssize_t idx) const
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
        return static_cast<size_t>(idx);
    }

    template <class T>
    bool TArrayView<const T>::operator!=(const TArrayView<const T>& other) const
    {
        return !(*this == other);
    }

    template <class T>
    TArrayView<const T>& TArrayView<const T>::operator=(const TArrayView<T>& other)
    {
        m_data = other.data();
        m_size = other.size();
        return *this;
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

    // void specialization

    TArrayView<void>::TArrayView(void* ptr, size_t sz) : m_data(ptr), m_size(sz) {}

    TArrayView<void>::TArrayView(void* begin, void* end)
        : m_data(begin), m_size(static_cast<size_t>(ptrCast<uint8_t>(end) - ptrCast<uint8_t>(begin)))
    {
    }

    bool TArrayView<void>::operator==(const TArrayView<void>& other) const
    {
        if (other.m_data == m_data && other.m_size == m_size)
        {
            return true;
        }
        if (other.m_size != m_size)
        {
            return false;
        }
        return true;
    }

    TArrayView<const void> TArrayView<void>::slice(ssize_t begin, ssize_t end) const
    {
        const void* ptr_begin = ptrCast<const void>(ptrCast<const uint8_t>(m_data) + revIndex(begin));
        const void* ptr_end = ptrCast<const void>(ptrCast<const uint8_t>(m_data) + (end == 0 ? m_size : revIndex(end)));
        return TArrayView<const void>(ptr_begin, ptr_end);
    }

    TArrayView<const void> TArrayView<void>::subView(ssize_t begin, size_t count) const
    {
        if (count == 0)
        {
            return slice(begin);
        }
        return slice(begin, begin + count);
    }

    size_t TArrayView<void>::revIndex(ssize_t idx) const
    {
        if (idx < 0)
        {
            return m_size - idx;
        }
        return static_cast<size_t>(idx);
    }

    bool TArrayView<void>::operator!=(const TArrayView<void>& other) const { return !(*this == other); }

    template <class T>
    TArrayView<void>& TArrayView<void>::operator=(const TArrayView<T>& other)
    {
        m_data = other.data();
        m_size = other.size() * sizeof(T);
        return *this;
    }

    size_t TArrayView<void>::size() const { return m_size; }

    const void* TArrayView<void>::data() const { return m_data; }

    const void* TArrayView<void>::begin() const { return m_data; }

    const void* TArrayView<void>::end() const { return ptrCast<void>(ptrCast<uint8_t>(m_data) + m_size); }

    // const void specialzation

    template <class T>
    TArrayView<const void>::TArrayView(const TArrayView<const T>& other)
        : m_data(other.data()), m_size(other.size() * sizeof(T))
    {
    }

    TArrayView<const void>::TArrayView(const void* ptr, size_t sz) : m_data(ptr), m_size(sz) {}

    TArrayView<const void>::TArrayView(const void* begin, const void* end)
        : m_data(begin), m_size(static_cast<size_t>(ptrCast<uint8_t>(end) - ptrCast<uint8_t>(begin)))
    {
    }

    bool TArrayView<const void>::operator==(const TArrayView<const void>& other) const
    {
        if (other.m_data == m_data && other.m_size == m_size)
        {
            return true;
        }
        if (other.m_size != m_size)
        {
            return false;
        }
        return true;
    }

    TArrayView<const void> TArrayView<const void>::slice(ssize_t begin, ssize_t end) const
    {
        const void* ptr_begin = ptrCast<const void>(ptrCast<const uint8_t>(m_data) + revIndex(begin));
        const void* ptr_end = ptrCast<const void>(ptrCast<const uint8_t>(m_data) + (end == 0 ? m_size : revIndex(end)));
        return TArrayView<const void>(ptr_begin, ptr_end);
    }

    TArrayView<const void> TArrayView<const void>::subView(ssize_t begin, size_t count) const
    {
        if (count == 0)
        {
            return slice(begin);
        }
        return slice(begin, begin + count);
    }

    size_t TArrayView<const void>::revIndex(ssize_t idx) const
    {
        if (idx < 0)
        {
            return m_size - idx;
        }
        return static_cast<size_t>(idx);
    }

    bool TArrayView<const void>::operator!=(const TArrayView<const void>& other) const { return !(*this == other); }

    TArrayView<const void>& TArrayView<const void>::operator=(const TArrayView<const void>& other)
    {
        m_data = other.data();
        m_size = other.size();
        return *this;
    }

    template <class T>
    TArrayView<const void>& TArrayView<const void>::operator=(const TArrayView<const T>& other)
    {
        m_data = other.data();
        m_size = other.size() * sizeof(T);
        return *this;
    }

    size_t TArrayView<const void>::size() const { return m_size; }

    const void* TArrayView<const void>::data() const { return m_data; }

    const void* TArrayView<const void>::begin() const { return m_data; }

    const void* TArrayView<const void>::end() const { return ptrCast<void>(ptrCast<uint8_t>(m_data) + m_size); }

    // free functions

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
