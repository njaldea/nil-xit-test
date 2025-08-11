#pragma once

#include "../IFrame.hpp"

namespace nil::xit::gtest::headless
{
    struct CacheManager;
}

namespace nil::xit::gtest::builders::input
{
    struct IFrame: builders::IFrame
    {
        using builders::IFrame::install;
        virtual void install(headless::CacheManager& cache_manager) = 0;
    };
}
