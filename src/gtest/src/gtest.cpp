#include <nil/xit/gtest/builders/FrameBuilder.hpp>
#include <nil/xit/gtest/builders/MainBuilder.hpp>
#include <nil/xit/gtest/builders/TestBuilder.hpp>

#include <nil/xit/gtest/utils.hpp>

namespace nil::xit::gtest
{
    namespace builders
    {
        std::string to_tag(
            const std::string& suite_id,
            const std::string& test_id,
            const std::string& dir
        )
        {
            return suite_id + '.' + test_id + '[' + dir + ']';
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
    }

    Instances& get_instance()
    {
        static auto instance = Instances{};
        return instance;
    }
}
