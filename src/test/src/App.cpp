#include <nil/service/structs.hpp>
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
        // std::filesystem::remove_all(std::filesystem::temp_directory_path() / app_name);
        set_cache_directory(xit, std::filesystem::temp_directory_path() / app_name);
    }

    const std::vector<std::string>& App::installed_tags() const
    {
        return tags;
    }

    const std::vector<std::string_view>& App::installed_tag_inputs(std::string_view tag) const
    {
        if (auto it = tag_inputs.find(tag); it != tag_inputs.end())
        {
            return it->second;
        }
        return blank;
    }

    const std::vector<std::string_view>& App::installed_tag_outputs(std::string_view tag) const
    {
        if (auto it = tag_outputs.find(tag); it != tag_outputs.end())
        {
            return it->second;
        }
        return blank;
    }

    void App::add_info(
        std::string tag,
        std::vector<std::string_view> inputs,
        std::vector<std::string_view> outputs
    )
    {
        tag_inputs.emplace(tag, std::move(inputs));
        tag_outputs.emplace(tag, std::move(outputs));
        tags.emplace_back(std::move(tag));
    }

    void App::finalize_inputs(std::string_view tag) const
    {
        if (const auto it = tag_inputs.find(tag); it != tag_inputs.end())
        {
            for (const auto& frame_id : it->second)
            {
                if (const auto frame_it = input_frames.find(frame_id);
                    frame_it != input_frames.end())
                {
                    frame_it->second->finalize(tag);
                }
            }
        }
    }
}
