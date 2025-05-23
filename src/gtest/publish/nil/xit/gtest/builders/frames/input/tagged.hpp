#pragma once

#include "../../../headless/Inputs.hpp"
#include "../../../utils/from_member.hpp"
#include "../../../utils/from_self.hpp"
#include "Frame.hpp"

#include <nil/xit/test/App.hpp>
#include <nil/xit/test/frame/input/Tagged.hpp>

#include <type_traits>

namespace nil::xit::gtest::builders::input::tagged
{
    template <typename T>
    class Frame final: public input::Frame
    {
    public:
        using type = T;

        Frame(
            std::string init_id,
            std::optional<std::filesystem::path> init_file,
            std::function<
                std::unique_ptr<typename test::frame::input::tagged::Info<T>::IDataManager>()>
                init_loader_creator
        )
            : id(std::move(init_id))
            , file(std::move(init_file))
            , loader_creator(std::move(init_loader_creator))
        {
        }

        void install(test::App& app, const std::filesystem::path& path) override
        {
            auto* frame = file.has_value()
                ? app.add_tagged_input<T>(id, path / file.value(), loader_creator())
                : app.add_tagged_input<T>(id, {}, loader_creator());
            for (const auto& installer : bindings)
            {
                installer(*frame);
            }
        }

        void install(headless::Inputs& inputs) override
        {
            using IDataManager = test::frame::input::tagged::Info<T>::IDataManager;

            struct Cache: headless::Cache<T>
            {
                explicit Cache(std::unique_ptr<IDataManager> init_manager)
                    : manager(std::move(init_manager))
                {
                }

                T get(std::string_view tag) const
                {
                    return manager->initialize(tag);
                }

                std::unique_ptr<IDataManager> manager;
            };

            inputs.values.emplace(id, std::make_unique<Cache>(loader_creator()));
        }

        template <test::frame::input::is_valid_value_accessor<T&> Accessor>
        Frame<T>& value(std::string value_id, Accessor accessor)
        {
            using accessor_return_t = std::remove_cvref_t<decltype(accessor(std::declval<T&>()))>;
            bindings.emplace_back(                                               //
                [value_id = std::move(value_id), accessor = std::move(accessor)] //
                (test::frame::input::tagged::Info<T> & info)
                { info.template add_value<accessor_return_t>(value_id, accessor); } //
            );
            return *this;
        }

        template <typename U>
        Frame<T>& value(std::string value_id, U T::*member)
        {
            return value(std::move(value_id), from_member(member));
        }

        Frame<T>& value(std::string value_id)
        {
            return value(std::move(value_id), from_self<T>());
        }

        template <typename Callable>
            requires std::is_invocable_v<Callable, T, std::string_view>
        Frame<T>& signal(std::string signal_id, Callable callable)
        {
            bindings.push_back( //
                [signal_id = std::move(signal_id),
                 callable = std::move(callable)](test::frame::input::tagged::Info<T>& info)
                { info.add_signal(signal_id, callable); } //
            );
            return *this;
        }

    private:
        std::string id;
        std::optional<std::filesystem::path> file;

        std::function<std::unique_ptr<typename test::frame::input::tagged::Info<T>::IDataManager>()>
            loader_creator;
        std::vector<std::function<void(test::frame::input::tagged::Info<T>&)>> bindings;
    };
}
