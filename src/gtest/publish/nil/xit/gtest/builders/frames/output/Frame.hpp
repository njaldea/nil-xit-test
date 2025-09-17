#pragma once

#include "../../../utils/from_self.hpp"
#include "../IFrame.hpp"

#include <nil/xit/buffer_type.hpp>
#include <nil/xit/test/App.hpp>
#include <nil/xit/test/frame/output/Info.hpp>

namespace nil::xit::gtest::builders::output
{
    template <typename T>
    class Frame: public builders::IFrame
    {
    public:
        using type = T;

        template <test::frame::output::is_valid_value_accessor<T> Accessor>
        Frame<T>& value(std::string id, Accessor accessor)
        {
            using accessor_return_t = std::remove_cvref_t<decltype(accessor(std::declval<T&>()))>;
            values.emplace_back(                                     //
                [id = std::move(id), accessor = std::move(accessor)] //
                (test::frame::output::Info<T> & info)
                { info.template add_value<accessor_return_t>(id, accessor); }
            );
            return *this;
        }

        template <std::same_as<T> Z, typename U>
        Frame<T>& value(std::string id, U Z::*member)
        {
            return value(
                std::move(id),
                [member](const Z& value) -> const U& { return value.*member; }
            );
        }

        Frame<T>& value(std::string id)
        {
            return value(std::move(id), from_self<T>());
        }

    protected:
        // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
        std::vector<std::function<void(nil::xit::test::frame::output::Info<T>&)>> values;
    };

    template <typename T>
    class OutputFrame final: public Frame<T>
    {
    public:
        OutputFrame(std::string init_id, std::optional<std::string> init_path)
            : id(std::move(init_id))
            , path(std::move(init_path))
        {
        }

        void install(test::App& app) override
        {
            auto* frame = path.has_value() //
                ? app.add_output<T>(id, path.value())
                : app.add_output<T>(id);
            if (this->values.empty())
            {
                if constexpr (nil::xit::is_built_in_value<T> || nil::xit::has_codec<T>)
                {
                    frame->template add_value<T>("value", from_self<T>());
                }
            }
            else
            {
                for (const auto& value_installer : this->values)
                {
                    value_installer(*frame);
                }
            }
        }

    private:
        std::string id;
        std::optional<std::string> path;
    };
}
