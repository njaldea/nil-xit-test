#pragma once

#include <nil/gate/ports/Compatible.hpp>

#include "../../utils.hpp"

#include <type_traits>

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
        virtual void initialize(std::string_view tag) = 0;
    };

    template <typename Accessor, typename T>
    concept is_valid_value_accessor //
        = std::is_lvalue_reference_v<utils::return_t<Accessor, T>>
        && !std::is_const_v<std::remove_reference_t<utils::return_t<Accessor, T>>>;

    template <typename T>
    struct Info: IInfo
    {
        using type = T;
        virtual nil::gate::ports::Compatible<T> get_input(std::string_view tag) = 0;
    };
}
