#pragma once
#include <ostream>

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
    };

    struct PrintAllOptions: public PrintOptions
    {
        static const bool print_calculated_values = true;
    };

    template<class Options = PrintOptions, class T>
    void printStruct(std::ostream& os, const T& obj);

}

#include "print-inl.hpp"
