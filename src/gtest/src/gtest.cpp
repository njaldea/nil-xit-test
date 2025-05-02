#include <nil/xit/gtest/builders/FrameBuilder.hpp>
#include <nil/xit/gtest/builders/MainBuilder.hpp>
#include <nil/xit/gtest/builders/TestBuilder.hpp>

#include <nil/xit/gtest/Instances.hpp>

namespace nil::xit::gtest
{
    namespace builders
    {
        std::string to_tag_suffix(const std::string& test_id, const std::string& dir)
        {
            if (dir == ".")
            {
                return test_id;
            }
            return test_id + '[' + dir + ']';
        }

        std::string to_tag(
            const std::string& suite_id,
            const std::string& test_id,
            const std::string& dir
        )
        {
            return suite_id + '.' + to_tag_suffix(test_id, dir);
        }

        void MainBuilder::install(test::App& app, const std::filesystem::path& path) const
        {
            if (frame)
            {
                frame->install(app, path);
            }
        }

        void FrameBuilder::install(test::App& app, const std::filesystem::path& path) const
        {
            for (const auto& frame : input_frames)
            {
                frame->install(app, path);
            }
            for (const auto& frame : output_frames)
            {
                frame->install(app, path);
            }
        }

        void FrameBuilder::install(headless::Inputs& inputs) const
        {
            for (const auto& frame : input_frames)
            {
                frame->install(inputs);
            }
        }

        void TestBuilder::install(test::App& app, const std::filesystem::path& path) const
        {
            for (const auto& t : installer)
            {
                t->install(app, path);
            }
        }

        void TestBuilder::install(headless::Inputs& inputs, const std::filesystem::path& path) const
        {
            for (const auto& t : installer)
            {
                t->install(inputs, path);
            }
        }

        void TestBuilder::install(std::ostream& oss, const std::filesystem::path& path) const
        {
            for (const auto& t : installer)
            {
                t->install(oss, path);
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
        std::string_view tag_to_dir(std::string_view tag)
        {
            const auto i1 = tag.find_last_of('[') + 1;
            const auto i2 = tag.find_last_of(']');
            if (i1 < tag.size() - 1 && i2 == tag.size() - 1)
            {
                return tag.substr(i1, i2 - i1);
            }
            return ".";
        }
    }
}
