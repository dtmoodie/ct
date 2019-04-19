#include <ct/VariadicTypedef.hpp>
#include <type_traits>

int main()
{
    using float_double = ct::VariadicTypedef<float, double>;
    static_assert(std::is_same<float_double, ct::VariadicTypedef<float, double>>::value, "");
    static_assert( std::is_same<ct::toTuple<float_double>, std::tuple<float, double>>::value, "");

    using float_double_int = ct::append<float_double, int>;
    static_assert(std::is_same<float_double_int, ct::VariadicTypedef<float, double, int>>::value, "");
    static_assert(std::is_same<ct::toTuple<float_double_int>, std::tuple<float, double, int>>::value, "");

    using int_float_double = ct::append<int, float_double>;
    static_assert(std::is_same<int_float_double, ct::VariadicTypedef<int, float, double>>::value, "");
    static_assert(std::is_same<ct::toTuple<int_float_double>, std::tuple<int, float, double>>::value, "");

    return 0;
}