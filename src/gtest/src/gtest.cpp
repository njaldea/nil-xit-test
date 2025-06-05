#include <nil/xit/gtest/Instances.hpp>
#include <nil/xit/gtest/builders/FrameBuilder.hpp>
#include <nil/xit/gtest/builders/MainBuilder.hpp>
#include <nil/xit/gtest/builders/TestBuilder.hpp>

#include <nil/xit/structs.hpp>

#include <sstream>

namespace nil::xit::gtest
{
    namespace builders
    {
        std::string to_tag_suffix(std::string_view test_id, const FileInfo& file_info)
        {
            std::ostringstream ss;
            ss << test_id << '[' << file_info.group;
            if (file_info.path.filename() == ".")
            {
                ss << ']';
            }
            else
            {
                ss << ':' << file_info.path.c_str() << ']';
            }
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
        }

        void FrameBuilder::install(headless::Inputs& inputs) const
        {
            for (const auto& frame : input_frames)
            {
                frame->install(inputs);
            }
        }

        void TestBuilder::install(test::App& app, const group_map& groups) const
        {
            for (const auto& t : installer)
            {
                t->install(app, groups);
            }
        }

        void TestBuilder::install(headless::Inputs& inputs, const group_map& groups) const
        {
            for (const auto& t : installer)
            {
                t->install(inputs, groups);
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
    }
}
