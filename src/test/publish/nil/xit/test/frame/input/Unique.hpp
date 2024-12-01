#pragma once

#include "Info.hpp"

#include <nil/xit/unique/add_signal.hpp>
#include <nil/xit/unique/add_value.hpp>
#include <nil/xit/unique/structs.hpp>

#include <nil/gate/Core.hpp>

#include <type_traits>

namespace nil::xit::test::frame::input::unique
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

            virtual T initialize() const = 0;
            virtual void update(const T& value) const = 0;
            virtual void finalize(const T& value) const = 0;
        };

        nil::xit::unique::Frame* frame = nullptr;
        nil::gate::Core* gate = nullptr;
        std::unique_ptr<IDataManager> manager;

        struct
        {
            std::optional<T> data;
            nil::gate::edges::Mutable<T>* input = nullptr;
        } info;

        nil::gate::edges::Compatible<T> get_input(std::string_view /* tag */) override
        {
            if (info.input == nullptr)
            {
                info.input = gate->edge<T>();
            }
            return info.input;
        }

        void finalize(std::string_view /* tag */) const override
        {
            if (info.data.has_value())
            {
                manager->finalize(info.data.value());
            }
        }

        template <typename V, is_valid_value_getter<T&> Accessor>
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
                        parent->info.data = parent->manager->initialize();
                        parent->info.input->set_value(parent->info.data.value());
                        parent->gate->commit();
                    }
                    return accessor(parent->info.data.value());
                }

                void set(V new_data) override
                {
                    accessor(parent->info.data.value()) = std::move(new_data);
                    parent->info.input->set_value(parent->info.data.value());
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

        template <typename Callable>
            requires std::is_invocable_v<Callable, T>
        void add_signal(std::string id, Callable callable)
        {
            nil::xit::unique::add_signal(
                *frame,
                std::move(id),
                [this, callable = std::move(callable)]()
                {
                    if (info.data.has_value())
                    {
                        callable(info.data.value());
                    }
                }
            );
        }
    };
}
