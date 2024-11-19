#include "xit_gtest.hpp"

#include <nil/clix.hpp>
#include <nil/clix/node.hpp>
#include <nil/clix/structs.hpp>

#include <iostream>

namespace nil::xit::gtest
{
    namespace builders
    {
        void MainBuilder::install(App& app, const std::filesystem::path& path) const
        {
            if (frame)
            {
                frame->install(app, path);
            }
        }

        void FrameBuilder::install(App& app, const std::filesystem::path& path) const
        {
            for (const auto& frame : frames)
            {
                frame->install(app, path);
            }
        }

        void TestBuilder::install(App& app, const std::filesystem::path& path) const
        {
            for (const auto& t : tests)
            {
                t(app, path);
            }
        }
    }

    inline Instances& get_instance()
    {
        static auto instance = Instances{};
        return instance;
    }

    int main(int argc, const char** argv)
    {
        auto node = nil::clix::create_node();
        flag(node, "help", {.skey = 'h', .msg = "show this help"});
        sub(node,
            "gui",
            "run in gui mode",
            [](nil::clix::Node& sub_node)
            {
                number(sub_node, "port", {.skey = 'p', .msg = "use port", .fallback = 0});
                use(sub_node,
                    [](const nil::clix::Options& options)
                    {
                        auto& instance = nil::xit::gtest::get_instance();

                        const auto http_server = nil::xit::make_server({
                            .source_path = instance.paths.server,
                            .port = std::uint16_t(number(options, "port")),
                            .buffer_size = 1024ul * 1024ul * 100ul //
                        });

                        on_ready(
                            http_server,
                            [](const nil::service::ID& id) { std::cout << id.text << std::endl; }
                        );

                        nil::xit::test::App app(http_server, "nil-xit-gtest");
                        instance.frame_builder.install(app, instance.paths.ui);
                        instance.test_builder.install(app, instance.paths.test);
                        instance.main_builder.install(app, instance.paths.main_ui);

                        start(http_server);
                        return 0;
                    });
            });
        use(node,
            [](const nil::clix::Options& options)
            {
                if (flag(options, "help"))
                {
                    help(options, std::cout);
                    return 0;
                }
                std::cout << "headless mode impl here" << std::endl;
                return 0;
            });
        return nil::clix::run(node, argc, argv);
    }
}

__attribute__((weak)) int main(int argc, const char** argv)
{
    return nil::xit::gtest::main(argc, argv);
}
