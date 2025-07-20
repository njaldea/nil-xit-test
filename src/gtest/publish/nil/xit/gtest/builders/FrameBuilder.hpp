#pragma once

#include "frames/IFrame.hpp"
#include "frames/expect/Frame.hpp"
#include "frames/input/global.hpp"
#include "frames/input/test.hpp"
#include "frames/output/Frame.hpp"

#include <nil/xit/test/frame/IDataManager.hpp>

#include "../utils/from_data.hpp"

#include <string_view>
#include <type_traits>
#include <vector>

namespace nil::xit::gtest::builders
{
    template <typename T>
    concept is_loader_unique = requires() {
        { T::initialize() };
    } || requires(T loader) {
        { loader.initialize() };
    };

    template <typename T>
    concept is_loader_tagged = requires() {
        { T::initialize(std::declval<std::string_view>()) };
    } || requires(T loader) {
        { loader.initialize(std::declval<std::string_view>()) };
    };

    class FrameBuilder final
    {
    public:
        template <typename Loader>
            requires(!is_loader_tagged<decltype(std::declval<Loader>()())>)
        auto& create_test_input(std::string id, std::optional<std::string> path, Loader loader)
        {
            return create_test_input(
                std::move(id),
                std::move(path),
                [loader = std::move(loader)]() { return from_data(loader()); }
            );
        }

        template <typename Loader>
            requires(is_loader_tagged<decltype(std::declval<Loader>()())>)
        auto& create_test_input(std::string id, std::optional<std::string> path, Loader loader)
        {
            using loader_t = std::remove_cvref_t<decltype(loader())>;
            using type = std::remove_cvref_t<decltype(loader().initialize(std::string_view()))>;

            return static_cast<input::test::Frame<type>&>(
                *input_frames.emplace_back(std::make_unique<input::test::InputFrame<type>>(
                    std::move(id),
                    std::move(path),
                    [loader = std::move(loader)]()
                    {
                        using test::frame::make_data_manager;
                        return make_data_manager<loader_t, type, std::string_view>(loader());
                    }
                ))
            );
        }

        template <typename Loader>
            requires(!is_loader_unique<decltype(std::declval<Loader>()())>)
        auto& create_global_input(std::string id, std::optional<std::string> path, Loader loader)
        {
            return create_global_input(
                std::move(id),
                std::move(path),
                [loader = std::move(loader)]() { return from_data(loader()); }
            );
        }

        template <typename Loader>
            requires(is_loader_unique<decltype(std::declval<Loader>()())>)
        auto& create_global_input(std::string id, std::optional<std::string> path, Loader loader)
        {
            using loader_t = std::remove_cvref_t<decltype(loader())>;
            using type = std::remove_cvref_t<decltype(loader().initialize())>;

            return static_cast<input::global::Frame<type>&>(
                *input_frames.emplace_back(std::make_unique<input::global::InputFrame<type>>(
                    std::move(id),
                    std::move(path),
                    [loader = std::move(loader)]()
                    {
                        using test::frame::make_data_manager;
                        return make_data_manager<loader_t, type>(loader());
                    }
                ))
            );
        }

        template <typename Loader>
            requires(!is_loader_tagged<decltype(std::declval<Loader>()())>)
        auto& create_expect(std::string id, std::optional<std::string> path, Loader loader)
        {
            return create_expect(
                std::move(id),
                std::move(path),
                [loader = std::move(loader)]() { return from_data(loader()); }
            );
        }

        template <typename Loader>
            requires(is_loader_tagged<decltype(std::declval<Loader>()())>)
        auto& create_expect(std::string id, std::optional<std::string> path, Loader loader)
        {
            using loader_t = std::remove_cvref_t<decltype(loader())>;
            using type = std::remove_cvref_t<decltype(loader().initialize(std::string_view()))>;

            return static_cast<expect::Frame<type>&>(
                *expect_frames.emplace_back(std::make_unique<expect::ExpectFrame<type>>(
                    std::move(id),
                    std::move(path),
                    [loader = std::move(loader)]()
                    {
                        using test::frame::make_data_manager;
                        return make_data_manager<loader_t, type, std::string_view>(loader());
                    }
                ))
            );
        }

        template <typename T>
        auto& create_output(std::string id, std::optional<std::string> path)
        {
            using type = std::remove_cvref_t<T>;
            return static_cast<output::Frame<type>&>(*output_frames.emplace_back(
                std::make_unique<output::OutputFrame<type>>(std::move(id), std::move(path))
            ));
        }

        void install(test::App& app) const;
        void install(headless::CacheManager& cache_manager) const;

    private:
        std::vector<std::unique_ptr<input::IFrame>> input_frames;
        std::vector<std::unique_ptr<expect::IFrame>> expect_frames;
        std::vector<std::unique_ptr<IFrame>> output_frames;
    };
}
