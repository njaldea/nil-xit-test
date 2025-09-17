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
    int run_help(const nil::clix::Options& options)
    {
        auto& instance = get_instance();

        help(options, std::cout);

        std::cout << "EXPECTED GROUPS:\n";
        for (const auto& g : instance.paths.used_groups)
        {
            std::cout << " -  " << g << '\n';
        }
        return 0;
    }

    bool parse_groups(const nil::clix::Options& options)
    {
        bool status = true;
        auto& instance = get_instance();
        if (has_value(options, "path-group"))
        {
            for (const auto& path_group : params(options, "path-group"))
            {
                auto i = path_group.find_first_of('=');
                if (i == std::string::npos)
                {
                    if (status)
                    {
                        std::cerr << "incorrect path_group format\n";
                    }
                    std::cerr << " -  " << path_group << '\n';
                    status = false;
                }
                else
                {
                    instance.paths.groups.emplace(
                        path_group.substr(0, i),
                        path_group.substr(i + 1)
                    );
                }
            }
            if (!status)
            {
                std::cerr << "expecting: <key>=<value>\n\n";
            }
        }

        if (!flag(options, "ignore-missing-groups"))
        {
            for (const auto& group : instance.paths.used_groups)
            {
                if (!instance.paths.groups.contains(group))
                {
                    status = false;
                    std::cerr << "missing group: " << group << '\n';
                }
            }
        }

        return status;
    }

    void print_groups()
    {
        auto& instance = get_instance();
        std::cout << "GROUPS:\n";
        for (const auto& g : instance.paths.used_groups)
        {
            auto it = instance.paths.groups.find(g);
            if (it != instance.paths.groups.end())
            {
                std::cout << " -  " << g << '=' << it->second.c_str() << '\n';
            }
            else
            {
                std::cout << " -  " << g << "=??\n";
            }
        }
    }

    int run_gui(const nil::clix::Options& options)
    {
        if (flag(options, "help"))
        {
            return run_help(options);
        }

        if (!parse_groups(options))
        {
            return 1;
        }

        auto& instance = get_instance();

        if (has_value(options, "path-assets"))
        {
            for (const auto& asset_path : params(options, "path-assets"))
            {
                instance.paths.assets.emplace_back(asset_path);
            }
        }

        if (flag(options, "clear"))
        {
            std::filesystem::remove_all(std::filesystem::temp_directory_path() / "nil-xit-gtest");
        }

        if (flag(options, "list"))
        {
            instance.test_builder.install(std::cout, instance.paths.groups);
            if (flag(options, "verbose"))
            {
                print_groups();
            }
            return 0;
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

        test::App app(
            use_ws(http_server, "/ws"),
            "nil-xit-gtest",
            std::uint32_t(number(options, "jobs"))
        );
        app.set_groups(instance.paths.groups);
        instance.frame_builder.install(app);
        instance.test_builder.install(app, instance.paths.groups);
        instance.main_builder.install(app);

        start(http_server);
        return 0;
    }

    int run_headless(const nil::clix::Options& options)
    {
        if (flag(options, "help"))
        {
            return run_help(options);
        }

        if (!parse_groups(options))
        {
            return 1;
        }

        auto& instance = get_instance();

        headless::CacheManager cache_manager;
        instance.frame_builder.install(cache_manager);
        instance.test_builder.install(cache_manager, instance.paths.groups);
        GTEST_FLAG_SET(list_tests, flag(options, "list"));
        ::testing::InitGoogleTest();
        const auto result = RUN_ALL_TESTS();

        if (flag(options, "list") && flag(options, "verbose"))
        {
            print_groups();
        }
        return result;
    }

    void node_gui(nil::clix::Node& node)
    {
        flag(node, "help", {.skey = 'h', .msg = "show this help"});
        flag(node, "list", {.skey = 'l', .msg = "list available tests"});
        flag(node, "verbose", {.skey = 'v', .msg = "list additiona information like groups"});
        flag(node, "clear", {.skey = 'c', .msg = "clear cache on boot"});
        flag(node, "ignore-missing-groups", {.skey = 'i', .msg = "ignore missing groups"});
        param(node, "host", {.skey = {}, .msg = "use host", .fallback = "127.0.0.1"});
        number(node, "port", {.skey = 'p', .msg = "use port", .fallback = 0});
        number(node, "jobs", {.skey = 'j', .msg = "number of jobs", .fallback = 1, .implicit = 0});
        params(node, "path-group", {.skey = 'g', .msg = "add group path"});
        params(node, "path-assets", {.skey = 'a', .msg = "use for assets"});
        use(node, run_gui);
    }

    void node_headless(nil::clix::Node& node)
    {
        flag(node, "help", {.skey = 'h', .msg = "show this help"});
        flag(node, "list", {.skey = 'l', .msg = "list available tests"});
        flag(node, "verbose", {.skey = 'v', .msg = "list additiona information like groups"});
        flag(node, "ignore-missing-groups", {.skey = 'i', .msg = "ignore missing groups"});
        params(node, "path-group", {.skey = 'g', .msg = "add group path"});
        sub(node, "gui", "run in gui mode", node_gui);
        use(node, run_headless);
    }

    int main(int argc, const char* const* argv)
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
