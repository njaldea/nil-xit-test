#pragma once

#include <filesystem>

namespace nil::xit::test
{
    class App;
}

namespace nil::xit::gtest::builders
{
    struct IFrame
    {
        virtual ~IFrame() = default;
        IFrame() = default;
        IFrame(IFrame&&) = delete;
        IFrame(const IFrame&) = delete;
        IFrame& operator=(IFrame&&) = delete;
        IFrame& operator=(const IFrame&) = delete;
        virtual void install(test::App& app, const std::filesystem::path& path) = 0;
    };

    template <typename Accessor, typename T>
    concept is_compatible_accessor = requires(const Accessor& accessor) {
        {
            accessor.set(
                std::declval<T&>(),
                std::declval<decltype(accessor.get(std::declval<const T&>()))>()
            )
        } -> std::same_as<void>;
    };
    template <typename Getter, typename Setter, typename T>
    concept is_compatible_getter_setter = requires(Getter getter, Setter setter) {
        {
            setter(std::declval<T&>(), std::declval<decltype(getter(std::declval<const T&>()))>())
        } -> std::same_as<void>;
    };

}
