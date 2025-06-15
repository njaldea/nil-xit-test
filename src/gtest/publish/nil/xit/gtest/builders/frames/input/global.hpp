#pragma once

#include "../../../headless/Inputs.hpp"
#include "../../../utils/from_member.hpp"
#include "../../../utils/from_self.hpp"
#include "Frame.hpp"

#include <nil/xit/test/App.hpp>
#include <nil/xit/test/frame/input/Global.hpp>

#include <type_traits>

namespace nil::xit::gtest::builders::input::global
{
    using nil::xit::test::frame::input::global::Info;

    template <typename T>
    class Frame final: public input::Frame
    {
    public:
        using type = T;

        Frame(
            std::string init_id,
            std::optional<std::string> init_path,
            std::function<std::unique_ptr<typename Info<T>::IDataManager>()> init_loader_creator
        )
            : id(std::move(init_id))
            , path(std::move(init_path))
            , loader_creator(std::move(init_loader_creator))
        {
        }

        void install(test::App& app) override
        {
            auto* frame = path.has_value()
                ? app.add_global_input<T>(id, path.value(), loader_creator())
                : app.add_global_input<T>(id, loader_creator());
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

        template <test::frame::input::is_valid_value_accessor<T&> Accessor>
        Frame<T>& value(std::string value_id, Accessor accessor)
        {
            using accessor_return_t = std::remove_cvref_t<decltype(accessor(std::declval<T&>()))>;
            values.emplace_back(                                                 //
                [value_id = std::move(value_id), accessor = std::move(accessor)] //
                (Info<T> & info) { info.template add_value<accessor_return_t>(value_id, accessor); }
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

    private:
        std::string id;
        std::optional<std::string> path;
        std::function<std::unique_ptr<typename Info<T>::IDataManager>()> loader_creator;
        std::vector<std::function<void(Info<T>&)>> values;
        std::vector<std::function<void(Info<T>&)>> signals;
    };
}
