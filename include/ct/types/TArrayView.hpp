#ifndef CT_ARRAY_VIEW_HPP
#define CT_ARRAY_VIEW_HPP
#include "../config.hpp"
#include "../type_traits.hpp"
#include <minitensor/Tensor.hpp>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
namespace ct
{
    template <class T = uint8_t, class U>
    CT_DEVICE_INLINE T* ptrCast(U* ptr)
    {
        return static_cast<T*>(static_cast<void*>(ptr));
    }

    template <class T = uint8_t, class U>
    CT_DEVICE_INLINE const T* ptrCast(const U* ptr)
    {
        return static_cast<const T*>(static_cast<const void*>(ptr));
    }

    struct TArrayViewTag
    {
    };

    template <class T, ssize_t N = -1>
    struct TArrayView;

    template <class T, ssize_t N, class DERIVED>
    struct TArrayBaseConst : TArrayViewTag
    {
        using value_type = T;
        CT_DEVICE_INLINE const T* begin() const;
        CT_DEVICE_INLINE const T* end() const;
        CT_DEVICE_INLINE size_t revIndex(ssize_t) const;
        CT_DEVICE_INLINE bool empty() const { return static_cast<const DERIVED*>(this)->size() == 0; }

        CT_DEVICE_INLINE TArrayView<const T, N> slice(ssize_t begin, ssize_t end = 0) const;
        CT_DEVICE_INLINE TArrayView<const T, N> subView(ssize_t begin, size_t count = 0) const;

        void copyTo(TArrayView<T, N>& dest) const;
    };

    template <class T, ssize_t N, class DERIVED>
    struct TArrayBase : public TArrayBaseConst<T, N, DERIVED>
    {
        CT_DEVICE_INLINE T* begin();
        CT_DEVICE_INLINE T* end();
        CT_DEVICE_INLINE const T* begin() const;
        CT_DEVICE_INLINE const T* end() const;

        CT_DEVICE_INLINE TArrayView<T, N> slice(ssize_t begin, ssize_t end = 0);
        CT_DEVICE_INLINE TArrayView<T, N> subView(ssize_t begin, size_t count = 0);
        CT_DEVICE_INLINE TArrayView<const T, N> slice(ssize_t begin, ssize_t end = 0) const;
        CT_DEVICE_INLINE TArrayView<const T, N> subView(ssize_t begin, size_t count = 0) const;
    };

    template <class T, ssize_t N>
    struct TArrayView : public TArrayBase<T, N, TArrayView<T, N>>
    {
        CT_DEVICE_INLINE TArrayView(T* ptr = nullptr);

        CT_DEVICE_INLINE const T& operator[](ssize_t idx) const;
        CT_DEVICE_INLINE T& operator[](ssize_t idx);

        CT_DEVICE_INLINE bool operator==(const TArrayView& other) const;
        CT_DEVICE_INLINE bool operator!=(const TArrayView& other) const;

        CT_DEVICE_INLINE size_t size() const;

        CT_DEVICE_INLINE T* data();
        CT_DEVICE_INLINE const T* data() const;

        CT_DEVICE_INLINE operator TArrayView<void, N * sizeof(T)>() &;
        CT_DEVICE_INLINE operator TArrayView<void, N * sizeof(T)>() &&;

        CT_DEVICE_INLINE operator TArrayView<const T, N>() const;
        CT_DEVICE_INLINE operator TArrayView<const T, -1>() const;

      protected:
        T* m_data;
    };

    template <class T, ssize_t N>
    struct TArrayView<const T, N> : public TArrayBaseConst<T, N, TArrayView<const T, N>>
    {
        CT_DEVICE_INLINE TArrayView(const T* ptr = nullptr);

        CT_DEVICE_INLINE const T& operator[](ssize_t idx) const;

        CT_DEVICE_INLINE bool operator==(const TArrayView& other) const;
        CT_DEVICE_INLINE bool operator!=(const TArrayView& other) const;

        CT_DEVICE_INLINE size_t size() const;

        CT_DEVICE_INLINE const T* data() const;

        CT_DEVICE_INLINE operator TArrayView<const void, N * sizeof(T)>() const&;
        CT_DEVICE_INLINE operator TArrayView<const void, N * sizeof(T)>() &&;

      protected:
        const T* m_data;
    };

    template <class T>
    struct TArrayView<T, -1> : public TArrayBase<T, -1, TArrayView<T, -1>>
    {
        CT_DEVICE_INLINE TArrayView(T* ptr = nullptr, size_t sz = 0);
        CT_DEVICE_INLINE TArrayView(T* begin, T* end);
        CT_DEVICE_INLINE TArrayView(mt::Tensor<T, 1> tensor);
        TArrayView(std::vector<T>& vector);

        CT_DEVICE_INLINE const T& operator[](ssize_t idx) const;
        CT_DEVICE_INLINE T& operator[](ssize_t idx);

        CT_DEVICE_INLINE bool operator==(const TArrayView& other) const;
        CT_DEVICE_INLINE bool operator!=(const TArrayView& other) const;

        CT_DEVICE_INLINE size_t size() const;

        CT_DEVICE_INLINE T* data();
        CT_DEVICE_INLINE const T* data() const;

        CT_DEVICE_INLINE operator TArrayView<void, -1>() &;
        CT_DEVICE_INLINE operator TArrayView<void, -1>() &&;
        CT_DEVICE_INLINE operator TArrayView<const T, -1>() const;

      protected:
        T* m_data;
        size_t m_size;
    };

    template <class T>
    struct TArrayView<const T, -1> : public TArrayBaseConst<T, -1, TArrayView<const T, -1>>
    {
        CT_DEVICE_INLINE TArrayView(const T* ptr = nullptr, size_t sz = 0);
        CT_DEVICE_INLINE TArrayView(const T* begin, const T* end);

        TArrayView(const std::vector<T>& vector);

        CT_DEVICE_INLINE TArrayView& operator=(const TArrayView<T>& other);

        CT_DEVICE_INLINE const T& operator[](ssize_t idx) const;
        CT_DEVICE_INLINE bool operator==(const TArrayView& other) const;
        CT_DEVICE_INLINE bool operator!=(const TArrayView& other) const;

        CT_DEVICE_INLINE size_t size() const;
        CT_DEVICE_INLINE const T* data() const;

        CT_DEVICE_INLINE operator TArrayView<const void, -1>() const&;
        CT_DEVICE_INLINE operator TArrayView<const void, -1>() &&;

      protected:
        const T* m_data;
        size_t m_size;
    };

    template <>
    struct TArrayView<void, -1> : public TArrayBase<void, -1, TArrayView<void, -1>>
    {
        CT_DEVICE_INLINE TArrayView(void* ptr = nullptr, size_t sz = 0);
        CT_DEVICE_INLINE TArrayView(void* begin, void* end);
        CT_DEVICE_INLINE bool operator==(const TArrayView& other) const;
        CT_DEVICE_INLINE bool operator!=(const TArrayView& other) const;

        template <class T>
        CT_DEVICE_INLINE TArrayView& operator=(const TArrayView<T>& other);

        CT_DEVICE_INLINE size_t size() const;

        CT_DEVICE_INLINE void* data();
        CT_DEVICE_INLINE const void* data() const;

        template <class U>
        CT_DEVICE_INLINE operator TArrayView<U, -1>() &;
        template <class U>
        CT_DEVICE_INLINE operator TArrayView<U, -1>() &&;

        CT_DEVICE_INLINE operator TArrayView<const void>() const;

      protected:
        void* m_data;
        size_t m_size;
    };

    template <>
    struct TArrayView<const void, -1> : public TArrayBase<const void, -1, TArrayView<const void, -1>>
    {
        CT_DEVICE_INLINE TArrayView(const void* ptr = nullptr, size_t sz = 0);
        CT_DEVICE_INLINE TArrayView(const void* begin, const void* end);

        CT_DEVICE_INLINE bool operator==(const TArrayView& other) const;
        CT_DEVICE_INLINE bool operator!=(const TArrayView& other) const;

        template <class T>
        CT_DEVICE_INLINE TArrayView& operator=(const TArrayView<const T>& other);

        CT_DEVICE_INLINE TArrayView& operator=(const TArrayView<const void>& other);

        CT_DEVICE_INLINE size_t size() const;

        CT_DEVICE_INLINE const void* data() const;

        template <class U>
        CT_DEVICE_INLINE operator TArrayView<const U, -1>() const&;
        template <class U>
        CT_DEVICE_INLINE operator TArrayView<const U, -1>() &&;

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
    CT_DEVICE_INLINE const T* TArrayBaseConst<T, N, D>::begin() const
    {
        return static_cast<const D*>(this)->data();
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE const T* TArrayBaseConst<T, N, D>::end() const
    {
        auto This = static_cast<const D*>(this);
        return This->data() + This->size();
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE size_t TArrayBaseConst<T, N, D>::revIndex(ssize_t idx) const
    {
        if (idx < 0)
        {
            return static_cast<const D*>(this)->size() - idx;
        }
        return static_cast<size_t>(idx);
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE TArrayView<const T, N> TArrayBaseConst<T, N, D>::slice(ssize_t begin, ssize_t end) const
    {
        const T* ptr_begin = static_cast<const D*>(this)->data() + revIndex(begin);
        const T* ptr_end =
            static_cast<const D*>(this)->data() + (end == 0 ? static_cast<const D*>(this)->size() : revIndex(end));
        return TArrayView<const T>(ptr_begin, ptr_end);
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE TArrayView<const T, N> TArrayBaseConst<T, N, D>::subView(ssize_t begin, size_t count) const
    {
        if (count == 0)
        {
            return slice(begin);
        }
        return slice(begin, begin + count);
    }

    template <class T, ssize_t N, class D>
    void TArrayBaseConst<T, N, D>::copyTo(TArrayView<T, N>& dest) const
    {
        const size_t size = static_cast<const D*>(this)->size();
        assert(dest.size() == size);
        std::copy(begin(), end(), dest.data());
    }

    ///////////////////////////////////////////////////////////
    // TArrayBase
    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE T* TArrayBase<T, N, D>::begin()
    {
        return static_cast<D*>(this)->data();
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE T* TArrayBase<T, N, D>::end()
    {
        return static_cast<D*>(this)->data() + static_cast<D*>(this)->size();
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE const T* TArrayBase<T, N, D>::begin() const
    {
        return TArrayBaseConst<T, N, D>::begin();
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE const T* TArrayBase<T, N, D>::end() const
    {
        return TArrayBaseConst<T, N, D>::end();
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE TArrayView<T, N> TArrayBase<T, N, D>::slice(ssize_t begin, ssize_t end)
    {
        T* ptr_begin = static_cast<D*>(this)->data() + this->revIndex(begin);
        T* ptr_end = static_cast<D*>(this)->data() + (end == 0 ? static_cast<D*>(this)->size() : this->revIndex(end));
        return TArrayView<T>(ptr_begin, ptr_end);
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE TArrayView<T, N> TArrayBase<T, N, D>::subView(ssize_t begin, size_t count)
    {
        if (count == 0)
        {
            return slice(begin);
        }
        return slice(begin, begin + count);
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE TArrayView<const T, N> TArrayBase<T, N, D>::slice(ssize_t begin, ssize_t end) const
    {
        return TArrayBaseConst<T, N, D>::slice(begin, end);
    }

    template <class T, ssize_t N, class D>
    CT_DEVICE_INLINE TArrayView<const T, N> TArrayBase<T, N, D>::subView(ssize_t begin, size_t count) const
    {
        return TArrayBaseConst<T, N, D>::subView(begin, count);
    }

    ///////////////////////////////////////////////////////////
    // TArrayView<T, N>

    template <class T, ssize_t N>
    CT_DEVICE_INLINE TArrayView<T, N>::TArrayView(T* ptr) : m_data(ptr)
    {
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE const T& TArrayView<T, N>::operator[](ssize_t idx) const
    {
        return m_data[this->revIndex(idx)];
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE T& TArrayView<T, N>::operator[](ssize_t idx)
    {
        return m_data[this->revIndex(idx)];
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE bool TArrayView<T, N>::operator==(const TArrayView& other) const
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
    CT_DEVICE_INLINE bool TArrayView<T, N>::operator!=(const TArrayView& other) const
    {
        return !(*this == other);
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE size_t TArrayView<T, N>::size() const
    {
        return N;
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE T* TArrayView<T, N>::data()
    {
        return m_data;
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE const T* TArrayView<T, N>::data() const
    {
        return m_data;
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE TArrayView<T, N>::operator TArrayView<void, N * sizeof(T)>() &
    {
        return {m_data};
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE TArrayView<T, N>::operator TArrayView<void, N * sizeof(T)>() &&
    {
        TArrayView<void, N * sizeof(T)> ret(m_data);
        m_data = nullptr;
        return ret;
    }

    ///////////////////////////////////////////////////////////
    // TArrayView<const T, N>

    template <class T, ssize_t N>
    CT_DEVICE_INLINE TArrayView<const T, N>::TArrayView(const T* ptr) : m_data(ptr)
    {
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE const T& TArrayView<const T, N>::operator[](ssize_t idx) const
    {
        return m_data[this->revIndex(idx)];
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE bool TArrayView<const T, N>::operator==(const TArrayView& other) const
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
    CT_DEVICE_INLINE bool TArrayView<const T, N>::operator!=(const TArrayView& other) const
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
    CT_DEVICE_INLINE size_t TArrayView<const T, N>::size() const
    {
        return N;
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE const T* TArrayView<const T, N>::data() const
    {
        return m_data;
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE TArrayView<const T, N>::operator TArrayView<const void, N * sizeof(T)>() const&
    {
        return {m_data};
    }

    template <class T, ssize_t N>
    CT_DEVICE_INLINE TArrayView<const T, N>::operator TArrayView<const void, N * sizeof(T)>() &&
    {
        TArrayView<const void, N * sizeof(T)> ret(m_data);
        m_data = nullptr;
        return ret;
    }

    ///////////////////////////////////////////////////////////
    // TArrayView

    template <class T>
    CT_DEVICE_INLINE TArrayView<T>::TArrayView(T* ptr, size_t sz) : m_data(ptr), m_size(sz)
    {
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<T>::TArrayView(T* begin, T* end) : m_data(begin), m_size(end - begin)
    {
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<T>::TArrayView(mt::Tensor<T, 1> tensor)
    {
        assert(tensor.getShape().isContinuous());
        m_data = tensor.data();
        m_size = tensor.getShape()[0];
    }

    template <class T>
    TArrayView<T>::TArrayView(std::vector<T>& vector)
    {
        m_data = vector.data();
        m_size = vector.size();
    }

    template <class T>
    CT_DEVICE_INLINE const T& TArrayView<T>::operator[](ssize_t idx) const
    {
        return m_data[this->revIndex(idx)];
    }

    template <class T>
    CT_DEVICE_INLINE T& TArrayView<T>::operator[](ssize_t idx)
    {
        return m_data[this->revIndex(idx)];
    }

    template <class T>
    CT_DEVICE_INLINE bool TArrayView<T>::operator==(const TArrayView<T>& other) const
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
    CT_DEVICE_INLINE bool TArrayView<T>::operator!=(const TArrayView<T>& other) const
    {
        return !(*this == other);
    }

    template <class T>
    CT_DEVICE_INLINE size_t TArrayView<T>::size() const
    {
        return m_size;
    }

    template <class T>
    CT_DEVICE_INLINE T* TArrayView<T>::data()
    {
        return m_data;
    }

    template <class T>
    CT_DEVICE_INLINE const T* TArrayView<T>::data() const
    {
        return m_data;
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<T>::operator TArrayView<void, -1>() &
    {
        TArrayView<void, -1> ret(data(), sizeof(T) * size());
        return ret;
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<T>::operator TArrayView<void, -1>() &&
    {
        TArrayView<void, -1> ret(data(), sizeof(T) * size());
        m_data = nullptr;
        m_size = 0;
        return ret;
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<T>::operator TArrayView<const T, -1>() const
    {
        TArrayView<const T, -1> ret(data(), size());
        return ret;
    }

    ///////////////////////////////////////////////////////////////////////
    // const specialzation

    template <class T>
    CT_DEVICE_INLINE TArrayView<const T>::TArrayView(const T* ptr, size_t sz) : m_data(ptr), m_size(sz)
    {
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<const T>::TArrayView(const T* begin, const T* end) : m_data(begin), m_size(end - begin)
    {
    }

    template <class T>
    TArrayView<const T>::TArrayView(const std::vector<T>& vector) : m_data(vector.data()), m_size(vector.size())
    {
    }

    template <class T>
    CT_DEVICE_INLINE const T& TArrayView<const T>::operator[](ssize_t idx) const
    {
        return m_data[idx];
    }

    template <class T>
    CT_DEVICE_INLINE bool TArrayView<const T>::operator==(const TArrayView<const T>& other) const
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
    CT_DEVICE_INLINE bool TArrayView<const T>::operator!=(const TArrayView<const T>& other) const
    {
        return !(*this == other);
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<const T>& TArrayView<const T>::operator=(const TArrayView<T>& other)
    {
        m_data = other.data();
        m_size = other.size();
        return *this;
    }

    template <class T>
    CT_DEVICE_INLINE size_t TArrayView<const T>::size() const
    {
        return m_size;
    }

    template <class T>
    CT_DEVICE_INLINE const T* TArrayView<const T>::data() const
    {
        return m_data;
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<const T>::operator TArrayView<const void, -1>() const&
    {
        TArrayView<const void, -1> ret(data(), sizeof(T) * size());
        return ret;
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<const T>::operator TArrayView<const void, -1>() &&
    {
        TArrayView<const void, -1> ret(data(), sizeof(T) * size());
        m_data = nullptr;
        m_size = 0;
        return ret;
    }

    // void specialization

    CT_DEVICE_INLINE TArrayView<void>::TArrayView(void* ptr, size_t sz) : m_data(ptr), m_size(sz) {}

    CT_DEVICE_INLINE TArrayView<void>::TArrayView(void* begin, void* end)
        : m_data(begin), m_size(static_cast<size_t>(ptrCast<uint8_t>(end) - ptrCast<uint8_t>(begin)))
    {
    }

    CT_DEVICE_INLINE bool TArrayView<void>::operator==(const TArrayView<void>& other) const
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

    CT_DEVICE_INLINE bool TArrayView<void>::operator!=(const TArrayView<void>& other) const
    {
        return !(*this == other);
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<void>& TArrayView<void>::operator=(const TArrayView<T>& other)
    {
        m_data = other.data();
        m_size = other.size() * sizeof(T);
        return *this;
    }

    CT_DEVICE_INLINE size_t TArrayView<void>::size() const { return m_size; }

    CT_DEVICE_INLINE void* TArrayView<void>::data() { return m_data; }

    CT_DEVICE_INLINE const void* TArrayView<void>::data() const { return m_data; }

    template <class T>
    CT_DEVICE_INLINE TArrayView<void>::operator TArrayView<T, -1>() &
    {
        TArrayView<T, -1> ret(ptrCast<T>(data()), size() / sizeof(T));
        return ret;
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<void>::operator TArrayView<T, -1>() &&
    {
        TArrayView<T, -1> ret(ptrCast<T>(data()), size() / sizeof(T));
        m_data = nullptr;
        m_size = 0;
        return ret;
    }

    CT_DEVICE_INLINE TArrayView<void>::operator TArrayView<const void, -1>() const
    {
        TArrayView<const void, -1> ret(data(), size());
        return ret;
    }

    // const void specialzation

    CT_DEVICE_INLINE TArrayView<const void>::TArrayView(const void* ptr, size_t sz) : m_data(ptr), m_size(sz) {}

    CT_DEVICE_INLINE TArrayView<const void>::TArrayView(const void* begin, const void* end)
        : m_data(begin), m_size(static_cast<size_t>(ptrCast<uint8_t>(end) - ptrCast<uint8_t>(begin)))
    {
    }

    CT_DEVICE_INLINE bool TArrayView<const void>::operator==(const TArrayView<const void>& other) const
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

    CT_DEVICE_INLINE bool TArrayView<const void>::operator!=(const TArrayView<const void>& other) const
    {
        return !(*this == other);
    }

    CT_DEVICE_INLINE TArrayView<const void>& TArrayView<const void>::operator=(const TArrayView<const void>& other)
    {
        m_data = other.data();
        m_size = other.size();
        return *this;
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<const void>& TArrayView<const void>::operator=(const TArrayView<const T>& other)
    {
        m_data = other.data();
        m_size = other.size() * sizeof(T);
        return *this;
    }

    CT_DEVICE_INLINE size_t TArrayView<const void>::size() const { return m_size; }

    CT_DEVICE_INLINE const void* TArrayView<const void>::data() const { return m_data; }

    template <class T>
    CT_DEVICE_INLINE TArrayView<const void>::operator TArrayView<const T, -1>() const&
    {
        TArrayView<const T, -1> ret(ptrCast<const T>(data()), size() / sizeof(T));
        return ret;
    }

    template <class T>
    CT_DEVICE_INLINE TArrayView<const void>::operator TArrayView<const T, -1>() &&
    {
        TArrayView<const T, -1> ret(ptrCast<const T>(data()), size() / sizeof(T));
        m_data = nullptr;
        m_size = 0;
        return ret;
    }

    // free functions

    template <class T, ssize_t N>
    TArrayView<T, N> makeArrayView(TArrayView<T, N> view)
    {
        return view;
    }

    template <class T, ssize_t N>
    TArrayView<T, N> makeArrayView(TArrayView<T, N> view, size_t)
    {
        return view;
    }

    template <class T>
    TArrayView<T> makeArrayView(T* ptr, size_t size)
    {
        return TArrayView<T>(ptr, size);
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

    template <class T>
    struct IsArrayView
    {
        static constexpr const bool value = IsBase<Base<TArrayViewTag>, Derived<T>>::value;
    };
} // namespace ct

#endif // CT_ARRAY_VIEW_HPP
