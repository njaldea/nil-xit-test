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
            std::function<std::unique_ptr<typename Info<T>::IDataManager>()> init_loader_creator
        )
            : id(std::move(init_id))
            , file(std::move(init_file))
            , loader_creator(std::move(init_loader_creator))
        {
        }

        void install(test::App& app, const std::filesystem::path& path) override
        {
            auto* frame = app.add_unique_input<T>(id, path / file, loader_creator());
            for (const auto& value_installer : values)
            {
                value_installer(*frame);
            }
            for (const auto& signal_installer : signals)
            {
                signal_installer(*frame);
            }
        }

        void install(headless::Inputs& inputs) override
        {
            using IDataManager = Info<T>::IDataManager;

            struct Cache: headless::Cache<T>
            {
                explicit Cache(std::unique_ptr<IDataManager> init_manager)
                    : manager(std::move(init_manager))
                {
                }

                T get(std::string_view /* tag */) const override
                {
                    return manager->initialize();
                }

                std::unique_ptr<IDataManager> manager;
            };

            inputs.values.emplace(id, std::make_unique<Cache>(loader_creator()));
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
            values.emplace_back( //
                [value_id = std::move(value_id), accessor = std::move(accessor)](Info<T>& info)
                { info.template add_value<getter_return_t>(value_id, accessor); }
            );
            return *this;
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

        template <typename Callable>
            requires requires(Callable callable) {
                { callable(std::declval<T>()) };
            }
        Frame<T>& signal(std::string signal_id, Callable callable)
        {
            signals.push_back(
                [signal_id = std::move(signal_id), callable = std::move(callable)](Info<T>& info)
                { info.add_signal(signal_id, callable); } //
            );
            return *this;
        }

    private:
        std::string id;
        std::filesystem::path file;
        std::function<std::unique_ptr<typename Info<T>::IDataManager>()> loader_creator;
        std::vector<std::function<void(Info<T>&)>> values;
        std::vector<std::function<void(Info<T>&)>> signals;
    };
}
