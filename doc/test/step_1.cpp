// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

// Step 1: plain gtest without frames
#include "Circle.hpp"
#include <gtest/gtest.h>

TEST(Sample, circle)
{
    auto sut = Circle{.x = 0.1, .y = 0.1, .radius = 1};

    (void)sut;
    // do your test here
}
