#pragma once

#include "../IFrame.hpp"

#include <nil/xit/test/App.hpp>

#include <string>

namespace nil::xit::gtest::builders::main
{
    struct Frame final: IFrame
    {
        explicit Frame(std::string init_path)
            : path(std::move(init_path))
        {
        }

        void install(test::App& app) override
        {
            app.add_main(path);
        }

        std::string path;
    };
}
