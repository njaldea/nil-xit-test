// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

#pragma once

namespace nil::xit::test
{
    class App;
}

namespace nil::xit::gtest::builders
{
    struct IFrame
    {
        virtual ~IFrame() = default;
        IFrame() = default;
        IFrame(IFrame&&) = delete;
        IFrame(const IFrame&) = delete;
        IFrame& operator=(IFrame&&) = delete;
        IFrame& operator=(const IFrame&) = delete;
        virtual void install(test::App& app) = 0;
    };
}
