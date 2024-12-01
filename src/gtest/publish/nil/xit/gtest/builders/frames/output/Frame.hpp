#pragma once

#include "../../../utils/from_member.hpp"
#include "../IFrame.hpp"

#include <nil/xit/test/App.hpp>
#include <nil/xit/test/frame/output/Info.hpp>

namespace nil::xit::gtest::builders::output
{
    template <typename T>
    class Frame final: public builders::IFrame
    {
    public:
        using type = T;

        Frame(std::string init_id, std::filesystem::path init_file)
            : IFrame()
            , id(std::move(init_id))
            , file(std::move(init_file))
        {
        }

        void install(test::App& app, const std::filesystem::path& path) override
        {
            auto* frame = app.add_output<T>(id, path / file);
            for (const auto& value_installer : values)
            {
                value_installer(*frame);
            }
        }

        template <test::frame::output::is_valid_value_getter<T> Getter>
        Frame<T>& value(std::string value_id, Getter getter)
        {
            using getter_return_t = std::remove_cvref_t<decltype(getter(std::declval<T&>()))>;
            values.emplace_back(                                             //
                [value_id = std::move(value_id), getter = std::move(getter)] //
                (nil::xit::test::frame::output::Info<T> & info)
                { info.template add_value<getter_return_t>(value_id, getter); }
            );
            return *this;
        }

        template <typename U>
        Frame<T>& value(std::string value_id, U T::*member)
        {
            return value(std::move(value_id), from_member(member));
        }

        Frame<T>& value(std::string value_id)
        {
            return value(std::move(value_id), [](const T& value) -> const T& { return value; });
        }

    private:
        std::string id;
        std::filesystem::path file;
        std::vector<std::function<void(nil::xit::test::frame::output::Info<T>&)>> values;
    };
}
