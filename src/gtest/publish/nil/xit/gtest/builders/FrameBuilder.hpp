#pragma once

#include "frames/IFrame.hpp"
#include "frames/input/tagged.hpp"
#include "frames/input/unique.hpp"
#include "frames/output/Frame.hpp"

#include "../utils/from_data.hpp"

#include <nil/xalt/type_id.hpp>

#include <filesystem>
#include <string_view>
#include <type_traits>
#include <vector>

namespace nil::xit::gtest::builders
{
    template <typename T>
    concept is_loader_unique = requires() {
        { T::initialize() };
    } || requires(T maker) {
        { maker().initialize() };
    };

    template <typename T>
    concept is_loader_tagged = requires() {
        { T::initialize(std::declval<std::string_view>()) };
    } || requires(T maker) {
        { maker().initialize(std::declval<std::string_view>()) };
    };

    template <typename T, typename... Args>
    concept has_update = requires() {
        { T::update(std::declval<Args>()...) };
    } || requires(T loader) {
        { loader.update(std::declval<Args>()...) };
    };

    template <typename T, typename... Args>
    concept has_finalize = requires() {
        { T::finalize(std::declval<Args>()...) };
    } || requires(T loader) {
        { loader.finalize(std::declval<Args>()...) };
    };

    class FrameBuilder final
    {
    public:
        template <typename Loader>
            requires(!is_loader_tagged<Loader>)
        auto& create_tagged_input(
            std::string id,
            std::optional<std::filesystem::path> file,
            Loader loader
        )
        {
            return create_tagged_input(
                std::move(id),
                std::move(file),
                [loader = std::move(loader)]() { return from_data(loader()); }
            );
        }

        template <is_loader_tagged Loader>
        auto& create_tagged_input(
            std::string id,
            std::optional<std::filesystem::path> file,
            Loader loader
        )
        {
            using loader_t = std::remove_cvref_t<decltype(loader())>;
            using type = std::remove_cvref_t<decltype(loader().initialize(std::string_view()))>;
            using IDataManager = xit::test::frame::input::tagged::Info<type>::IDataManager;

            struct DataManager: IDataManager
            {
                explicit DataManager(loader_t init_loader)
                    : loader(std::move(init_loader))
                {
                }

                type initialize(std::string_view tag) const override
                {
                    return loader.initialize(tag);
                }

                void update(std::string_view tag, const type& value) const override
                {
                    if constexpr (has_update<loader_t, std::string_view, type>)
                    {
                        loader.update(tag, value);
                    }
                }

                void finalize(std::string_view tag, const type& value) const override
                {
                    if constexpr (has_finalize<loader_t, std::string_view, type>)
                    {
                        loader.finalize(tag, value);
                    }
                }

                loader_t loader;
            };

            return static_cast<input::tagged::Frame<type>&>(
                *input_frames.emplace_back(std::make_unique<input::tagged::Frame<type>>(
                    std::move(id),
                    std::move(file),
                    [loader = std::move(loader)]() -> std::unique_ptr<IDataManager>
                    { return std::make_unique<DataManager>(loader()); }
                ))
            );
        }

        template <typename Loader>
            requires(!is_loader_unique<Loader>)
        auto& create_unique_input(
            std::string id,
            std::optional<std::filesystem::path> file,
            Loader loader
        )
        {
            return create_unique_input(
                std::move(id),
                std::move(file),
                [loader = std::move(loader)]() { return from_data(loader()); }
            );
        }

        template <is_loader_unique Loader>
        auto& create_unique_input(
            std::string id,
            std::optional<std::filesystem::path> file,
            Loader loader
        )
        {
            using loader_t = std::remove_cvref_t<decltype(loader())>;
            using type = std::remove_cvref_t<decltype(loader().initialize())>;
            using IDataManager = xit::test::frame::input::unique::Info<type>::IDataManager;

            struct DataManager: IDataManager
            {
                explicit DataManager(loader_t init_loader)
                    : loader(std::move(init_loader))
                {
                }

                type initialize() const override
                {
                    return loader.initialize();
                }

                void update(const type& value) const override
                {
                    if constexpr (has_update<loader_t, type>)
                    {
                        loader.update(value);
                    }
                }

                void finalize(const type& value) const override
                {
                    if constexpr (has_finalize<loader_t, std::string_view, type>)
                    {
                        loader.finalize(value);
                    }
                }

                loader_t loader;
            };

            return static_cast<input::unique::Frame<type>&>(
                *input_frames.emplace_back(std::make_unique<input::unique::Frame<type>>(
                    std::move(id),
                    std::move(file),
                    [loader = std::move(loader)]() -> std::unique_ptr<IDataManager>
                    { return std::make_unique<DataManager>(loader()); }
                ))
            );
        }

        template <typename T>
        auto& create_output(std::string id, std::optional<std::filesystem::path> file)
        {
            using type = std::remove_cvref_t<T>;
            return static_cast<output::Frame<type>&>(*output_frames.emplace_back(
                std::make_unique<output::Frame<type>>(std::move(id), std::move(file))
            ));
        }

        void install(test::App& app, const std::filesystem::path& path) const;
        void install(headless::Inputs& inputs) const;

    private:
        std::vector<std::unique_ptr<input::Frame>> input_frames;
        std::vector<std::unique_ptr<IFrame>> output_frames;
    };
}
