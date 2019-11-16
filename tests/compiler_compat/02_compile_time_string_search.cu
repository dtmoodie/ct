#include <ct/StringView.hpp>

int main()
{
    constexpr ct::StringView view("askldjflkjasldkfjlkasjflksajdflkjsalkfjlaskjflkjsadlkfjaslkdjf");
    static_assert(view.find("skldjf") == 1, "");
}
