#pragma once

#include "../type.hpp"
#include "frames/IFrame.hpp"
#include "frames/input/tagged.hpp"
#include "frames/input/unique.hpp"
#include "frames/output/Frame.hpp"

#include <filesystem>
#include <string_view>
#include <vector>

namespace nil::xit::gtest::builders
{
    template <typename T, typename... Args>
    concept is_initializer = requires(T initializer) {
        { initializer(std::declval<Args>()...) };
    };

    class FrameBuilder final
    {
    public:
        template <typename Initializer>
            requires(!is_initializer<Initializer, std::string_view>)
        auto& create_tagged_input(
            std::string id,
            std::filesystem::path file,
            Initializer initializer
        )
        {
            return create_tagged_input(
                std::move(id),
                std::move(file),
                [=](std::string_view /*tag*/) { return initializer; }
            );
        }

        template <typename Initializer>
            requires(is_initializer<Initializer, std::string_view>)
        auto& create_tagged_input(
            std::string id,
            std::filesystem::path file,
            Initializer initializer
        )
        {
            using type = decltype(initializer(std::declval<std::string_view>()));
            return static_cast<input::tagged::Frame<type>&>(
                *input_frames.emplace_back(std::make_unique<input::tagged::Frame<type>>(
                    std::move(id),
                    std::move(file),
                    std::move(initializer)
                ))
            );
        }

        template <typename Initializer>
            requires(!is_initializer<Initializer>)
        auto& create_unique_input(
            std::string id,
            std::filesystem::path file,
            Initializer initializer
        )
        {
            return create_unique_input(
                std::move(id),
                std::move(file),
                [=]() { return initializer; }
            );
        }

        template <typename Initializer>
            requires(is_initializer<Initializer>)
        auto& create_unique_input(
            std::string id,
            std::filesystem::path file,
            Initializer initializer
        )
        {
            using type = decltype(initializer());
            return static_cast<input::unique::Frame<type>&>(
                *input_frames.emplace_back(std::make_unique<input::unique::Frame<type>>(
                    std::move(id),
                    std::move(file),
                    std::move(initializer)
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
