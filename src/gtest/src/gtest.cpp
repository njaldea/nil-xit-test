#include <nil/xit/gtest/Instances.hpp>
#include <nil/xit/gtest/Test.hpp>
#include <nil/xit/gtest/TestTracker.hpp>
#include <nil/xit/gtest/builders/FrameBuilder.hpp>
#include <nil/xit/gtest/builders/MainBuilder.hpp>
#include <nil/xit/gtest/builders/TestBuilder.hpp>

#include <nil/xit/structs.hpp>

#include <format>
#include <mutex>
#include <stdexcept>
#include <thread>

namespace nil::xit::gtest
{
    void TestTracker::set_result(bool value)
    {
        const auto tid = std::this_thread::get_id();
        std::lock_guard lock(mutex_);
        results_[tid] = value;
    }

    bool TestTracker::pop_result()
    {
        const auto tid = std::this_thread::get_id();
        std::lock_guard lock(mutex_);
        const auto node = results_.extract(tid);
        return !node.empty() && node.mapped();
    }

    TestTrackerScope::TestTrackerScope(TestTracker* init_res)
        : res(init_res)
    {
        res->set_result(true);
    }

    bool TestTrackerScope::pop()
    {
        const auto v = res->pop_result();
        res = nullptr;
        return v;
    }

    TestTrackerScope::~TestTrackerScope()
    {
        if (res != nullptr)
        {
            res->pop_result();
        }
    }

    Instances& get_instance()
    {
        static auto instance = Instances{};
        return instance;
    }

    namespace detail
    {
        xit::FileInfo tag_to_file_info(std::string_view tag)
        {
            const auto i1 = tag.find_last_of('[');
            const auto i2 = tag.find_last_of(':');
            const auto i3 = tag.find_last_of(']');
            if (i2 == std::string_view::npos)
            {
                return FileInfo{.group = std::string(tag.substr(i1 + 1, i3 - i1 - 1)), .path = "."};
            }
            return FileInfo{
                .group = std::string(tag.substr(i1 + 1, i2 - i1 - 1)),
                .path = tag.substr(i2 + 1, i3 - i2 - 1)
            };
        }

        xit::FileInfo path_to_file_info(std::string_view path)
        {
            const auto i1 = path.find_first_of('/');
            return FileInfo{
                .group = std::string(path.substr(1, i1 - 1)),
                .path = std::string(path.substr(i1 + 1))
            };
        }
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    std::filesystem::path resolve_from_tag(std::string_view tag, std::string_view path)
    {
        const auto& groups = get_instance().paths.groups;
        const auto file_info = detail::tag_to_file_info(tag);
        auto it = groups.find(file_info.group);
        if (it != groups.end())
        {
            return it->second / file_info.path / path;
        }
        throw std::runtime_error("unknown group");
    }

    std::filesystem::path resolve_from_path(std::string_view path)
    {
        const auto& groups = get_instance().paths.groups;
        const auto file_info = detail::path_to_file_info(path);
        auto it = groups.find(file_info.group);
        if (it != groups.end())
        {
            return it->second / file_info.path;
        }
        throw std::runtime_error("unknown group");
    }
}

namespace nil::xit::gtest::builders::main
{
    void MainFrame::install(test::App& app)
    {
        auto& frame = app.add_main(file_info);

        for (const auto& [key, value] : this->options)
        {
            add_option(frame, key, value);
        }
    }

    Frame& MainFrame::option(std::string key, std::string value)
    {
        options.emplace_back(std::move(key), std::move(value));
        return *this;
    }
}

namespace nil::xit::gtest::builders::view::test
{
    Frame& Frame::option(std::string key, std::string value)
    {
        options.emplace_back(std::move(key), std::move(value));
        return *this;
    }

    ViewFrame::ViewFrame(std::string init_id, FileInfo init_file_info)
        : id(std::move(init_id))
        , file_info(std::move(init_file_info))
    {
    }

    void ViewFrame::install(nil::xit::test::App& app)
    {
        auto* frame = app.add_test_view(id, file_info);

        for (const auto& value_installer : values)
        {
            value_installer(*frame);
        }

        for (const auto& [key, value] : options)
        {
            add_option(*frame->frame, key, value);
        }
    }
}

namespace nil::xit::gtest::builders::view::global
{
    Frame& Frame::option(std::string key, std::string value)
    {
        options.emplace_back(std::move(key), std::move(value));
        return *this;
    }

    ViewFrame::ViewFrame(std::string init_id, FileInfo init_file_info)
        : id(std::move(init_id))
        , file_info(std::move(init_file_info))
    {
    }

    void ViewFrame::install(nil::xit::test::App& app)
    {
        auto* frame = app.add_global_view(id, file_info);

        for (const auto& value_installer : values)
        {
            value_installer(*frame);
        }

        for (const auto& [key, value] : options)
        {
            add_option(*frame->frame, key, value);
        }
    }
}

namespace nil::xit::gtest::builders
{
    std::string to_tag_suffix(std::string_view test_id, const FileInfo& file_info)
    {
        return std::format("{}[{}:{}]", test_id, file_info.group, file_info.path.c_str());
    }

    std::string to_tag(
        std::string_view suite_id,
        std::string_view test_id,
        const FileInfo& file_info
    )
    {
        return std::format("{}.{}", suite_id, to_tag_suffix(test_id, file_info));
    }

    main::Frame& MainBuilder::create_main(FileInfo file_info)
    {
        auto f = std::make_unique<main::MainFrame>(std::move(file_info));
        auto* ptr = f.get();
        frame = std::move(f);
        return *ptr;
    }

    main::MainFrame::MainFrame(FileInfo init_file_info)
        : file_info(std::move(init_file_info))
    {
    }

    void MainBuilder::install(test::App& app) const
    {
        if (frame)
        {
            frame->install(app);
        }
        else
        {
            app.add_main(detail::get_file_info<"@nil-/xit/test/GTestMain.svelte">());
        }
    }

    FrameBuilder::FrameBuilder()
    {
        this->create_output<std::uint64_t>("tag_info", {});
    }

    void FrameBuilder::install(test::App& app) const
    {
        for (const auto& frame : view_frames)
        {
            frame->install(app);
        }
        for (const auto& frame : input_frames)
        {
            frame->install(app);
        }
        for (const auto& frame : output_frames)
        {
            frame->install(app);
        }
        for (const auto& frame : expect_frames)
        {
            frame->install(app);
        }
    }

    void FrameBuilder::install(headless::CacheManager& cache_manager) const
    {
        for (const auto& frame : input_frames)
        {
            frame->install(cache_manager);
        }
        for (const auto& frame : expect_frames)
        {
            frame->install(cache_manager);
        }
    }

    void TestBuilder::install(test::App& app, const group_map& groups) const
    {
        for (const auto& t : installer)
        {
            t->install(app, groups);
        }
    }

    void TestBuilder::install(headless::CacheManager& cache_manager, const group_map& groups) const
    {
        for (const auto& t : installer)
        {
            t->install(cache_manager, groups);
        }
    }

    void TestBuilder::install(std::ostream& oss, const group_map& groups) const
    {
        for (const auto& t : installer)
        {
            t->install(oss, groups);
        }
    }
}
