#pragma once

#include "../../../utils/from_data.hpp"
#include "../IFrame.hpp"

#include <nil/xit/tagged/add_option.hpp>
#include <nil/xit/test/App.hpp>
#include <nil/xit/test/frame/view/Test.hpp>

#include <functional>
#include <tuple>
#include <vector>

namespace nil::xit::gtest::builders::view::test
{
    class Frame
    {
    public:
        using type = void;

        template <typename Z>
        Frame& value(std::string value_id, Z unrelated_value)
        {
            values.emplace_back([value_id = std::move(value_id),
                                 seed = from_data<Z>(std::move(unrelated_value)
                                 )](nil::xit::test::frame::view::test::Info& info)
                                { info.template add_value<Z>(value_id, seed.initialize()); });
            return *this;
        }

        Frame& option(std::string key, std::string value);

    protected:
        // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
        std::vector<std::function<void(nil::xit::test::frame::view::test::Info&)>> values;
        // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
        std::vector<std::tuple<std::string, std::string>> options;
    };

    class ViewFrame final
        : public Frame
        , public builders::IFrame
    {
    public:
        ViewFrame(std::string init_id, FileInfo init_file_info);

        void install(nil::xit::test::App& app) override;

    private:
        std::string id;
        FileInfo file_info;
    };
}
