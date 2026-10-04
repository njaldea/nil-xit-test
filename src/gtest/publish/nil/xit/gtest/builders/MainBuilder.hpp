// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

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
