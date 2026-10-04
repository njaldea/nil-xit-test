// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

#include <nil/xit/test/RerunTag.hpp>

namespace nil::xit::test
{
    bool RerunTag::operator==(const RerunTag& /* o */) const
    {
        return false;
    }
}
