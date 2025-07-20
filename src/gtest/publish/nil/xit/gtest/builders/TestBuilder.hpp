#pragma once

#include "install.hpp"

#include <nil/xalt/transparent_stl.hpp>
#include <nil/xit/structs.hpp>

#include <filesystem>
#include <string>

namespace nil::xit::gtest::builders
{
    struct ITestInstaller
    {
        ITestInstaller() = default;
        virtual ~ITestInstaller() = default;
        ITestInstaller(ITestInstaller&&) = delete;
        ITestInstaller(const ITestInstaller&) = delete;
        ITestInstaller& operator=(ITestInstaller&&) = delete;
        ITestInstaller& operator=(const ITestInstaller&) = delete;

        virtual void install(
            test::App& app,
            const xalt::transparent_umap<std::filesystem::path>& groups
        ) = 0;

        virtual void install(
            headless::CacheManager& cache_manager,
            const xalt::transparent_umap<std::filesystem::path>& groups
        ) = 0;

        virtual void install(
            std::ostream& out,
            const xalt::transparent_umap<std::filesystem::path>& groups
        ) = 0;
    };

    template <typename T>
    struct TestInstaller: ITestInstaller
    {
        TestInstaller(
            // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            std::string_view init_suite_id,
            std::string_view init_test_id,
            xit::FileInfo init_file_info,
            const char* init_file,
            int init_line
        )
            : suite_id(init_suite_id)
            , test_id(init_test_id)
            , file_info(std::move(init_file_info))
            , file(init_file)
            , line(init_line)
        {
        }

        ~TestInstaller() override = default;
        TestInstaller(TestInstaller&&) = delete;
        TestInstaller(const TestInstaller&) = delete;
        TestInstaller& operator=(TestInstaller&&) = delete;
        TestInstaller& operator=(const TestInstaller&) = delete;

        void install(test::App& app, const xalt::transparent_umap<std::filesystem::path>& groups)
            override
        {
            install_impl(app, groups);
        }

        void install(
            headless::CacheManager& cache_manager,
            const xalt::transparent_umap<std::filesystem::path>& groups
        ) override
        {
            install_impl(cache_manager, groups);
        }

        void install(std::ostream& out, const xalt::transparent_umap<std::filesystem::path>& groups)
            override
        {
            install_impl(out, groups);
        }

    private:
        std::string suite_id;
        std::string test_id;
        xit::FileInfo file_info;
        const char* file;
        int line;

        void install_info(test::App& app, const FileInfo& info)
        {
            builders::install<T>(app, suite_id, test_id, info);
        }

        void install_info(headless::CacheManager& cache_manager, const FileInfo& info)
        {
            builders::install<T>(cache_manager, suite_id, test_id, info, file, line);
        }

        void install_info(std::ostream& oss, const FileInfo& info)
        {
            oss << to_tag(suite_id, test_id, info) << '\n';
        }

        template <typename Context>
        void install_impl(
            Context& context,
            const xalt::transparent_umap<std::filesystem::path>& groups
        )
        {
            const auto it = groups.find(file_info.group);
            if (it == groups.end())
            {
                // TODO: should fail?
                return;
            }
            const auto full_path = it->second / file_info.path;
            if (file_info.path.filename() == "*")
            {
                for (const auto& dir : std::filesystem::directory_iterator(full_path.parent_path()))
                {
                    if (!dir.is_directory())
                    {
                        continue;
                    }
                    install_info(
                        context,
                        FileInfo{
                            file_info.group,
                            (file_info.path.parent_path() / dir.path().filename()).string()
                        }
                    );
                }
            }
            else if (std::filesystem::is_directory(full_path))
            {
                install_info(context, FileInfo{file_info.group, file_info.path.string()});
            }
        }
    };

    class TestBuilder final
    {
        using group_map = xalt::transparent_umap<std::filesystem::path>;

    public:
        template <typename T>
        void add_test(
            std::string_view suite_id,
            std::string_view test_id,
            FileInfo file_info,
            const char* file,
            int line
        )
        {
            installer.push_back(std::make_unique<TestInstaller<Test<T>>>(
                suite_id,
                test_id,
                std::move(file_info),
                file,
                line
            ));
        }

        void install(test::App& app, const group_map& groups) const;
        void install(headless::CacheManager& cache_manager, const group_map& groups) const;
        void install(std::ostream& oss, const group_map& groups) const;

    private:
        std::vector<std::unique_ptr<ITestInstaller>> installer;
    };
}
