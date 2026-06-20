#pragma once

#include "builders/FrameBuilder.hpp"
#include "builders/MainBuilder.hpp"
#include "builders/TestBuilder.hpp"

#include "TestTracker.hpp"

#include <nil/xalt/transparent_stl.hpp>

#include <filesystem>
#include <set>

namespace nil::xit::gtest
{
    struct Instances final
    {
        struct Paths
        {
            xalt::transparent_umap<std::filesystem::path> groups;
            std::set<std::string> used_test_groups;
            std::set<std::string> used_ui_groups;
        };

        Paths paths;
        builders::MainBuilder main_builder;
        builders::FrameBuilder frame_builder;
        builders::TestBuilder test_builder;
        TestTracker tracker;
    };

    Instances& get_instance();
}
