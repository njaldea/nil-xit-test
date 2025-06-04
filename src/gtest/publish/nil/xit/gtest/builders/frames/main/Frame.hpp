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
        explicit Frame(FileInfo init_file_info, Converter init_converter)
            : IFrame()
            , file_info(std::move(init_file_info))
            , converter(std::move(init_converter))
        {
        }

        void install(test::App& app) override
        {
            app.add_main(file_info, converter);
        }

        FileInfo file_info;
        Converter converter;
    };
}
