#pragma once
namespace ct
{

    template<class AR, class T>
    void loadStruct(AR& ar, T& obj);

    template<class AR, class T>
    void saveStruct(AR& ar, const T& obj);

}

#include "cerealize-inl.hpp"
