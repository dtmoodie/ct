#pragma once
#include "../reflect.hpp"
#include <ostream>
#include <typeinfo>

namespace ct
{
struct PrintOptions
{
    static const bool print_name = true;
    static const char name_separator = ':';
    static const char value_separator = ' ';
    static const bool print_calculated_values = false;
    static const char object_begin = '(';
    static const char object_end = ')';
    static const char array_begin = '[';
    static const char array_end = ']';
    static const bool error_on_nonprintable = true;
    // overload this function to print something to the ostream when a datatype is not printable
    template <class T>
    static std::ostream& onUnprintable(std::ostream& os, const char* /*name*/, const T& /*data*/)
    {
        return os;
    }
};

struct SkipUnprintable : public PrintOptions
{
    static const bool error_on_nonprintable = false;
    template <class T>
    static std::ostream& onUnprintable(std::ostream& os, const char* name, const T& data)
    {
        os << "Unable to print '" << name << "' o << operator available for " << typeid(T).name();
        return os;
    }
};

struct PrintAllOptions : public PrintOptions
{
    static const bool print_calculated_values = true;
};

template <class Options = PrintOptions, class T>
auto printStruct(std::ostream& os, const T& obj) -> ct::enable_if_reflected<T>;
}

#include "print-inl.hpp"
