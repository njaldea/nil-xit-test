#pragma once

#include "frames/main/Frame.hpp"

#include <nil/xit/test/App.hpp>

#include <utility>

namespace nil::xit::gtest::builders
{
    class MainBuilder final
    {
    public:
        main::Frame& create_main(FileInfo file_info)
        {
            auto f = std::make_unique<main::MainFrame>(std::move(file_info));
            auto* ptr = f.get();
            frame = std::move(f);
            return *ptr;
        }

        void install(test::App& app) const;

    private:
        std::unique_ptr<IFrame> frame;
    };
}
