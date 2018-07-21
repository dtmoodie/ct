#include "common.hpp"
#include "Reflect.hpp"
#include <ct/reflect/print.hpp>
#include <iostream>

namespace std
{
    template<class T>
    ostream& operator << (ostream& os, const std::vector<T>& vec)
    {
        if (!vec.empty())
        {
            os << '[';
            for (size_t i = 0; i < vec.size(); ++i)
            {
                if (i != 0)
                    os << ' ';
                os << vec[i];
            }
            os << ']';
        }
        return os;
    }
}

struct Printer
{
    template<class T>
    void test(T& data)
    {
        std::cout << "\n====================\n";
        std::cout << ct::Reflect<T>::getName() << std::endl;
        ct::printStruct(std::cout, data);
        std::cout << std::endl;
    }
};

int main()
{
    Printer printer;
    testTypes(printer);
}