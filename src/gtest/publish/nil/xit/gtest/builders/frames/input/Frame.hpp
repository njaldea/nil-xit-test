// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

#pragma once

#include "../IFrame.hpp"

namespace nil::xit::gtest::headless
{
    struct CacheManager;
}

namespace nil::xit::gtest::builders::input
{
    struct IFrame: builders::IFrame
    {
        using builders::IFrame::install;
        virtual void install(headless::CacheManager& cache_manager) = 0;
    };
}
