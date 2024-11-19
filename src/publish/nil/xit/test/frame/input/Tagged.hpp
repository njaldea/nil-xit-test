#pragma once

#include "Info.hpp"

#include "../../transparent_hash_map.hpp"

#include <nil/xit/tagged/add_value.hpp>
#include <nil/xit/tagged/structs.hpp>

#include <nil/gate/Core.hpp>

#include <functional>

namespace nil::xit::test::frame::input::tagged
{
    template <typename T>
    struct Info final: input::Info<T>
    {
        struct Entry
        {
            std::optional<T> data;
            nil::gate::edges::Mutable<T>* input = nullptr;
        };

        nil::xit::tagged::Frame* frame = nullptr;
        nil::gate::Core* gate = nullptr;
        std::function<T(std::string_view)> initializer;
        transparent::hash_map<Entry> info;

        nil::gate::edges::Compatible<T> get_input(std::string_view tag) override
        {
            if (const auto it = info.find(tag); it != info.end())
            {
                return it->second.input;
            }
            return info.emplace(tag, typename Info<T>::Entry{std::nullopt, gate->edge<T>()})
                .first->second.input;
        }

        template <typename V, typename Getter, typename Setter>
            requires requires(Getter g, Setter s) {
                { g(std::declval<const T&>()) } -> std::same_as<V>;
                { s(std::declval<T&>(), std::declval<V>()) } -> std::same_as<void>;
            }
        void add_value(std::string id, Getter getter, Setter setter)
        {
            nil::xit::tagged::add_value(
                *frame,
                id,
                [this, getter = std::move(getter)](std::string_view tag)
                {
                    if (auto it = info.find(tag); it != info.end())
                    {
                        auto& entry = it->second;
                        if (!entry.data.has_value())
                        {
                            entry.data = initializer(tag);
                            entry.input->set_value(entry.data.value());
                            gate->commit();
                        }
                        return getter(entry.data.value());
                    }
                    return V();
                },
                [this, setter = std::move(setter)](std::string_view tag, V new_data)
                {
                    if (auto it = info.find(tag); it != info.end())
                    {
                        auto& entry = it->second;
                        setter(entry.data.value(), std::move(new_data));
                        entry.input->set_value(entry.data.value());
                        gate->commit();
                    }
                }
            );
        }
    };
}
