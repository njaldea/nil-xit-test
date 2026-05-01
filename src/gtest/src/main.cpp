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
    int run_help(const nil::clix::Options& options, const auto& groups)
    {
        help(options, std::cout);

        if (!groups.empty())
        {
            std::cout << "\nEXPECTED GROUPS:\n";
            for (const auto& group : groups)
            {
                std::cout << " -  " << group << '\n';
            }
        }
        return 0;
    }

    bool parse_groups(const nil::clix::Options& options, const auto& groups)
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

        if (flag(options, "verbose"))
        {
            for (const auto& group : instance.paths.groups)
            {
                if (!groups.contains(group.first))
                {
                    std::cerr << "unknown group: " << group.first << '\n';
                }
            }
        }

        if (!flag(options, "ignore-missing-groups"))
        {
            for (const auto& group : groups)
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

    void print_groups(const auto& groups)
    {
        auto& instance = get_instance();
        std::cout << "GROUPS:\n";
        for (const auto& group : groups)
        {
            auto it = instance.paths.groups.find(group);
            if (it != instance.paths.groups.end())
            {
                std::cout << " -  " << group << '=' << it->second.c_str() << '\n';
            }
            else
            {
                std::cout << " -  " << group << "=??\n";
            }
        }
    }

    int run_gui(const nil::clix::Options& options)
    {
        auto& instance = get_instance();

        std::set<std::string_view> groups;
        groups.insert(
            instance.paths.used_test_groups.begin(),
            instance.paths.used_test_groups.end()
        );
        groups.insert(instance.paths.used_ui_groups.begin(), instance.paths.used_ui_groups.end());

        if (flag(options, "help"))
        {
            return run_help(options, groups);
        }

        if (!parse_groups(options, groups))
        {
            return 1;
        }

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
                print_groups(groups);
            }
            return 0;
        }

        const auto http_server = nil::service::http::server::create({
            .host = param(options, "host"),
            .port = std::uint16_t(number(options, "port")),
            .buffer = 1024ul * 1024ul * 100ul //
        });

        nil::xit::setup_server(*http_server, instance.paths.assets);

        http_server->on_ready([](const nil::service::ID& id)
                              { std::cout << "http://" << to_string(id) << std::endl; });

        test::App app(
            *http_server,
            *http_server->use_ws("/ws"),
            "nil-xit-gtest",
            std::uint32_t(number(options, "jobs"))
        );
        app.set_groups(instance.paths.groups);
        instance.frame_builder.install(app);
        instance.test_builder.install(app, instance.paths.groups);
        instance.main_builder.install(app);
        app.start();

        http_server->run();
        return 0;
    }

    int run_headless(const nil::clix::Options& options)
    {
        if (flag(options, "help"))
        {
            return run_help(options, get_instance().paths.used_test_groups);
        }

        if (!parse_groups(options, get_instance().paths.used_test_groups))
        {
            return 1;
        }

        auto& instance = get_instance();

        headless::CacheManager cache_manager;
        instance.frame_builder.install(cache_manager);
        instance.test_builder.install(cache_manager, instance.paths.groups);
        ::testing::GTEST_FLAG(list_tests) = flag(options, "list");
        ::testing::InitGoogleTest();
        const auto result = RUN_ALL_TESTS();

        if (flag(options, "list") && flag(options, "verbose"))
        {
            print_groups(instance.paths.used_test_groups);
        }
        return result;
    }

    void node_gui(nil::clix::Node& node)
    {
        flag(node, "help", {.skey = 'h', .msg = "show this help"});
        flag(node, "list", {.skey = 'l', .msg = "list available tests"});
        flag(node, "verbose", {.skey = 'v', .msg = "list additional information like groups"});
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
        flag(node, "verbose", {.skey = 'v', .msg = "list additional information like groups"});
        flag(node, "ignore-missing-groups", {.skey = 'i', .msg = "ignore missing groups"});
        params(node, "path-group", {.skey = 'g', .msg = "add group path"});
        sub(node, "gui", "run in gui mode", node_gui);
        use(node, run_headless);
    }

    int main(int argc, const char* const* argv)
    {
        auto* node = nil::clix::create_node();
        node_headless(*node);
        auto v = nil::clix::run(*node, argc - 1, argv + 1);
        clix::destroy_node(node);
        return v;
    }
}

__attribute__((weak)) int main(int argc, const char** argv)
{
    return nil::xit::gtest::main(argc, argv);
}
