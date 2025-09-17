#pragma once

#include "../IDataManager.hpp"
#include "Info.hpp"

#include <nil/xit/tagged/add_value.hpp>
#include <nil/xit/tagged/structs.hpp>

#include <nil/gate/Core.hpp>

#include <memory>
#include <string_view>
#include <unordered_map>

namespace nil::xit::test::frame::input::test
{
    template <typename T>
    struct Info final: input::Info<T>
    {
        using Entry = input::Info<T>::Entry;

        nil::xit::tagged::Frame* frame = nullptr;
        nil::gate::Core* gate = nullptr;
        std::unique_ptr<IDataManager<T, std::string_view>> manager;
        std::unordered_map<std::string_view, Entry> info; // sv owned by tags from App

        nil::gate::ports::Mutable<T>* get_port(std::string_view tag) override
        {
            if (const auto it = info.find(tag); it != info.end())
            {
                return it->second.input;
            }
            throw std::runtime_error("should be unreachable");
        }

        void initialize(std::string_view tag) override
        {
            if (auto it = info.find(tag); it != info.end())
            {
                if (auto& entry = it->second; !entry.data.has_value())
                {
                    if (entry.input != nullptr)
                    {
                        entry.data = manager->initialize(tag);
                        entry.input->set_value(entry.data.value());
                    }
                }
            }
        }

        void finalize(std::string_view tag) const override
        {
            if (auto it = info.find(tag); it != info.end())
            {
                auto& entry = it->second;
                if (entry.data.has_value())
                {
                    manager->finalize(tag, entry.data.value());
                }
            }
        }

        template <typename V, is_valid_value_accessor<T&> Accessor>
        void add_value(std::string id, Accessor accessor)
        {
            struct XitAccessor: nil::xit::tagged::IAccessor<V>
            {
                XitAccessor(Info* init_parent, Accessor init_accessor)
                    : parent(init_parent)
                    , accessor(std::move(init_accessor))
                {
                }

                V get(std::string_view tag) const override
                {
                    if (auto it = parent->info.find(tag); it != parent->info.end())
                    {
                        auto& entry = it->second;
                        if (!entry.data.has_value())
                        {
                            parent->initialize(tag);
                            parent->gate->commit();
                        }
                        return accessor(entry.data.value());
                    }
                    return V();
                }

                void set(std::string_view tag, V new_data) override
                {
                    if (auto it = parent->info.find(tag); it != parent->info.end())
                    {
                        auto& entry = it->second;
                        accessor(entry.data.value()) = std::move(new_data);
                        if (entry.input != nullptr)
                        {
                            entry.input->set_value(entry.data.value());
                        }
                        parent->manager->update(tag, entry.data.value());
                        parent->gate->commit();
                    }
                }

                Info* parent;
                Accessor accessor;
            };

            nil::xit::tagged::add_value(
                *frame,
                std::move(id),
                std::make_unique<XitAccessor>(this, std::move(accessor))
            );
        }

        void add_info(std::string_view tag) override
        {
            info.emplace(tag, Entry{std::nullopt, gate->port<T>()});
        }
    };
}
