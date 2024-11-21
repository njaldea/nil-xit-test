#pragma once

#include "../IFrame.hpp"

namespace nil::xit::gtest::headless
{
    class Inputs;
}

namespace nil::xit::gtest::builders::input
{
    struct Frame: IFrame
    {
        using IFrame::install;
        virtual void install(headless::Inputs& inputs) = 0;
    };
}
