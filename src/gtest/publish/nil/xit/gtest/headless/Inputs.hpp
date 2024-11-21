#pragma once

#include <nil/xit/test/transparent_hash_map.hpp>

#include <memory>
#include <string_view>

namespace nil::xit::gtest::headless
{
    struct ICache
    {
        ICache() = default;
        virtual ~ICache() = default;
        ICache(ICache&&) = delete;
        ICache(const ICache&) = delete;
        ICache& operator=(ICache&&) = delete;
        ICache& operator=(const ICache&) = delete;
    };

    template <typename T>
    struct Cache: ICache
    {
        virtual T get(std::string_view) const = 0;
    };

    struct Inputs
    {
        template <typename T>
        T get(std::string_view id, const std::string& tag) const
        {
            if (const auto it = values.find(id); it != values.end())
            {
                return static_cast<Cache<T>*>(it->second.get())->get(tag);
            }
            return T();
        }

        test::transparent::hash_map<std::unique_ptr<ICache>> values;
    };
}
