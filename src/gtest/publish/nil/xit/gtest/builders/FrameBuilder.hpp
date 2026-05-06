#pragma once

#include "frames/IFrame.hpp"
#include "frames/expect/Frame.hpp"
#include "frames/input/global.hpp"
#include "frames/input/test.hpp"
#include "frames/output/Frame.hpp"

#include <nil/xit/test/frame/IDataManager.hpp>

#include "../utils/from_data.hpp"

#include <string_view>
#include <type_traits>
#include <vector>

namespace nil::xit::gtest::builders
{
    struct none
    {
    };

    template <typename T>
    concept is_loader_unique = requires() {
        { T::initialize() };
    } || requires(T loader) {
        { loader.initialize() };
    };

    template <typename T>
    concept is_loader_tagged = requires() {
        { T::initialize(std::declval<std::string_view>()) };
    } || requires(T loader) {
        { loader.initialize(std::declval<std::string_view>()) };
    };

    class FrameBuilder final
    {
    public:
        FrameBuilder();

        template <typename Loader>
            requires(!is_loader_tagged<decltype(std::declval<Loader>()())>)
        auto& create_test_input(std::string id, std::optional<FileInfo> file_info, Loader loader)
        {
            return create_test_input(
                std::move(id),
                std::move(file_info),
                [loader = std::move(loader)]() { return from_data(loader()); }
            );
        }

        template <typename Loader>
            requires(is_loader_tagged<decltype(std::declval<Loader>()())>)
        auto& create_test_input(std::string id, std::optional<FileInfo> file_info, Loader loader)
        {
            using loader_t = std::remove_cvref_t<decltype(loader())>;
            using type = std::remove_cvref_t<decltype(loader().initialize(std::string_view()))>;

            auto frame = std::make_unique<input::test::InputFrame<type>>(
                std::move(id),
                std::move(file_info),
                [loader = std::move(loader)]()
                {
                    using test::frame::make_data_manager;
                    return make_data_manager<loader_t, type, std::string_view>(loader());
                }
            );
            input::test::Frame<type>* ptr = frame.get();
            input_frames.emplace_back(std::move(frame));
            return *ptr;
        }

        auto& create_test_input(std::string id, std::optional<FileInfo> file_info)
        {
            return create_test_input(std::move(id), std::move(file_info), []() { return none(); });
        }

        template <typename Loader>
            requires(!is_loader_unique<decltype(std::declval<Loader>()())>)
        auto& create_global_input(std::string id, std::optional<FileInfo> file_info, Loader loader)
        {
            return create_global_input(
                std::move(id),
                std::move(file_info),
                [loader = std::move(loader)]() { return from_data(loader()); }
            );
        }

        template <typename Loader>
            requires(is_loader_unique<decltype(std::declval<Loader>()())>)
        auto& create_global_input(std::string id, std::optional<FileInfo> file_info, Loader loader)
        {
            using loader_t = std::remove_cvref_t<decltype(loader())>;
            using type = std::remove_cvref_t<decltype(loader().initialize())>;

            auto frame = std::make_unique<input::global::InputFrame<type>>(
                std::move(id),
                std::move(file_info),
                [loader = std::move(loader)]()
                {
                    using test::frame::make_data_manager;
                    return make_data_manager<loader_t, type>(loader());
                }
            );
            input::global::Frame<type>* ptr = frame.get();
            input_frames.emplace_back(std::move(frame));
            return *ptr;
        }

        auto& create_global_input(std::string id, std::optional<FileInfo> file_info)
        {
            return create_global_input(
                std::move(id),
                std::move(file_info),
                []() { return none(); }
            );
        }

        template <typename Loader>
            requires(!is_loader_tagged<decltype(std::declval<Loader>()())>)
        auto& create_expect(std::string id, std::optional<FileInfo> file_info, Loader loader)
        {
            return create_expect(
                std::move(id),
                std::move(file_info),
                [loader = std::move(loader)]() { return from_data(loader()); }
            );
        }

        template <typename Loader>
            requires(is_loader_tagged<decltype(std::declval<Loader>()())>)
        auto& create_expect(std::string id, std::optional<FileInfo> file_info, Loader loader)
        {
            using loader_t = std::remove_cvref_t<decltype(loader())>;
            using type = std::remove_cvref_t<decltype(loader().initialize(std::string_view()))>;

            auto frame = std::make_unique<expect::ExpectFrame<type>>(
                std::move(id),
                std::move(file_info),
                [loader = std::move(loader)]()
                {
                    using test::frame::make_data_manager;
                    return make_data_manager<loader_t, type, std::string_view>(loader());
                }
            );
            expect::Frame<type>* ptr = frame.get();
            expect_frames.emplace_back(std::move(frame));
            return *ptr;
        }

        template <typename T>
        auto& create_output(std::string id, std::optional<FileInfo> file_info)
        {
            using type = std::remove_cvref_t<T>;
            auto frame
                = std::make_unique<output::OutputFrame<type>>(std::move(id), std::move(file_info));
            output::Frame<type>* ptr = frame.get();
            output_frames.emplace_back(std::move(frame));
            return *ptr;
        }

        void install(test::App& app) const;
        void install(headless::CacheManager& cache_manager) const;

    private:
        std::vector<std::unique_ptr<input::IFrame>> input_frames;
        std::vector<std::unique_ptr<expect::IFrame>> expect_frames;
        std::vector<std::unique_ptr<IFrame>> output_frames;
    };
}

namespace nil::service
{
    template <>
    struct codec<xit::gtest::builders::none>
    {
        static std::size_t size(const xit::gtest::builders::none& message)
        {
            (void)message;
            return 0;
        }

        static std::size_t serialize(void* output, const xit::gtest::builders::none& data)
        {
            (void)output;
            (void)data;
            return 0;
        }

        static xit::gtest::builders::none deserialize(const void* input, std::uint64_t size)
        {
            (void)input;
            (void)size;
            return {};
        }
    };
}
