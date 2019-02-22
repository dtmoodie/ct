#ifndef CT_ARRAY_VIEW_HPP
#define CT_ARRAY_VIEW_HPP
#include <ct/type_traits.hpp>

#include <cereal/cereal.hpp>
#include <cstdint>
#include <iostream>

namespace ct
{
    template <class T>
    struct TArrayView
    {

        bool operator==(const TArrayView& other) const
        {
            if (other.data == data && other.size == size)
            {
                return true;
            }
            if (other.size != size)
            {
                return false;
            }

            for (size_t i = 0; i < size; ++i)
            {
                if (data[i] != other.data[i])
                {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const TArrayView& other) const { return !(*this == other); }

        template <class AR>
        void serialize(AR& ar)
        {
            ar(cereal::make_size_tag(size));
            for (size_t i = 0; i < size; ++i)
            {
                ar(data[i]);
            }
        }

        T* data;
        size_t size;
    };

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
        for (size_t i = 0; i < view.size; ++i)
        {
            if (i != 0)
            {
                os << " ";
            }
            os << view.data[i];
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
