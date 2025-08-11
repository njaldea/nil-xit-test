#pragma once

#include "builders/FrameBuilder.hpp"
#include "builders/MainBuilder.hpp"
#include "builders/TestBuilder.hpp"

#include <nil/xalt/transparent_stl.hpp>

#include <filesystem>
#include <set>
#include <vector>

namespace nil::xit::gtest
{
    struct Instances final
    {
        struct Paths
        {
            std::vector<std::filesystem::path> assets;
            xalt::transparent_umap<std::filesystem::path> groups;
            std::set<std::string> used_groups;
        };

        Paths paths;
        builders::MainBuilder main_builder;
        builders::FrameBuilder frame_builder;
        builders::TestBuilder test_builder;
    };

    Instances& get_instance();
}
