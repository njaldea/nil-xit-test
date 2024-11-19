#pragma once

#include "../IInfo.hpp"

#include <nil/gate/edges/Compatible.hpp>

namespace nil::xit::test::frame::input
{
    template <typename T>
    struct Info: IInfo
    {
        using type = T;
        virtual nil::gate::edges::Compatible<T> get_input(std::string_view tag) = 0;
    };
}
