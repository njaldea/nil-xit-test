#include <nil/xit/gtest/Instances.hpp>
#include <nil/xit/gtest/builders/FrameBuilder.hpp>
#include <nil/xit/gtest/builders/MainBuilder.hpp>
#include <nil/xit/gtest/builders/TestBuilder.hpp>

#include <nil/xit/structs.hpp>

#include <sstream>
#include <stdexcept>

namespace nil::xit::gtest
{
    namespace builders
    {
        std::string to_tag_suffix(std::string_view test_id, const FileInfo& file_info)
        {
            std::ostringstream ss;
            ss << test_id                //
               << '['                    //
               << file_info.group        //
               << ':'                    //
               << file_info.path.c_str() //
               << ']';
            return ss.str();
        }

        std::string to_tag(
            std::string_view suite_id,
            std::string_view test_id,
            const FileInfo& file_info
        )
        {
            std::ostringstream ss;
            ss << suite_id << '.' << to_tag_suffix(test_id, file_info);
            return ss.str();
        }

        void MainBuilder::install(test::App& app) const
        {
            if (frame)
            {
                frame->install(app);
            }
            else
            {
                throw std::runtime_error("main frame is not registered");
            }
        }

        FrameBuilder::FrameBuilder()
        {
            this->create_output<bool>("tag_info", {});
        }

        void FrameBuilder::install(test::App& app) const
        {
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

        void TestBuilder::install(headless::CacheManager& cache_manager, const group_map& groups)
            const
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
