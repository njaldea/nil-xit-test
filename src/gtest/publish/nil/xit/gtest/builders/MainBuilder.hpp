#pragma once

#include "frames/main/Frame.hpp"

#include <nil/xit/test/App.hpp>

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace nil::xit::gtest::builders
{
    class MainBuilder final
    {
    public:
        template <typename FromVS>
            requires requires(FromVS converter) {
                { converter(std::declval<std::vector<std::string>>()) };
            }
        void create_main(std::filesystem::path file, FromVS converter)
        {
            frame = std::make_unique<main::Frame<FromVS>>(
                std::move(file),
                std::move(std::move(converter))
            );
        }

        void install(test::App& app, const std::filesystem::path& path) const;

    private:
        std::unique_ptr<IFrame> frame;
    };
}
