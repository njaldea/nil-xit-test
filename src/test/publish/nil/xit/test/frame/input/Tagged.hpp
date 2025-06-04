#pragma once

#include "Info.hpp"

#include <nil/xalt/transparent_stl.hpp>

#include <nil/xit/tagged/add_signal.hpp>
#include <nil/xit/tagged/add_value.hpp>
#include <nil/xit/tagged/structs.hpp>

#include <nil/gate/Core.hpp>

namespace nil::xit::test::frame::input::tagged
{
    template <typename T>
    struct Info final: input::Info<T>
    {
        struct IDataManager
        {
            IDataManager() = default;
            virtual ~IDataManager() = default;
            IDataManager(IDataManager&&) = delete;
            IDataManager(const IDataManager&) = delete;
            IDataManager& operator=(IDataManager&&) = delete;
            IDataManager& operator=(const IDataManager&) = delete;

            virtual T initialize(std::string_view tag) const = 0;
            virtual void update(std::string_view tag, const T& value) const = 0;
            virtual void finalize(std::string_view tag, const T& value) const = 0;
        };

        struct Entry
        {
            // data and input has duplicate data to ease thread safety
            std::optional<T> data;
            nil::gate::ports::Mutable<T>* input = nullptr;
        };

        nil::xit::tagged::Frame* frame = nullptr;
        nil::gate::Core* gate = nullptr;
        std::unique_ptr<IDataManager> manager;
        xalt::transparent_umap<Entry> info;

        nil::gate::ports::Compatible<T> get_input(std::string_view tag) override
        {
            if (const auto it = info.find(tag); it != info.end())
            {
                return it->second.input;
            }
            return info.emplace(tag, typename Info<T>::Entry{std::nullopt, gate->port<T>()})
                .first->second.input;
        }

        void initialize(std::string_view tag) override
        {
            if (auto it = info.find(tag); it != info.end())
            {
                if (auto& entry = it->second; !entry.data.has_value())
                {
                    entry.data = manager->initialize(tag);
                    if (entry.input != nullptr)
                    {
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
    };
}
