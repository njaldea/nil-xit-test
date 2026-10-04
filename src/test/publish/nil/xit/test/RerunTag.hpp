// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

#pragma once

namespace nil::xit::test
{
    struct RerunTag
    {
        bool operator==(const RerunTag& /* o */) const;
    };
}
