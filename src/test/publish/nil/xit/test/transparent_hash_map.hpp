#pragma once

#include <string_view>
#include <unordered_map>

namespace nil::xit::test::transparent
{
    struct Hash
    {
        using is_transparent = void;

        std::size_t operator()(std::string_view s) const;
    };

    struct Equal
    {
        using is_transparent = void;

        bool operator()(std::string_view l, std::string_view r) const;
    };

    template <typename T>
    using hash_map = std::unordered_map<std::string, T, Hash, Equal>;

}
