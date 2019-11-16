#include <ct/StringView.hpp>
#include <ct/hash.hpp>

int main()
{
    constexpr ct::StringView view("askldjflkjasldkfjlkasjflksajdflkjsalkfjlaskjflkjsadlkfjaslkdjf");
    constexpr uint32_t hash = ct::crc32(view);
    static_assert(hash != 0, "");
}
