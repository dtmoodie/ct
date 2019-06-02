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

    template <class T, ssize_t N = -1>
    struct TArrayView;

    template <class T, ssize_t N, class DERIVED>
    struct TArrayBaseConst
    {
        const T* begin() const;
        const T* end() const;
        size_t revIndex(ssize_t) const;

        TArrayView<const T, N> slice(ssize_t begin, ssize_t end = 0) const;
        TArrayView<const T, N> subView(ssize_t begin, size_t count = 0) const;
    };

    template <class T, ssize_t N, class DERIVED>
    struct TArrayBase : public TArrayBaseConst<T, N, DERIVED>
    {
        T* begin();
        T* end();
        const T* begin() const;
        const T* end() const;

        TArrayView<T, N> slice(ssize_t begin, ssize_t end = 0);
        TArrayView<T, N> subView(ssize_t begin, size_t count = 0);
        TArrayView<const T, N> slice(ssize_t begin, ssize_t end = 0) const;
        TArrayView<const T, N> subView(ssize_t begin, size_t count = 0) const;
    };

    template <class T, ssize_t N>
    struct TArrayView : public TArrayBase<T, N, TArrayView<T, N>>
    {
        TArrayView(TArrayView<void>&);
        TArrayView(TArrayView<void>&&);
        TArrayView(T* ptr = nullptr);

        const T& operator[](ssize_t idx) const;
        T& operator[](ssize_t idx);

        bool operator==(const TArrayView& other) const;
        bool operator!=(const TArrayView& other) const;

        size_t size() const;

        T* data();
        const T* data() const;

      protected:
        T* m_data;
    };

    template <class T, ssize_t N>
    struct TArrayView<const T, N> : public TArrayBaseConst<T, N, TArrayView<const T, N>>
    {
        TArrayView(const TArrayView<const void>&);
        TArrayView(TArrayView<const void>&&);
        TArrayView(const T* ptr = nullptr);

        const T& operator[](ssize_t idx) const;

        bool operator==(const TArrayView& other) const;
        bool operator!=(const TArrayView& other) const;

        size_t size() const;

        const T* data() const;

      protected:
        const T* m_data;
    };

    template <class T>
    struct TArrayView<T, -1> : public TArrayBase<T, -1, TArrayView<T, -1>>
    {
        TArrayView(TArrayView<void>&);
        TArrayView(TArrayView<void>&&);
        TArrayView(T* ptr = nullptr, size_t sz = 0);
        TArrayView(T* begin, T* end);

        const T& operator[](ssize_t idx) const;
        T& operator[](ssize_t idx);

        bool operator==(const TArrayView& other) const;
        bool operator!=(const TArrayView& other) const;

        size_t size() const;

        T* data();
        const T* data() const;

      protected:
        T* m_data;
        size_t m_size;
    };

    template <class T>
    struct TArrayView<const T, -1> : public TArrayBaseConst<T, -1, TArrayView<const T, -1>>
    {
        TArrayView(const TArrayView<T>&);
        TArrayView(const TArrayView<const void>&);
        TArrayView(const T* ptr = nullptr, size_t sz = 0);
        TArrayView(const T* begin, const T* end);
        TArrayView& operator=(const TArrayView<T>& other);

        const T& operator[](ssize_t idx) const;
        bool operator==(const TArrayView& other) const;
        bool operator!=(const TArrayView& other) const;

        size_t size() const;

        const T* data() const;

      protected:
        const T* m_data;
        size_t m_size;
    };

    template <>
    struct TArrayView<void, -1> : public TArrayBase<void, -1, TArrayView<void, -1>>
    {
        template <class T>
        TArrayView(TArrayView<T>&& other);
        template <class T>
        TArrayView(TArrayView<T>& other);
        inline TArrayView(void* ptr = nullptr, size_t sz = 0);
        inline TArrayView(void* begin, void* end);

        inline bool operator==(const TArrayView& other) const;
        inline bool operator!=(const TArrayView& other) const;

        template <class T>
        TArrayView& operator=(const TArrayView<T>& other);

        inline size_t size() const;

        inline void* data();
        inline const void* data() const;

      protected:
        void* m_data;
        size_t m_size;
    };

    template <>
    struct TArrayView<const void, -1> : public TArrayBase<const void, -1, TArrayView<void, -1>>
    {
        template <class T>
        TArrayView(const TArrayView<const T>&);
        template <class T>
        TArrayView(const TArrayView<T>&);
        inline TArrayView(const void* ptr = nullptr, size_t sz = 0);
        inline TArrayView(const void* begin, const void* end);

        inline bool operator==(const TArrayView& other) const;
        inline bool operator!=(const TArrayView& other) const;

        template <class T>
        TArrayView& operator=(const TArrayView<const T>& other);

        inline TArrayView& operator=(const TArrayView<const void>& other);

        inline size_t size() const;

        inline const void* data() const;

      protected:
        const void* m_data;
        size_t m_size;
    };

    ///////////////////////////////////////////////////////////
    /// Implementation
    ///////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////
    // TArrayBaseConst
    template <class T, ssize_t N, class D>
    const T* TArrayBaseConst<T, N, D>::begin() const
    {
        return static_cast<const D*>(this)->data();
    }

    template <class T, ssize_t N, class D>
    const T* TArrayBaseConst<T, N, D>::end() const
    {
        auto This = static_cast<const D*>(this);
        return This->data() + This->size();
    }

    template <class T, ssize_t N, class D>
    size_t TArrayBaseConst<T, N, D>::revIndex(ssize_t idx) const
    {
        if (idx < 0)
        {
            return static_cast<const D*>(this)->size() - idx;
        }
        return static_cast<size_t>(idx);
    }

    template <class T, ssize_t N, class D>
    TArrayView<const T, N> TArrayBaseConst<T, N, D>::slice(ssize_t begin, ssize_t end) const
    {
        const T* ptr_begin = static_cast<const D*>(this)->data() + revIndex(begin);
        const T* ptr_end =
            static_cast<const D*>(this)->data() + (end == 0 ? static_cast<const D*>(this)->size() : revIndex(end));
        return TArrayView<const T>(ptr_begin, ptr_end);
    }

    template <class T, ssize_t N, class D>
    TArrayView<const T, N> TArrayBaseConst<T, N, D>::subView(ssize_t begin, size_t count) const
    {
        if (count == 0)
        {
            return slice(begin);
        }
        return slice(begin, begin + count);
    }

    ///////////////////////////////////////////////////////////
    // TArrayBase
    template <class T, ssize_t N, class D>
    T* TArrayBase<T, N, D>::begin()
    {
        return static_cast<D*>(this)->data();
    }

    template <class T, ssize_t N, class D>
    T* TArrayBase<T, N, D>::end()
    {
        return static_cast<D*>(this)->data() + static_cast<D*>(this)->size();
    }

    template <class T, ssize_t N, class D>
    const T* TArrayBase<T, N, D>::begin() const
    {
        return TArrayBaseConst<T, N, D>::begin();
    }

    template <class T, ssize_t N, class D>
    const T* TArrayBase<T, N, D>::end() const
    {
        return TArrayBaseConst<T, N, D>::end();
    }

    template <class T, ssize_t N, class D>
    TArrayView<T, N> TArrayBase<T, N, D>::slice(ssize_t begin, ssize_t end)
    {
        T* ptr_begin = static_cast<D*>(this)->data() + this->revIndex(begin);
        T* ptr_end = static_cast<D*>(this)->data() + (end == 0 ? static_cast<D*>(this)->size() : this->revIndex(end));
        return TArrayView<T>(ptr_begin, ptr_end);
    }

    template <class T, ssize_t N, class D>
    TArrayView<T, N> TArrayBase<T, N, D>::subView(ssize_t begin, size_t count)
    {
        if (count == 0)
        {
            return slice(begin);
        }
        return slice(begin, begin + count);
    }

    template <class T, ssize_t N, class D>
    TArrayView<const T, N> TArrayBase<T, N, D>::slice(ssize_t begin, ssize_t end) const
    {
        return TArrayBaseConst<T, N, D>::slice(begin, end);
    }

    template <class T, ssize_t N, class D>
    TArrayView<const T, N> TArrayBase<T, N, D>::subView(ssize_t begin, size_t count) const
    {
        return TArrayBaseConst<T, N, D>::subView(begin, count);
    }

    ///////////////////////////////////////////////////////////
    // TArrayView<T, N>
    template <class T, ssize_t N>
    TArrayView<T, N>::TArrayView(TArrayView<void>& other) : m_data(ptrCast<T>(other.data()))
    {
    }

    template <class T, ssize_t N>
    TArrayView<T, N>::TArrayView(TArrayView<void>&& other) : m_data(ptrCast<T>(other.data()))
    {
    }

    template <class T, ssize_t N>
    TArrayView<T, N>::TArrayView(T* ptr) : m_data(ptr)
    {
    }

    template <class T, ssize_t N>
    const T& TArrayView<T, N>::operator[](ssize_t idx) const
    {
        return m_data[this->revIndex(idx)];
    }

    template <class T, ssize_t N>
    T& TArrayView<T, N>::operator[](ssize_t idx)
    {
        return m_data[this->revIndex(idx)];
    }

    template <class T, ssize_t N>
    bool TArrayView<T, N>::operator==(const TArrayView& other) const
    {
        if (other.m_data == m_data)
        {
            return true;
        }

        for (size_t i = 0; i < N; ++i)
        {
            if (m_data[i] != other[i])
            {
                return false;
            }
        }
        return true;
    }

    template <class T, ssize_t N>
    bool TArrayView<T, N>::operator!=(const TArrayView& other) const
    {
        return !(*this == other);
    }

    template <class T, ssize_t N>
    size_t TArrayView<T, N>::size() const
    {
        return N;
    }

    template <class T, ssize_t N>
    T* TArrayView<T, N>::data()
    {
        return m_data;
    }

    template <class T, ssize_t N>
    const T* TArrayView<T, N>::data() const
    {
        return m_data;
    }

    ///////////////////////////////////////////////////////////
    // TArrayView<const T, N>

    template <class T, ssize_t N>
    TArrayView<const T, N>::TArrayView(const TArrayView<const void>& other) : m_data(ptrCast<T>(other.data()))
    {
    }

    template <class T, ssize_t N>
    TArrayView<const T, N>::TArrayView(TArrayView<const void>&& other) : m_data(ptrCast<T>(other.data()))
    {
    }

    template <class T, ssize_t N>
    TArrayView<const T, N>::TArrayView(const T* ptr) : m_data(ptr)
    {
    }

    template <class T, ssize_t N>
    const T& TArrayView<const T, N>::operator[](ssize_t idx) const
    {
        return m_data[this->revIndex(idx)];
    }

    template <class T, ssize_t N>
    bool TArrayView<const T, N>::operator==(const TArrayView& other) const
    {
        if (other.m_data == m_data)
        {
            return true;
        }

        for (size_t i = 0; i < N; ++i)
        {
            if (m_data[i] != other[i])
            {
                return false;
            }
        }
        return true;
    }

    template <class T, ssize_t N>
    bool TArrayView<const T, N>::operator!=(const TArrayView& other) const
    {
        if (other.m_data == m_data)
        {
            return true;
        }

        for (size_t i = 0; i < N; ++i)
        {
            if (m_data[i] != other[i])
            {
                return false;
            }
        }
        return true;
    }

    template <class T, ssize_t N>
    size_t TArrayView<const T, N>::size() const
    {
        return N;
    }

    template <class T, ssize_t N>
    const T* TArrayView<const T, N>::data() const
    {
        return m_data;
    }

    ///////////////////////////////////////////////////////////
    // TArrayView
    template <class T>
    TArrayView<T>::TArrayView(TArrayView<void>& other)
        : m_data(static_cast<T*>(other.data())), m_size(other.size() / sizeof(T))
    {
    }

    template <class T>
    TArrayView<T>::TArrayView(TArrayView<void>&& other)
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
        return m_data[this->revIndex(idx)];
    }

    template <class T>
    T& TArrayView<T>::operator[](ssize_t idx)
    {
        return m_data[this->revIndex(idx)];
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

    ///////////////////////////////////////////////////////////////////////
    // const specialzation

    template <class T>
    TArrayView<const T>::TArrayView(const TArrayView<const void>& other)
        : m_data(static_cast<const T*>(other.data())), m_size(other.size() / sizeof(T))
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

    // void specialization

    template <class T>
    TArrayView<void>::TArrayView(TArrayView<T>&& other) : m_data(other.data()), m_size(other.size() * sizeof(T))
    {
    }

    template <class T>
    TArrayView<void>::TArrayView(TArrayView<T>& other) : m_data(other.data()), m_size(other.size() * sizeof(T))
    {
    }

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

    bool TArrayView<void>::operator!=(const TArrayView<void>& other) const { return !(*this == other); }

    template <class T>
    TArrayView<void>& TArrayView<void>::operator=(const TArrayView<T>& other)
    {
        m_data = other.data();
        m_size = other.size() * sizeof(T);
        return *this;
    }

    size_t TArrayView<void>::size() const { return m_size; }

    void* TArrayView<void>::data() { return m_data; }
    const void* TArrayView<void>::data() const { return m_data; }

    // const void specialzation

    template <class T>
    TArrayView<const void>::TArrayView(const TArrayView<const T>& other)
        : m_data(other.data()), m_size(other.size() * sizeof(T))
    {
    }

    template <class T>
    TArrayView<const void>::TArrayView(const TArrayView<T>& other)
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
