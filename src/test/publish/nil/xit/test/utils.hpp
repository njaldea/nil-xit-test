// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

#pragma once

#include <utility>

namespace nil::xit::test::utils
{
    template <typename Callable, typename... T>
    using return_t = decltype(std::declval<Callable>()(std::declval<T>()...));
}
