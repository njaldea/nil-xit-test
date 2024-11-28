#include <nil/xit/gtest.hpp>

#include <nil/service/ID.hpp>
#include <nil/service/structs.hpp>

#include <nil/clix.hpp>
#include <nil/clix/node.hpp>
#include <nil/clix/structs.hpp>

#include <gtest/gtest.h>

#include <iostream>

namespace nil::xit::gtest
{
    int main(int argc, const char** argv)
    {
        auto node = nil::clix::create_node();
        flag(node, "help", {.skey = 'h', .msg = "show this help"});
        sub(node,
            "gui",
            "run in gui mode",
            [](nil::clix::Node& sub_node)
            {
                param(sub_node, "host", {.skey = {}, .msg = "use host", .fallback = "127.0.0.1"});
                number(sub_node, "port", {.skey = 'p', .msg = "use port", .fallback = 0});
                flag(sub_node, "help", {.skey = 'h', .msg = "show this help"});
                use(sub_node,
                    [](const nil::clix::Options& options)
                    {
                        if (flag(options, "help"))
                        {
                            help(options, std::cout);
                            return 0;
                        }
                        auto& instance = nil::xit::gtest::get_instance();

                        const auto http_server = nil::xit::make_server({
                            .source_path = instance.paths.server,
                            .host = param(options, "host"),
                            .port = std::uint16_t(number(options, "port")),
                            .buffer_size = 1024ul * 1024ul * 100ul //
                        });

                        on_ready(
                            http_server,
                            [](const nil::service::ID& id)
                            { std::cout << "http://" << id.text << std::endl; }
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
                headless::Inputs inputs;
                auto& instance = nil::xit::gtest::get_instance();
                instance.frame_builder.install(inputs);
                instance.test_builder.install(inputs, instance.paths.test);
                ::testing::InitGoogleTest();
                return RUN_ALL_TESTS();
            });
        return nil::clix::run(node, argc, argv);
    }
}

__attribute__((weak)) int main(int argc, const char** argv)
{
    return nil::xit::gtest::main(argc, argv);
}
