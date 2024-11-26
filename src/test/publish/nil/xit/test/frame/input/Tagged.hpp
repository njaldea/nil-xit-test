#pragma once

#include "Info.hpp"

#include "../../transparent_hash_map.hpp"

#include <nil/xit/tagged/add_signal.hpp>
#include <nil/xit/tagged/add_value.hpp>
#include <nil/xit/tagged/structs.hpp>

#include <nil/gate/Core.hpp>

namespace nil::xit::test::frame::input::tagged
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

            virtual T load(std::string_view tag) const = 0;
            virtual void update(std::string_view tag, const T& value) const = 0;
        };

        struct Entry
        {
            std::optional<T> data;
            nil::gate::edges::Mutable<T>* input = nullptr;
        };

        nil::xit::tagged::Frame* frame = nullptr;
        nil::gate::Core* gate = nullptr;
        std::unique_ptr<IDataLoader> loader;
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

        template <typename V, typename Accessor>
            requires requires(const Accessor& accessor) {
                { accessor.get(std::declval<const T&>()) } -> std::same_as<V>;
                { accessor.set(std::declval<T&>(), std::declval<V>()) } -> std::same_as<void>;
            }
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
                            entry.data = parent->loader->load(tag);
                            entry.input->set_value(entry.data.value());
                            parent->gate->commit();
                        }
                        return accessor.get(entry.data.value());
                    }
                    return V();
                }

                void set(std::string_view tag, V new_data) const override
                {
                    if (auto it = parent->info.find(tag); it != parent->info.end())
                    {
                        auto& entry = it->second;
                        accessor.set(entry.data.value(), std::move(new_data));
                        entry.input->set_value(entry.data.value());
                        parent->loader->update(tag, entry.data.value());
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

            this->add_value<V>(std::move(id), Accessor(std::move(getter), std::move(setter)));
        }

        template <typename Callable>
            requires requires(Callable callable) {
                { callable(std::declval<T>(), std::string_view()) };
            }
        void add_signal(std::string id, Callable callable)
        {
            nil::xit::tagged::add_signal(
                *frame,
                std::move(id),
                [this, callable = std::move(callable)](std::string_view tag)
                {
                    if (const auto it = info.find(tag); it != info.end())
                    {
                        auto& data = it->second.data;
                        if (data.has_value())
                        {
                            callable(data.value(), tag);
                        }
                    }
                }
            );
        }
    };
}
