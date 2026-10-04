// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

#pragma once

#include "Info.hpp"

#include <nil/xit/unique/add_value.hpp>
#include <nil/xit/unique/structs.hpp>

#include <utility>

namespace nil::xit::test::frame::view::global
{
    struct Info final: view::IInfo
    {
        nil::xit::unique::Frame* frame = nullptr;

        template <typename V>
        void add_value(std::string id, V seed_value)
        {
            struct XitAccessor: nil::xit::unique::IAccessor<V>
            {
                explicit XitAccessor(V init_seed_value)
                    : value(std::move(init_seed_value))
                {
                }

                V get() const override
                {
                    return value;
                }

                void set(V new_data) override
                {
                    value = std::move(new_data);
                }

                V value;
            };

            nil::xit::unique::add_value(
                *frame,
                std::move(id),
                std::make_unique<XitAccessor>(std::move(seed_value))
            );
        }
    };
}
