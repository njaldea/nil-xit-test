#pragma once

#include "../../../headless/CacheManager.hpp"
#include "../../../utils/from_data.hpp"
#include "../../../utils/from_member.hpp"
#include "../../../utils/from_self.hpp"
#include "Frame.hpp"

#include <nil/xit/buffer_type.hpp>
#include <nil/xit/test/App.hpp>
#include <nil/xit/test/frame/input/Global.hpp>
#include <nil/xit/unique/add_option.hpp>

#include <type_traits>

namespace nil::xit::gtest::builders::input::global
{
    template <typename T>
    class Frame
    {
    public:
        using type = T;

        template <typename Accessor>
            requires(test::frame::input::is_valid_value_accessor<Accessor, T&>)
        Frame<T>& value(std::string id, Accessor accessor)
        {
            using accessor_return_t = std::remove_cvref_t<decltype(accessor(std::declval<T&>()))>;
            values.emplace_back(                                     //
                [id = std::move(id), accessor = std::move(accessor)] //
                (nil::xit::test::frame::input::global::Info<T> & info)
                { info.template add_value<accessor_return_t>(id, accessor); }
            );
            return *this;
        }

        template <typename Z, typename U>
            requires(std::is_same_v<T, Z>)
        Frame<T>& value(std::string id, U Z::*member)
        {
            return value(std::move(id), from_member(member));
        }

        Frame<T>& value(std::string id)
        {
            return value(std::move(id), from_self<T>());
        }

        template <typename Z>
            requires(!std::is_same_v<T, Z> && !test::frame::input::is_valid_value_accessor<Z, T&>)
        Frame<T>& value(std::string id, Z unrelated_value)
        {
            return value(std::move(id), from_data<Z>(std::move(unrelated_value)));
        }

        Frame<T>& option(std::string key, std::string value)
        {
            options.emplace_back(std::move(key), std::move(value));
        }

    protected:
        // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
        std::vector<std::function<void(nil::xit::test::frame::input::global::Info<T>&)>> values;
        // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
        std::vector<std::tuple<std::string, std::string>> options;
    };

    template <typename T>
    class InputFrame final
        : public Frame<T>
        , public IFrame
    {
    public:
        using loader_creator_t
            = std::function<std::unique_ptr<xit::test::frame::IDataManager<T>>()>;

        InputFrame(
            std::string init_id,
            std::optional<FileInfo> init_file_info,
            loader_creator_t init_loader_creator
        )
            : id(std::move(init_id))
            , file_info(std::move(init_file_info))
            , loader_creator(std::move(init_loader_creator))
        {
        }

        void install(test::App& app) override
        {
            auto* frame = file_info.has_value() //
                ? app.add_global_input<T>(loader_creator(), id, file_info.value())
                : app.add_global_input<T>(loader_creator(), id);
            if (this->values.empty())
            {
                if constexpr (nil::xit::detail::has_deserialize<T>
                              && nil::xit::detail::has_serialize<T>)
                {
                    frame->template add_value<T>("value", from_self<T>());
                }
            }
            else
            {
                for (const auto& installer : this->values)
                {
                    installer(*frame);
                }
            }

            add_signal(*frame->frame, "finalize", [frame]() { frame->finalize({}); });

            for (const auto& [key, value] : this->options)
            {
                add_option(*frame->frame, key, value);
            }
        }

        void install(headless::CacheManager& cache_manager) override
        {
            struct Cache: headless::Cache<T>
            {
                explicit Cache(std::unique_ptr<test::frame::IDataManager<T>> init_manager)
                    : manager(std::move(init_manager))
                {
                }

                T get(std::string_view /* tag */) const override
                {
                    return manager->initialize();
                }

                std::unique_ptr<test::frame::IDataManager<T>> manager;
            };

            cache_manager.values.emplace(id, std::make_unique<Cache>(loader_creator()));
        }

    private:
        std::string id;
        std::optional<FileInfo> file_info;
        loader_creator_t loader_creator;
    };
}
