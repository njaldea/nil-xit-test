#pragma once

#include <nil/xalt/transparent_stl.hpp>

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
        virtual void install(test::App& app) = 0;
    };
}
