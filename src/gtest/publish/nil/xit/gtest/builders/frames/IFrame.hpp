#pragma once

#include <filesystem>

namespace nil::xit::test
{
    class App;
}

namespace nil::xit::gtest::builders
{
    struct IFrame
    {
        virtual ~IFrame() = default;
        IFrame() = default;
        IFrame(IFrame&&) = delete;
        IFrame(const IFrame&) = delete;
        IFrame& operator=(IFrame&&) = delete;
        IFrame& operator=(const IFrame&) = delete;
        virtual void install(test::App& app, const std::filesystem::path& path) = 0;
    };
}
