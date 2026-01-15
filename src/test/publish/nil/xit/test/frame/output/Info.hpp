#pragma once

#include "../../RerunTag.hpp"
#include "../../utils.hpp"

#include <nil/xit/tagged/add_value.hpp>
#include <nil/xit/tagged/post.hpp>
#include <nil/xit/tagged/structs.hpp>

#include <nil/gate/Core.hpp>

#include <functional>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

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
    concept is_valid_value_accessor //
        = std::is_lvalue_reference_v<utils::return_t<Accessor, T>>
        && std::is_const_v<std::remove_reference_t<utils::return_t<Accessor, T>>>;

    template <typename T>
    struct Info: IInfo
    {
        using type = T;

        struct Entry
        {
            nil::gate::ports::External<RerunTag>* rerun = nullptr;
            nil::gate::ports::External<bool>* requested = nullptr;
        };

        nil::xit::tagged::Frame* frame = nullptr;
        nil::gate::Core* gate = nullptr;
        std::unordered_map<std::string_view, Entry> info; // sv owned by tags from App
        std::vector<std::function<void(std::string_view, const T&)>> values;

        template <typename V, is_valid_value_accessor<const T&> Accessor>
        void add_value(std::string id, Accessor accessor)
        {
            auto* value = &nil::xit::tagged::add_value(
                *frame,
                std::move(id),
                [](std::string_view /* tag */) { return V(); }
            );
            values.push_back( //
                [value, accessor = std::move(accessor)](std::string_view tag, const T& data)
                { nil::xit::tagged::post(tag, *value, accessor(data)); }
            );
        }

        void add_info(std::string_view tag)
        {
            gate->post(
                [tag, this](gate::Graph& graph) {
                    info.emplace(
                        tag,
                        Entry{.rerun = graph.port(RerunTag()), .requested = graph.port(false)}
                    );
                }
            );
        }

        void post(std::string_view tag, const T& data)
        {
            for (const auto& value : values)
            {
                value(tag, data);
            }
        }

        gate::ports::External<bool>* info_requested(std::string_view tag) const
        {
            if (const auto it = info.find(tag); info.end() != it)
            {
                return it->second.requested;
            }
            return nullptr;
        }

        gate::ports::External<RerunTag>* info_rerun(std::string_view tag) const
        {
            if (const auto it = info.find(tag); info.end() != it)
            {
                return it->second.rerun;
            }
            return nullptr;
        }
    };
}
