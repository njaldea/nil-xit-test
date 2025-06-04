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

        Frame(std::string init_id, std::optional<FileInfo> init_file_info)
            : IFrame()
            , id(std::move(init_id))
            , file_info(std::move(init_file_info))
        {
        }

        void install(test::App& app) override
        {
            auto* frame = file_info.has_value() ? app.add_output<T>(id, file_info.value())
                                                : app.add_output<T>(id);
            for (const auto& value_installer : values)
            {
                value_installer(*frame);
            }
        }

        template <test::frame::output::is_valid_value_accessor<T> Accessor>
        Frame<T>& value(std::string value_id, Accessor accessor)
        {
            using accessor_return_t = std::remove_cvref_t<decltype(accessor(std::declval<T&>()))>;
            values.emplace_back(                                                 //
                [value_id = std::move(value_id), accessor = std::move(accessor)] //
                (test::frame::output::Info<T> & info)
                { info.template add_value<accessor_return_t>(value_id, accessor); }
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
        std::optional<FileInfo> file_info;
        std::vector<std::function<void(nil::xit::test::frame::output::Info<T>&)>> values;
    };
}
