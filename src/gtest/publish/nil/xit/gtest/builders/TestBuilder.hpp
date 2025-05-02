#pragma once

#include "install.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace nil::xit::gtest::builders
{
    struct TestInstaller
    {
        TestInstaller(
            std::string init_suite_id,
            std::string init_test_id,
            std::string init_path,
            const char* init_file,
            int init_line
        )
            : suite_id(std::move(init_suite_id))
            , test_id(std::move(init_test_id))
            , path(std::move(init_path))
            , file(init_file)
            , line(init_line)
        {
        }

        virtual ~TestInstaller() = default;
        TestInstaller(TestInstaller&&) = default;
        TestInstaller(const TestInstaller&) = default;
        TestInstaller& operator=(TestInstaller&&) = default;
        TestInstaller& operator=(const TestInstaller&) = default;

        void install(test::App& app, const std::filesystem::path& relative_path)
        {
            std::filesystem::path p = path;
            const auto full_path = relative_path / p;
            if (p.filename() == "*")
            {
                for (const auto& dir : std::filesystem::directory_iterator(full_path.parent_path()))
                {
                    if (dir.is_directory())
                    {
                        install(app, (p.parent_path() / dir.path().filename()).string());
                    }
                }
            }
            else if (std::filesystem::is_directory(full_path))
            {
                install(app, p.string());
            }
        }

        void install(headless::Inputs& inputs, const std::filesystem::path& relative_path)
        {
            std::filesystem::path p = path;
            const auto full_path = relative_path / p;
            if (p.filename() == "*")
            {
                for (const auto& dir : std::filesystem::directory_iterator(full_path.parent_path()))
                {
                    if (dir.is_directory())
                    {
                        install(inputs, (p.parent_path() / dir.path().filename()).string());
                    }
                }
            }
            else if (std::filesystem::is_directory(full_path))
            {
                install(inputs, p.string());
            }
        }

        void install(std::ostream& out, const std::filesystem::path& relative_path)
        {
            std::filesystem::path p = path;
            const auto full_path = relative_path / p;
            if (p.filename() == "*")
            {
                for (const auto& dir : std::filesystem::directory_iterator(full_path.parent_path()))
                {
                    if (dir.is_directory())
                    {
                        install(out, (p.parent_path() / dir.path().filename()).string());
                    }
                }
            }
            else if (std::filesystem::is_directory(full_path))
            {
                install(out, p.string());
            }
        }

        virtual void install(test::App& app, const std::string& tag) = 0;
        virtual void install(headless::Inputs& inputs, const std::string& tag) = 0;
        virtual void install(std::ostream& oss, const std::string& tag) = 0;

        std::string suite_id;
        std::string test_id;
        std::string path;
        const char* file;
        int line;
    };

    class TestBuilder final
    {
    public:
        template <typename T>
            requires requires() { typename T::base_t; }
        void add_test(
            const std::string& suite_id,
            const std::string& test_id,
            const std::filesystem::path& path,
            const char* file,
            int line
        )
        {
            struct Installer: TestInstaller
            {
                using TestInstaller::TestInstaller;

                void install(test::App& app, const std::string& tag) override
                {
                    builders::install<T>(app, suite_id, test_id, tag);
                }

                void install(headless::Inputs& inputs, const std::string& tag) override
                {
                    builders::install<T>(inputs, suite_id, test_id, tag, file, line);
                }

                void install(std::ostream& oss, const std::string& tag) override
                {
                    oss << to_tag(suite_id, test_id, tag);
                }
            };

            installer.push_back(std::make_unique<Installer>(suite_id, test_id, path, file, line));
        }

        void install(test::App& app, const std::filesystem::path& path) const;
        void install(headless::Inputs& inputs, const std::filesystem::path& path) const;
        void install(std::ostream& oss, const std::filesystem::path& path) const;

    private:
        std::vector<std::unique_ptr<TestInstaller>> installer;
    };
}
