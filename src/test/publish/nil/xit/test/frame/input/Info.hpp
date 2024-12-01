#pragma once

#include <nil/gate/edges/Compatible.hpp>

#include "../../utils.hpp"

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
        virtual void finalize(std::string_view tag) const = 0;
    };

    template <typename Accessor, typename T>
    concept is_valid_value_getter //
        = std::is_same_v<         //
            utils::return_t<Accessor, T>,
            std::decay_t<utils::return_t<Accessor, T>>&>;

    template <typename T>
    struct Info: IInfo
    {
        using type = T;
        virtual nil::gate::edges::Compatible<T> get_input(std::string_view tag) = 0;
    };
}
