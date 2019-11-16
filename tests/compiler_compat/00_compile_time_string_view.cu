#include <ct/StringView.hpp>

int main()
{
    constexpr ct::StringView view("asdf");
    static_assert(view.size() == 4, "");
    return 0;
}