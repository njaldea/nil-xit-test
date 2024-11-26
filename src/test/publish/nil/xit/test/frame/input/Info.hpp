#pragma once

#include <nil/gate/edges/Compatible.hpp>

namespace nil::xit::test::frame::input
{
    struct IInfo
    {
        IInfo() = default;
        IInfo(IInfo&&) = delete;
        IInfo(const IInfo&) = delete;
        IInfo& operator=(IInfo&&) = delete;
        IInfo& operator=(const IInfo&) = delete;
        virtual ~IInfo() = default;
    };

    template <typename T>
    struct Info: IInfo
    {
        using type = T;
        virtual nil::gate::edges::Compatible<T> get_input(std::string_view tag) = 0;
    };
}
