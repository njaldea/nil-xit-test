#pragma once

#include "../IFrame.hpp"

#include <nil/xit/test/App.hpp>

#include <string>
#include <type_traits>
#include <vector>

namespace nil::xit::gtest::builders::main
{
    template <typename Converter>
        requires std::is_invocable_v<Converter, std::vector<std::string>>
    struct Frame final: IFrame
    {
        explicit Frame(std::string init_path, Converter init_converter)
            : IFrame()
            , path(std::move(init_path))
            , converter(std::move(init_converter))
        {
        }

        void install(test::App& app) override
        {
            app.add_main(path, converter);
        }

        std::string path;
        Converter converter;
    };
}
