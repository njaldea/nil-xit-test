#pragma once

#include "../../RerunTag.hpp"
#include "../../transparent_hash_map.hpp"
#include "../../utils.hpp"

#include <nil/xit/tagged/add_value.hpp>
#include <nil/xit/tagged/post.hpp>
#include <nil/xit/tagged/structs.hpp>

#include <nil/gate/Core.hpp>

#include <functional>
#include <string_view>
#include <type_traits>

namespace nil::xit::test::frame::output
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

    template <typename Accessor, typename T>
    concept is_valid_value_getter //
        = std::is_same_v<
            utils::return_t<Accessor, T>,
            const std::decay_t<utils::return_t<Accessor, T>>&>;

    template <typename T>
    struct Info: IInfo
    {
        using type = T;
        nil::xit::tagged::Frame* frame = nullptr;
        transparent::hash_map<nil::gate::edges::Mutable<RerunTag>*> rerun;
        transparent::hash_map<nil::gate::edges::Mutable<bool>*> requested;
        std::vector<std::function<void(std::string_view, const T&)>> values;

        template <typename V, is_valid_value_getter<const T&> Getter>
        void add_value(std::string id, Getter getter)
        {
            auto* value = &nil::xit::tagged::add_value(
                *frame,
                std::move(id),
                [](std::string_view /* tag */) { return V(); }
            );
            values.push_back( //
                [value, getter = std::move(getter)](std::string_view tag, const T& data)
                { nil::xit::tagged::post(tag, *value, getter(data)); }
            );
        }
    };
}
