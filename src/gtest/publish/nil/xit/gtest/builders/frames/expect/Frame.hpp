#pragma once

#include "../../../headless/CacheManager.hpp"
#include "../IFrame.hpp"

#include <nil/xit/tagged/add_signal.hpp>
#include <nil/xit/test/App.hpp>
#include <nil/xit/test/frame/expect/Info.hpp>

namespace nil::xit::gtest::builders::expect
{
    struct IFrame: builders::IFrame
    {
        using builders::IFrame::install;
        virtual void install(headless::CacheManager& cache_manager) = 0;
    };

    template <typename T>
    struct Frame: IFrame
    {
        using type = T;
    };

    template <typename T>
    class ExpectFrame final: public Frame<T>
    {
    public:
        using loader_creator_t
            = std::function<std::unique_ptr<xit::test::frame::IDataManager<T, std::string_view>>()>;

        ExpectFrame(
            std::string init_id,
            std::optional<std::string> init_path,
            loader_creator_t init_loader_creator
        )
            : id(std::move(init_id))
            , path(std::move(init_path))
            , loader_creator(std::move(init_loader_creator))
        {
        }

        void install(test::App& app) override
        {
            auto* frame = path.has_value() //
                ? app.add_expect<T>(loader_creator(), id, path.value())
                : app.add_expect<T>(loader_creator(), id);

            xit::tagged::add_signal(
                *frame->frame,
                "finalize",
                [frame](std::string_view tag) { frame->finalize(tag); }
            );
        }

        void install(headless::CacheManager& cache_manager) override
        {
            struct Cache: headless::Cache<T>
            {
                explicit Cache(std::unique_ptr<xit::test::frame::IDataManager<T, std::string_view>>
                                   init_manager)
                    : manager(std::move(init_manager))
                {
                }

                T get(std::string_view tag) const override
                {
                    return manager->initialize(tag);
                }

                std::unique_ptr<xit::test::frame::IDataManager<T, std::string_view>> manager;
            };

            cache_manager.values.emplace(id, std::make_unique<Cache>(loader_creator()));
        }

    private:
        std::string id;
        std::optional<std::string> path;
        loader_creator_t loader_creator;
    };
}
