#pragma once

#include <memory>
#include <utility>

namespace nil::xit::test::frame
{
    template <typename T, typename... Args>
    concept has_update = requires() {
        { T::update(std::declval<Args>()...) };
    } || requires(T loader) {
        { loader.update(std::declval<Args>()...) };
    };

    template <typename T, typename... Args>
    concept has_finalize = requires() {
        { T::finalize(std::declval<Args>()...) };
    } || requires(T loader) {
        { loader.finalize(std::declval<Args>()...) };
    };

    template <typename T, typename... Args>
    struct IDataManager
    {
        IDataManager() = default;
        virtual ~IDataManager() = default;
        IDataManager(IDataManager&&) = delete;
        IDataManager(const IDataManager&) = delete;
        IDataManager& operator=(IDataManager&&) = delete;
        IDataManager& operator=(const IDataManager&) = delete;

        virtual T initialize(Args... tag) const = 0;
        virtual void update(Args... tag, const T& value) const = 0;
        virtual void finalize(Args... tag, const T& value) const = 0;
    };

    template <typename Loader, typename T, typename... Args>
    auto make_data_manager(Loader loader) -> std::unique_ptr<IDataManager<T, Args...>>
    {
        struct DataManager: IDataManager<T, Args...>
        {
            explicit DataManager(Loader init_loader)
                : loader(std::move(init_loader))
            {
            }

            T initialize(Args... args) const override
            {
                return loader.initialize(args...);
            }

            void update(Args... args, const T& value) const override
            {
                if constexpr (has_update<Loader, Args..., T>)
                {
                    loader.update(args..., value);
                }
            }

            void finalize(Args... args, const T& value) const override
            {
                if constexpr (has_finalize<Loader, Args..., T>)
                {
                    loader.finalize(args..., value);
                }
            }

            Loader loader;
        };

        return std::make_unique<DataManager>(std::move(loader));
    }
}
