#pragma once

#include "Info.hpp"

#include <nil/xit/tagged/add_value.hpp>
#include <nil/xit/tagged/structs.hpp>

#include <memory>
#include <string_view>
#include <unordered_map>

namespace nil::xit::test::frame::view::test
{
    struct Info final: view::IInfo
    {
        nil::xit::tagged::Frame* frame = nullptr;

        template <typename V>
        void add_value(std::string id, V seed_value)
        {
            struct XitAccessor: nil::xit::tagged::IAccessor<V>
            {
                explicit XitAccessor(V init_seed_value)
                    : seed_value(std::move(init_seed_value))
                {
                }

                V get(std::string_view tag) const override
                {
                    auto [it, inserted] = values.try_emplace(std::string(tag), seed_value);
                    (void)inserted;
                    return it->second;
                }

                void set(std::string_view tag, V new_data) override
                {
                    auto [it, inserted] = values.try_emplace(std::string(tag), seed_value);
                    (void)inserted;
                    it->second = std::move(new_data);
                }

                mutable std::unordered_map<std::string, V> values;
                V seed_value;
            };

            nil::xit::tagged::add_value(
                *frame,
                std::move(id),
                std::make_unique<XitAccessor>(std::move(seed_value))
            );
        }
    };
}
