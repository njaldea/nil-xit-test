#pragma once

#include "builders/FrameBuilder.hpp"
#include "builders/MainBuilder.hpp"
#include "builders/TestBuilder.hpp"

#include <filesystem>

namespace nil::xit::gtest
{
    struct Instances
    {
        struct
        {
            std::filesystem::path server;
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
