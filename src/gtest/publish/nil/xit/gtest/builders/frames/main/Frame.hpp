#pragma once

#include "../IFrame.hpp"

#include <nil/xit/structs.hpp>
#include <nil/xit/test/App.hpp>
#include <nil/xit/unique/add_option.hpp>

namespace nil::xit::gtest::builders::main
{
    struct Frame
    {
        Frame& option(std::string key, std::string value)
        {
            options.emplace_back(std::move(key), std::move(value));
            return *this;
        }

    protected:
        // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
        std::vector<std::tuple<std::string, std::string>> options;
    };

    struct MainFrame final
        : Frame
        , IFrame
    {
        explicit MainFrame(FileInfo init_file_info)
            : file_info(std::move(init_file_info))
        {
        }

        void install(test::App& app) override
        {
            auto& frame = app.add_main(file_info);

            for (const auto& [key, value] : this->options)
            {
                add_option(frame, key, value);
            }
        }

        nil::xit::FileInfo file_info;
    };
}
