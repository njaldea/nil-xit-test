#include <nil/service/structs.hpp>
#include <nil/xit/structs.hpp>
#include <nil/xit/test.hpp>

#include <nil/gate/runners/Async.hpp>

#include <sstream>

namespace nil::xit::test
{
    App::App(
        nil::service::IRunnableService& run_service,
        nil::service::IEventService& event_service,
        std::string_view app_name,
        std::uint32_t jobs
    )
        : xit(nil::xit::create_core(run_service, event_service))
    {
        runner = std::make_unique<nil::gate::runners::Async>(jobs == 0 ? 10 : jobs);
        gate.set_runner(runner.get());
        event_service.on_ready([this]() { gate.commit(); });
        set_cache_directory(*xit, std::filesystem::temp_directory_path() / app_name);
    }

    App::~App() noexcept
    {
        destroy_core(xit);
    }

    void App::set_groups(const xalt::transparent_umap<std::filesystem::path>& paths)
    {
        nil::xit::set_groups(*xit, paths);
    }

    void App::start()
    {
        gate.commit();
    }

    std::span<const std::string_view> App::installed_tags() const
    {
        return tags_view;
    }

    std::span<const std::string_view> App::installed_tag_views(std::string_view tag) const
    {
        if (auto it = tag_info.find(tag); it != tag_info.end())
        {
            return it->second.views;
        }
        return {};
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
        std::vector<std::string_view> views,
        std::vector<std::string_view> inputs,
        std::vector<std::string_view> outputs,
        std::vector<std::string_view> expects
    )
    {
        auto sv = std::string_view(*tags.emplace(std::move(tag)).first);
        tag_info.emplace(
            sv,
            TagInfo{
                .views = std::move(views),
                .inputs = std::move(inputs),
                .outputs = std::move(outputs),
                .expects = std::move(expects)
            }
        );
        return tags_view.emplace_back(sv);
    }

    unique::Frame& App::add_main(const FileInfo& file_info)
    {
        static constexpr auto converter = [](std::span<const std::string_view> ids)
        {
            std::ostringstream oss;
            const auto size = ids.size();
            auto i = 0UL;
            for (const auto& id : ids)
            {
                oss << id;
                i += 1;
                if (i < size)
                {
                    oss << ',';
                }
            }
            return oss.str();
        };

        {
            auto& frame = add_tagged_frame(*xit, "frame_info");
            add_value(
                frame,
                "views",
                [this](std::string_view tag) { return converter(installed_tag_views(tag)); }
            );
            add_value(
                frame,
                "inputs",
                [this](std::string_view tag) { return converter(installed_tag_inputs(tag)); }
            );
            add_value(
                frame,
                "outputs",
                [this](std::string_view tag) { return converter(installed_tag_outputs(tag)); }
            );
            add_value(
                frame,
                "expects",
                [this](std::string_view tag) { return converter(installed_tag_expects(tag)); }
            );
        }
        auto& frame = add_unique_frame(*xit, "index", file_info);
        add_value(frame, "tags", [this]() { return converter(installed_tags()); });
        add_signal(
            frame,
            "finalize",
            [this](std::string_view tag) { return finalize_inputs(tag); }
        );
        return frame;
    }

    frame::view::test::Info* App::add_test_view(std::string_view id, FileInfo file_info)
    {
        auto p = std::make_unique<frame::view::test::Info>();
        auto* s = p.get();
        view_frames.emplace(*frames.emplace(id).first, std::move(p));

        s->frame = &add_tagged_frame(*xit, std::string(id), std::move(file_info));
        return s;
    }

    frame::view::global::Info* App::add_global_view(std::string_view id, FileInfo file_info)
    {
        auto p = std::make_unique<frame::view::global::Info>();
        auto* s = p.get();
        view_frames.emplace(*frames.emplace(id).first, std::move(p));

        s->frame = &add_unique_frame(*xit, std::string(id), std::move(file_info));
        return s;
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
