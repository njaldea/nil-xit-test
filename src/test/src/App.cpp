#include <nil/xit/test.hpp>

// TODO: use better runner like asio/parallel
#include <nil/gate/runners/NonBlocking.hpp>

#include <iostream>

namespace nil::xit::test
{
    App::App(nil::service::S service, std::string_view app_name)
        : xit(nil::xit::make_core(service))
    {
        gate.set_runner<nil::gate::runners::NonBlocking>();
        on_ready(service, [this]() { gate.commit(); });
        set_cache_directory(xit, std::filesystem::temp_directory_path() / app_name);
    }

    App::App(nil::service::HTTPService& service, std::string_view app_name)
        : xit(nil::xit::make_core(service))
    {
        gate.set_runner<nil::gate::runners::NonBlocking>();
        on_ready(service, [this]() { gate.commit(); });
        set_cache_directory(xit, std::filesystem::temp_directory_path() / app_name);
    }

    const std::vector<std::string>& App::installed_tags() const
    {
        return tags;
    }

    const std::vector<std::string>& App::installed_frame_inputs(std::string_view tag) const
    {
        if (auto it = frame_inputs.find(tag); it != frame_inputs.end())
        {
            return it->second;
        }
        return blank;
    }

    const std::vector<std::string>& App::installed_frame_outputs(std::string_view tag) const
    {
        if (auto it = frame_outputs.find(tag); it != frame_outputs.end())
        {
            return it->second;
        }
        return blank;
    }

    void App::finalize_inputs(std::string_view tag) const
    {
        if (const auto it = frame_inputs.find(tag); it != frame_inputs.end())
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
