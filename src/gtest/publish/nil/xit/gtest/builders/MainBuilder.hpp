#pragma once

#include "frames/main/Frame.hpp"

#include <nil/xit/test/App.hpp>

#include <string>
#include <utility>

namespace nil::xit::gtest::builders
{
    class MainBuilder final
    {
    public:
        void create_main(std::string path)
        {
            frame = std::make_unique<main::Frame>(std::move(path));
        }

        void install(test::App& app) const;

    private:
        std::unique_ptr<IFrame> frame;
    };
}
