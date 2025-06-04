#pragma once

#include "frames/main/Frame.hpp"

#include <nil/xit/test/App.hpp>

#include <nil/xalt/transparent_stl.hpp>

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace nil::xit::gtest::builders
{
    class MainBuilder final
    {
    public:
        template <typename FromVS>
            requires std::is_invocable_v<FromVS, std::vector<std::string>>
        void create_main(FileInfo file_info, FromVS converter)
        {
            frame = std::make_unique<main::Frame<FromVS>>( //
                std::move(file_info),
                std::move(converter)
            );
        }

        void install(test::App& app) const;

    private:
        std::unique_ptr<IFrame> frame;
    };
}
