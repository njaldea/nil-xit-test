#pragma once

#include "../type.hpp"
#include "frames/IFrame.hpp"
#include "frames/input/tagged.hpp"
#include "frames/input/unique.hpp"
#include "frames/output/Frame.hpp"

#include "../utils/from_data.hpp"
#include "nil/xit/test/frame/input/Tagged.hpp"

#include <filesystem>
#include <string_view>
#include <type_traits>
#include <vector>

namespace nil::xit::gtest::builders
{
    template <typename T>
    concept is_loader_unique = requires(T loader) {
        { loader.load() };
    };

    template <typename T>
    concept is_loader_tagged = requires(T loader) {
        { loader.load(std::declval<std::string_view>()) };
    };

    template <typename T, typename... Args>
    concept has_update = requires(T loader) {
        { loader.update(std::declval<Args>()...) };
    };

    class FrameBuilder final
    {
    public:
        template <typename Data>
            requires(!is_loader_tagged<decltype(std::declval<Data>()())>)
        auto& create_tagged_input(std::string id, std::filesystem::path file, Data data)
        {
            return create_tagged_input(
                std::move(id),
                std::move(file),
                [data = std::move(data)]() { return from_data(data()); }
            );
        }

        template <typename Loader>
            requires(is_loader_tagged<decltype(std::declval<Loader>()())>)
        auto& create_tagged_input(std::string id, std::filesystem::path file, Loader loader)
        {
            using loader_t = std::remove_cvref_t<decltype(loader())>;
            using type = std::remove_cvref_t<decltype(loader().load(std::string_view()))>;
            using IDataLoader = xit::test::frame::input::tagged::Info<type>::IDataLoader;

            struct DataLoader: IDataLoader
            {
                explicit DataLoader(loader_t init_loader)
                    : loader(std::move(init_loader))
                {
                }

                type load(std::string_view tag) const override
                {
                    return loader.load(tag);
                }

                void update(std::string_view tag, const type& value) const override
                {
                    if constexpr (has_update<loader_t, std::string_view, type>)
                    {
                        loader.update(tag, value);
                    }
                }

                loader_t loader;
            };

            return static_cast<input::tagged::Frame<type>&>(
                *input_frames.emplace_back(std::make_unique<input::tagged::Frame<type>>(
                    std::move(id),
                    std::move(file),
                    [loader = std::move(loader)]() -> std::unique_ptr<IDataLoader>
                    { return std::make_unique<DataLoader>(loader()); }
                ))
            );
        }

        template <typename Data>
            requires(!is_loader_unique<decltype(std::declval<Data>()())>)
        auto& create_unique_input(std::string id, std::filesystem::path file, Data data)
        {
            return create_unique_input(
                std::move(id),
                std::move(file),
                [data = std::move(data)]() { return from_data(data()); }
            );
        }

        template <typename Loader>
            requires(is_loader_unique<decltype(std::declval<Loader>()())>)
        auto& create_unique_input(std::string id, std::filesystem::path file, Loader loader)
        {
            using loader_t = std::remove_cvref_t<decltype(loader())>;
            using type = std::remove_cvref_t<decltype(loader().load())>;
            using IDataLoader = xit::test::frame::input::unique::Info<type>::IDataLoader;

            struct DataLoader: IDataLoader
            {
                explicit DataLoader(loader_t init_loader)
                    : loader(std::move(init_loader))
                {
                }

                type load() const override
                {
                    return loader.load();
                }

                void update(const type& value) const override
                {
                    if constexpr (has_update<loader_t, type>)
                    {
                        loader.update(value);
                    }
                }

                loader_t loader;
            };

            return static_cast<input::unique::Frame<type>&>(
                *input_frames.emplace_back(std::make_unique<input::unique::Frame<type>>(
                    std::move(id),
                    std::move(file),
                    [loader = std::move(loader)]() -> std::unique_ptr<IDataLoader>
                    { return std::make_unique<DataLoader>(loader()); }
                ))
            );
        }

        template <typename T>
        auto& create_output(std::string id, std::filesystem::path file, type<T> /* type */ = {})
        {
            return static_cast<output::Frame<T>&>(*output_frames.emplace_back(
                std::make_unique<output::Frame<T>>(std::move(id), std::move(file))
            ));
        }

        void install(test::App& app, const std::filesystem::path& path) const;
        void install(headless::Inputs& inputs) const;

    private:
        std::vector<std::unique_ptr<input::Frame>> input_frames;
        std::vector<std::unique_ptr<IFrame>> output_frames;
    };
}
