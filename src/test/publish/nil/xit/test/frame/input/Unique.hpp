#pragma once

#include "Info.hpp"

#include <nil/xit/unique/add_signal.hpp>
#include <nil/xit/unique/add_value.hpp>
#include <nil/xit/unique/structs.hpp>

#include <nil/gate/Core.hpp>

namespace nil::xit::test::frame::input::unique
{
    template <typename T>
    struct Info final: input::Info<T>
    {
        struct IDataLoader
        {
            IDataLoader() = default;
            virtual ~IDataLoader() = default;
            IDataLoader(IDataLoader&&) = delete;
            IDataLoader(const IDataLoader&) = delete;
            IDataLoader& operator=(IDataLoader&&) = delete;
            IDataLoader& operator=(const IDataLoader&) = delete;

            virtual T load() const = 0;
            virtual void update(const T& value) const = 0;
        };

        nil::xit::unique::Frame* frame = nullptr;
        nil::gate::Core* gate = nullptr;
        std::unique_ptr<IDataLoader> loader;

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

        template <typename V, typename Accessor>
            requires requires(const Accessor& accessor) {
                { accessor.get(std::declval<const T&>()) } -> std::same_as<V>;
                { accessor.set(std::declval<T&>(), std::declval<V>()) } -> std::same_as<void>;
            }
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
                        parent->info.data = parent->loader->load();
                        parent->info.input->set_value(parent->info.data.value());
                        parent->gate->commit();
                    }
                    return accessor.get(parent->info.data.value());
                }

                void set(V new_data) const override
                {
                    accessor.set(parent->info.data.value(), std::move(new_data));
                    parent->info.input->set_value(parent->info.data.value());
                    parent->loader->update(parent->info.data.value());
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

        template <typename V, typename Getter, typename Setter>
            requires requires(Getter g, Setter s) {
                { g(std::declval<const T&>()) } -> std::same_as<V>;
                { s(std::declval<T&>(), std::declval<V>()) } -> std::same_as<void>;
            }
        void add_value(std::string id, Getter getter, Setter setter)
        {
            struct Accessor
            {
                Accessor(Getter init_getter, Setter init_setter)
                    : getter(std::move(init_getter))
                    , setter(std::move(init_setter))
                {
                }

                V get(const T& value) const
                {
                    return getter(value);
                }

                void set(T& value, V new_value) const
                {
                    setter(value, std::move(new_value));
                }

                Getter getter;
                Setter setter;
            };

            add_value<V>(std::move(id), Accessor(std::move(getter), std::move(setter)));
        }

        template <typename Callable>
            requires requires(Callable callable) {
                { callable(std::declval<T>()) };
            }
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
