#pragma once

#include <nil/xit/test/App.hpp>
#include <nil/xit/test/frame/input/Unique.hpp>

#include "../../../headless/Inputs.hpp"
#include "Frame.hpp"

namespace nil::xit::gtest::builders::input::unique
{
    using nil::xit::test::frame::input::unique::Info;

    template <typename T>
    class Frame final: public input::Frame
    {
    public:
        using type = T;

        Frame(
            std::string init_id,
            std::filesystem::path init_file,
            std::function<T()> init_initializer
        )
            : id(std::move(init_id))
            , file(std::move(init_file))
            , initializer(std::move(init_initializer))
        {
        }

        void install(test::App& app, const std::filesystem::path& path) override
        {
            auto* frame = app.add_unique_input(id, path / file, initializer);
            for (const auto& value_installer : values)
            {
                value_installer(*frame);
            }
        }

        void install(headless::Inputs& inputs) override
        {
            struct Cache: headless::Cache<T>
            {
                explicit Cache(std::function<T()> init_initializer)
                    : initializer(std::move(init_initializer))
                {
                }

                T get(std::string_view /* tag */) const override
                {
                    return initializer();
                }

                std::function<T()> initializer;
            };

            inputs.values.emplace(id, std::make_unique<Cache>(initializer));
        }

        template <typename Getter, typename Setter>
            requires(is_compatible_getter_setter<Getter, Setter, T>)
        Frame<T>& value(std::string value_id, Getter getter, Setter setter)
        {
            using getter_return_t = decltype(getter(std::declval<const T&>()));
            values.emplace_back(
                [value_id = std::move(value_id),
                 getter = std::move(getter),
                 setter = std::move(setter)](Info<T>& info) {
                    info.template add_value<getter_return_t>(
                        value_id,
                        std::move(getter),
                        std::move(setter)
                    );
                }
            );
            return *this;
        }

        template <is_compatible_accessor<T> Accessor>
        Frame<T>& value(std::string value_id, Accessor accessor)
        {
            using getter_return_t = decltype(accessor.get(std::declval<const T&>()));
            return value(
                std::move(value_id),
                [accessor](const T& value) { return accessor.get(value); },
                [accessor](T& value, getter_return_t new_value)
                { return accessor.set(value, std::move(new_value)); }
            );
        }

        template <typename U>
        Frame<T>& value(std::string value_id, U T::*member)
        {
            return value(
                std::move(value_id),
                [member](const T& value) { return value.*member; },
                [member](T& value, U new_data) { value.*member = std::move(new_data); }
            );
        }

        Frame<T>& value(std::string value_id)
        {
            return value(
                std::move(value_id),
                [](const T& value) { return value; },
                [](T& value, T new_value) { value = std::move(new_value); }
            );
        }

    private:
        std::string id;
        std::filesystem::path file;
        std::function<T()> initializer;
        std::vector<std::function<void(Info<T>&)>> values;
    };
}
