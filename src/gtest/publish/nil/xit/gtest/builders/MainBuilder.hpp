#pragma once

#include "frames/main/Frame.hpp"

#include <nil/xit/test/App.hpp>

namespace nil::xit::gtest::builders
{
    class MainBuilder final
    {
    public:
        main::Frame& create_main(FileInfo file_info);
        void install(test::App& app) const;

    private:
        std::unique_ptr<IFrame> frame;
    };
}
