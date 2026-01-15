#pragma once

#include "../IDataManager.hpp"
#include "Info.hpp"

#include <nil/xit/unique/add_value.hpp>
#include <nil/xit/unique/structs.hpp>

#include <nil/gate/Core.hpp>

#include <memory>

namespace nil::xit::test::frame::input::global
{
    template <typename T>
    struct Info final: input::Info<T>
    {
        using Entry = input::Info<T>::Entry;

        nil::xit::unique::Frame* frame = nullptr;
        nil::gate::Core* gate = nullptr;
        std::unique_ptr<IDataManager<T>> manager;
        Entry info;

        nil::gate::ports::External<T>* get_port(std::string_view /* tag */) override
        {
            if (info.input != nullptr)
            {
                return info.input;
            }
            throw std::runtime_error("should be unreachable");
        }

        void initialize(std::string_view /* tag */) override
        {
            if (!info.data.has_value())
            {
                info.data = manager->initialize();
                if (info.input != nullptr)
                {
                    info.input->set_value(info.data.value());
                }
            }
        }

        void finalize(std::string_view /* tag */) const override
        {
            if (info.data.has_value())
            {
                manager->finalize(info.data.value());
            }
        }

        template <typename V, is_valid_value_accessor<T&> Accessor>
        void add_value(std::string id, Accessor accessor)
        {
            struct XitAccessor: nil::xit::unique::IAccessor<V>
            {
                XitAccessor(Info* init_parent, Accessor init_accessor)
                    : parent(init_parent)
                    , accessor(std::move(init_accessor))
                {
                }

                V get() const override
                {
                    if (!parent->info.data.has_value())
                    {
                        parent->initialize({});
                        parent->gate->commit();
                    }
                    return accessor(parent->info.data.value());
                }

                void set(V new_data) override
                {
                    accessor(parent->info.data.value()) = std::move(new_data);
                    if (parent->info.input != nullptr)
                    {
                        parent->info.input->set_value(parent->info.data.value());
                    }
                    parent->manager->update(parent->info.data.value());
                    parent->gate->commit();
                }

                Info* parent;
                Accessor accessor;
            };

            nil::xit::unique::add_value(
                *frame,
                std::move(id),
                std::make_unique<XitAccessor>(this, std::move(accessor))
            );
        }

        void add_info(std::string_view tag) override
        {
            gate->post(
                [tag, this](gate::Graph& graph)
                {
                    if (info.input == nullptr)
                    {
                        info.input = graph.port<T>();
                    }
                }
            );
        }
    };
}
