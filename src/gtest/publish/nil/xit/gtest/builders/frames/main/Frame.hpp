// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

#pragma once

#include "../IFrame.hpp"

#include <nil/xit/structs.hpp>
#include <nil/xit/test/App.hpp>
#include <nil/xit/unique/add_option.hpp>

namespace nil::xit::gtest::builders::main
{
    struct Frame
    {
        Frame() = default;
        Frame(Frame&&) = delete;
        Frame(const Frame&) = delete;
        Frame& operator=(Frame&&) = delete;
        Frame& operator=(const Frame&) = delete;
        virtual ~Frame() noexcept = default;

        virtual Frame& option(std::string key, std::string value) = 0;
    };

    struct MainFrame final
        : Frame
        , IFrame
    {
        explicit MainFrame(FileInfo init_file_info);

        void install(test::App& app) override;
        Frame& option(std::string key, std::string value) override;

    private:
        nil::xit::FileInfo file_info;
        std::vector<std::tuple<std::string, std::string>> options;
    };
}
