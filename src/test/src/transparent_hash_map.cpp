#include <nil/xit/test/transparent_hash_map.hpp>

namespace nil::xit::test::transparent
{
    std::size_t Hash::operator()(std::string_view s) const
    {
        return std::hash<std::string_view>()(s);
    }

    bool Equal::operator()(std::string_view l, std::string_view r) const
    {
        return l == r;
    }
}
