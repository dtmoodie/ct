#pragma once
#include <vector>
namespace ct
{
    template<class T> struct Indexable
    {
        enum
        {
            ConstSize = 0,
            Dims = 0,
            IsIndexable = 0
        };
    };

    template<class T> struct Indexable<std::vector<T>>
    {
        enum
        {
            ConstSize = 0,
            Dims = 1,
            IsIndexable = 1
        };
    };
}