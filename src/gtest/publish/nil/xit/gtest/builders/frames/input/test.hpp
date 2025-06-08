#pragma once

#include "../../../headless/Inputs.hpp"
#include "../../../utils/from_member.hpp"
#include "../../../utils/from_self.hpp"
#include "Frame.hpp"

#include <nil/xit/test/App.hpp>
#include <nil/xit/test/frame/input/Test.hpp>

#include <type_traits>

namespace nil::xit::gtest::builders::input::test
{
    template <typename T>
    class Frame final: public input::Frame
    {
    public:
        using type = T;

        Frame(
            std::string init_id,
            std::optional<FileInfo> init_file_info,
            std::function<
                std::unique_ptr<typename xit::test::frame::input::test::Info<T>::IDataManager>()>
                init_loader_creator
        )
            : id(std::move(init_id))
            , file_info(std::move(init_file_info))
            , loader_creator(std::move(init_loader_creator))
        {
        }

        void install(xit::test::App& app) override
        {
            auto* frame = file_info.has_value()
                ? app.add_test_input<T>(id, file_info.value(), loader_creator())
                : app.add_test_input<T>(id, loader_creator());
            for (const auto& installer : bindings)
            {
                installer(*frame);
            }
        }

        void install(headless::Inputs& inputs) override
        {
            using IDataManager = xit::test::frame::input::test::Info<T>::IDataManager;

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

        template <xit::test::frame::input::is_valid_value_accessor<T&> Accessor>
        Frame<T>& value(std::string value_id, Accessor accessor)
        {
            using accessor_return_t = std::remove_cvref_t<decltype(accessor(std::declval<T&>()))>;
            bindings.emplace_back(                                               //
                [value_id = std::move(value_id), accessor = std::move(accessor)] //
                (xit::test::frame::input::test::Info<T> & info)
                { info.template add_value<accessor_return_t>(value_id, accessor); } //
            );
            return *this;
        }

        template <std::same_as<T> Z, typename U>
        Frame<T>& value(std::string value_id, U Z::*member)
        {
            return value(std::move(value_id), from_member<T, U>(member));
        }

        Frame<T>& value(std::string value_id)
        {
            return value(std::move(value_id), from_self<T>());
        }

    private:
        std::string id;
        std::optional<FileInfo> file_info;

        std::function<
            std::unique_ptr<typename xit::test::frame::input::test::Info<T>::IDataManager>()>
            loader_creator;
        std::vector<std::function<void(xit::test::frame::input::test::Info<T>&)>> bindings;
    };
}
