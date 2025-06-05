#pragma once

#include "install.hpp"

#include <filesystem>
#include <nil/xalt/transparent_stl.hpp>
#include <nil/xit/structs.hpp>
#include <string>

namespace nil::xit::gtest::builders
{
    struct TestInstaller
    {
        TestInstaller(
            std::string init_suite_id,
            std::string init_test_id,
            xit::FileInfo init_file_info,
            const char* init_file,
            int init_line
        )
            : suite_id(std::move(init_suite_id))
            , test_id(std::move(init_test_id))
            , file_info(std::move(init_file_info))
            , file(init_file)
            , line(init_line)
        {
        }

        virtual ~TestInstaller() = default;
        TestInstaller(TestInstaller&&) = default;
        TestInstaller(const TestInstaller&) = default;
        TestInstaller& operator=(TestInstaller&&) = default;
        TestInstaller& operator=(const TestInstaller&) = default;

        void install(test::App& app, const xalt::transparent_umap<std::filesystem::path>& groups)
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
                    install(
                        app,
                        FileInfo{
                            file_info.group,
                            (file_info.path.parent_path() / dir.path().filename()).string() //
                        }
                    );
                }
            }
            else if (std::filesystem::is_directory(full_path))
            {
                install(app, FileInfo{file_info.group, file_info.path.string()});
            }
        }

        void install(
            headless::Inputs& inputs,
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
                    install(
                        inputs,
                        FileInfo{
                            file_info.group,
                            (file_info.path.parent_path() / dir.path().filename()).string() //
                        }
                    );
                }
            }
            else if (std::filesystem::is_directory(full_path))
            {
                install(inputs, FileInfo{file_info.group, file_info.path.string()});
            }
        }

        void install(std::ostream& out, const xalt::transparent_umap<std::filesystem::path>& groups)
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
                    if (dir.is_directory())
                    {
                        install(
                            out,
                            FileInfo{
                                file_info.group,
                                (file_info.path.parent_path() / dir.path().filename()).string() //
                            }
                        );
                    }
                }
            }
            else if (std::filesystem::is_directory(full_path))
            {
                install(out, FileInfo{file_info.group, file_info.path.string()});
            }
        }

        virtual void install(test::App& app, const FileInfo& file_info) = 0;
        virtual void install(headless::Inputs& inputs, const FileInfo& file_info) = 0;
        virtual void install(std::ostream& oss, const FileInfo& file_info) = 0;

        std::string suite_id;
        std::string test_id;
        xit::FileInfo file_info;
        const char* file;
        int line;
    };

    class TestBuilder final
    {
        using group_map = xalt::transparent_umap<std::filesystem::path>;

    public:
        template <typename T>
            requires requires() { typename T::base_t; }
        void add_test(
            const std::string& suite_id,
            const std::string& test_id,
            const FileInfo& file_info,
            const char* file,
            int line
        )
        {
            struct Installer: TestInstaller
            {
                using TestInstaller::TestInstaller;

                void install(test::App& app, const FileInfo& info) override
                {
                    builders::install<T>(app, suite_id, test_id, info);
                }

                void install(headless::Inputs& inputs, const FileInfo& info) override
                {
                    builders::install<T>(inputs, suite_id, test_id, info, file, line);
                }

                void install(std::ostream& oss, const FileInfo& info) override
                {
                    oss << to_tag(suite_id, test_id, info) << '\n';
                }
            };

            installer.push_back(
                std::make_unique<Installer>(suite_id, test_id, file_info, file, line)
            );
        }

        void install(test::App& app, const group_map& groups) const;
        void install(headless::Inputs& inputs, const group_map& groups) const;
        void install(std::ostream& oss, const group_map& groups) const;

    private:
        std::vector<std::unique_ptr<TestInstaller>> installer;
    };
}
