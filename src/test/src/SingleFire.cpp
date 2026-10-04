// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

#include <nil/xit/test/SingleFire.hpp>

#include <utility>

namespace nil::xit::test
{
    bool SingleFire::operator==(const SingleFire& /* o */) const
    {
        return false;
    }

    bool SingleFire::pop() const
    {
        return std::exchange(value, false);
    }
}
