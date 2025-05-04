#pragma once

#include "../IFrame.hpp"

#include <nil/xit/test/App.hpp>

#include <filesystem>
#include <string>
#include <type_traits>
#include <vector>

namespace nil::xit::gtest::builders::main
{
    template <typename Converter>
        requires std::is_invocable_v<Converter, std::vector<std::string>>
    struct Frame final: IFrame
    {
        explicit Frame(std::filesystem::path init_file, Converter init_converter)
            : IFrame()
            , file(std::move(init_file))
            , converter(std::move(init_converter))
        {
        }

        void install(test::App& app, const std::filesystem::path& path) override
        {
            app.add_main(path / file, converter);
        }

        std::filesystem::path file;
        Converter converter;
    };
}
