#pragma once

#include "install.hpp"

#include <filesystem>
#include <functional>
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
            for (const auto& dir : std::filesystem::directory_iterator(relative_path / path))
            {
                if (dir.is_directory())
                {
                    install(app, dir.path().filename().string());
                }
            }
        }

        void install(headless::Inputs& inputs, const std::filesystem::path& relative_path)
        {
            for (const auto& dir : std::filesystem::directory_iterator(relative_path / path))
            {
                if (dir.is_directory())
                {
                    install(inputs, dir.path().filename().string());
                }
            }
        }

        virtual void install(test::App& app, const std::string& tag) = 0;
        virtual void install(headless::Inputs& inputs, const std::string& tag) = 0;

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
                    nil::xit::gtest::builders::install<T> //
                        (app, type<typename T::base_t>(), suite_id, test_id, tag);
                }

                void install(headless::Inputs& inputs, const std::string& tag) override
                {
                    nil::xit::gtest::builders::install<T> //
                        (inputs, type<typename T::base_t>(), suite_id, test_id, tag, file, line);
                }
            };

            installer.push_back(std::make_unique<Installer>(suite_id, test_id, path, file, line));
        }

        void install(test::App& app, const std::filesystem::path& path) const;
        void install(headless::Inputs& inputs, const std::filesystem::path& path) const;

    private:
        std::vector<std::unique_ptr<TestInstaller>> installer;
    };
}
