#pragma once

#include "builders/FrameBuilder.hpp"
#include "builders/MainBuilder.hpp"
#include "builders/TestBuilder.hpp"

#include "TestTracker.hpp"

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
        TestTracker tracker;
    };

    Instances& get_instance();

    namespace builders
    {
        template <typename T, typename I, typename O, typename E>
        bool run_test(I& inputs, O& outputs, E& expects)
        {
            get_instance().tracker.set_result(true);
            try
            {
                T p;
                p.setup();
                p.run(inputs, outputs, expects);
                p.teardown();
            }
            catch (const std::exception&)
            {
                // exception is thrown
                return false;
            }
            catch (...)
            {
                // unknown exception is thrown
                return false;
            }
            return get_instance().tracker.pop_result();
        }
    }
}
