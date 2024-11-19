#pragma once

#include "Info.hpp"

#include <nil/xit/unique/add_value.hpp>
#include <nil/xit/unique/structs.hpp>

#include <nil/gate/Core.hpp>

#include <functional>

namespace nil::xit::test::frame::input::unique
{
    template <typename T>
    struct Info final: input::Info<T>
    {
        nil::xit::unique::Frame* frame = nullptr;
        nil::gate::Core* gate = nullptr;
        std::function<T()> initializer;

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

        template <typename V, typename Getter, typename Setter>
            requires requires(Getter g, Setter s) {
                { g(std::declval<const T&>()) } -> std::same_as<V>;
                { s(std::declval<T&>(), std::declval<V>()) } -> std::same_as<void>;
            }
        void add_value(std::string id, Getter getter, Setter setter)
        {
            nil::xit::unique::add_value(
                *frame,
                id,
                [this, getter = std::move(getter)]()
                {
                    if (!info.data.has_value())
                    {
                        info.data = this->initializer();
                        info.input->set_value(info.data.value());
                        gate->commit();
                    }
                    return getter(info.data.value());
                },
                [this, setter = std::move(setter)](V new_data)
                {
                    setter(info.data.value(), std::move(new_data));
                    info.input->set_value(info.data.value());
                    gate->commit();
                }
            );
        }
    };
}
