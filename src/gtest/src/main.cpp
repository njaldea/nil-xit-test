#include <nil/xit/gtest.hpp>

#include <nil/service/ID.hpp>
#include <nil/service/http/server/create.hpp>
#include <nil/service/structs.hpp>

#include <nil/clix.hpp>
#include <nil/clix/node.hpp>
#include <nil/clix/structs.hpp>

#include <gtest/gtest.h>

#include <iostream>

namespace nil::xit::gtest
{
    int run_gui(const nil::clix::Options& options)
    {
        if (flag(options, "help"))
        {
            help(options, std::cout);
            return 0;
        }

        auto& instance = get_instance();

        if (flag(options, "list"))
        {
            instance.test_builder.install(std::cout, instance.paths.test);
            return 0;
        }

        if (flag(options, "clear"))
        {
            std::filesystem::remove_all(std::filesystem::temp_directory_path() / "nil-xit-gtest");
        }

        const auto http_server = nil::service::http::server::create({
            .host = param(options, "host"),
            .port = std::uint16_t(number(options, "port")),
            .buffer = 1024ul * 1024ul * 100ul //
        });

        nil::xit::setup_server(http_server, instance.paths.assets);

        on_ready(
            http_server,
            [](const nil::service::ID& id) { std::cout << "http://" << id.text << std::endl; }
        );

        test::App app(use_ws(http_server, "/ws"), "nil-xit-gtest");
        instance.frame_builder.install(app, instance.paths.ui);
        instance.test_builder.install(app, instance.paths.test);
        instance.main_builder.install(app, instance.paths.main_ui);

        start(http_server);
        return 0;
    }

    int run_headless(const nil::clix::Options& options)
    {
        if (flag(options, "help"))
        {
            help(options, std::cout);
            return 0;
        }
        headless::Inputs inputs;
        auto& instance = get_instance();
        instance.frame_builder.install(inputs);
        instance.test_builder.install(inputs, instance.paths.test);
        GTEST_FLAG_SET(list_tests, flag(options, "list"));
        ::testing::InitGoogleTest();
        return RUN_ALL_TESTS();
    }

    void node_gui(nil::clix::Node& node)
    {
        flag(node, "help", {.skey = 'h', .msg = "show this help"});
        flag(node, "list", {.skey = 'l', .msg = "list available tests"});
        flag(node, "clear", {.skey = 'c', .msg = "clear cache on boot"});
        param(node, "host", {.skey = {}, .msg = "use host", .fallback = "127.0.0.1"});
        number(node, "port", {.skey = 'p', .msg = "use port", .fallback = 0});
        use(node, run_gui);
    }

    void node_headless(nil::clix::Node& node)
    {
        flag(node, "help", {.skey = 'h', .msg = "show this help"});
        flag(node, "list", {.skey = 'l', .msg = "list available tests"});
        sub(node, "gui", "run in gui mode", node_gui);
        use(node, run_headless);
    }

    int main(int argc, const char** argv)
    {
        auto node = nil::clix::create_node();
        node_headless(node);
        return nil::clix::run(node, argc, argv);
    }
}

__attribute__((weak)) int main(int argc, const char** argv)
{
    return nil::xit::gtest::main(argc, argv);
}
