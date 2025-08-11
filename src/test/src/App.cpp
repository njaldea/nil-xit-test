#include <nil/service/structs.hpp>
#include <nil/xit/structs.hpp>
#include <nil/xit/test.hpp>

// TODO: use better runner like asio/parallel
#include <nil/gate/runners/NonBlocking.hpp>

namespace nil::xit::test
{
    App::App(nil::service::P service, std::string_view app_name)
        : xit(nil::xit::make_core(service))
    {
        gate.set_runner<nil::gate::runners::NonBlocking>();
        on_ready(service, [this]() { gate.commit(); });
        set_cache_directory(xit, std::filesystem::temp_directory_path() / app_name);
    }

    void App::set_groups(const xalt::transparent_umap<std::filesystem::path>& paths)
    {
        nil::xit::set_groups(xit, paths);
    }

    std::span<const std::string_view> App::installed_tags() const
    {
        return tags_view;
    }

    std::span<const std::string_view> App::installed_tag_inputs(std::string_view tag) const
    {
        if (auto it = tag_info.find(tag); it != tag_info.end())
        {
            return it->second.inputs;
        }
        return {};
    }

    std::span<const std::string_view> App::installed_tag_outputs(std::string_view tag) const
    {
        if (auto it = tag_info.find(tag); it != tag_info.end())
        {
            return it->second.outputs;
        }
        return {};
    }

    std::span<const std::string_view> App::installed_tag_expects(std::string_view tag) const
    {
        if (auto it = tag_info.find(tag); it != tag_info.end())
        {
            return it->second.expects;
        }
        return {};
    }

    std::string_view App::add_info(
        std::string tag,
        std::vector<std::string_view> inputs,
        std::vector<std::string_view> outputs,
        std::vector<std::string_view> expects
    )
    {
        auto sv = std::string_view(*tags.emplace(std::move(tag)).first);
        tag_info.emplace(
            sv,
            TagInfo{
                .inputs = std::move(inputs),
                .outputs = std::move(outputs),
                .expects = std::move(expects)
            }
        );
        return tags_view.emplace_back(sv);
    }

    void App::finalize_inputs(std::string_view tag) const
    {
        if (const auto it = tag_info.find(tag); it != tag_info.end())
        {
            for (const auto& frame_id : it->second.inputs)
            {
                if (const auto frame_it
                    = input_frames.find(frame_id.substr(0, frame_id.size() - 4));
                    frame_it != input_frames.end())
                {
                    frame_it->second->finalize(tag);
                }
            }
        }
    }
}
