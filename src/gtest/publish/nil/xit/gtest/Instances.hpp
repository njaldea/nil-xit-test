#pragma once

#include "builders/FrameBuilder.hpp"
#include "builders/MainBuilder.hpp"
#include "builders/TestBuilder.hpp"

#include <filesystem>
#include <vector>

namespace nil::xit::gtest
{
    struct Instances final
    {
        struct
        {
            std::vector<std::filesystem::path> assets;
            std::filesystem::path main_ui;
            std::filesystem::path ui;
            std::filesystem::path test;
        } paths;

        builders::MainBuilder main_builder;
        builders::FrameBuilder frame_builder;
        builders::TestBuilder test_builder;
    };

    Instances& get_instance();
}
