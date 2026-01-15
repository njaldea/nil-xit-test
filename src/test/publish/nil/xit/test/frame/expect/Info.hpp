#pragma once

#include <nil/gate/Core.hpp>

#include <nil/xit/tagged/structs.hpp>

#include "../../SingleFire.hpp"
#include "../../utils.hpp"
#include "../IDataManager.hpp"

#include <memory>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <unordered_map>

namespace nil::xit::test::frame::expect
{
    struct IInfo
    {
        IInfo() = default;
        IInfo(IInfo&&) = delete;
        IInfo(const IInfo&) = delete;
        IInfo& operator=(IInfo&&) = delete;
        IInfo& operator=(const IInfo&) = delete;
        virtual ~IInfo() = default;
        virtual void initialize(std::string_view tag) = 0;
        virtual void finalize(std::string_view tag) const = 0;
    };

    template <typename Accessor, typename T>
    concept is_valid_value_accessor //
        = std::is_lvalue_reference_v<utils::return_t<Accessor, T>>
        && !std::is_const_v<std::remove_reference_t<utils::return_t<Accessor, T>>>;

    template <typename T>
    struct Info final: IInfo
    {
        using type = T;

        struct Entry
        {
            bool is_initialized = false;
            nil::gate::ports::External<T>* input = nullptr;
            nil::gate::ports::External<bool>* enabler = nullptr;
            nil::gate::ports::External<SingleFire>* single_fire = nullptr;
            nil::gate::ports::External<bool>* requested = nullptr;
        };

        nil::xit::tagged::Frame* frame = nullptr;
        nil::gate::Core* gate = nullptr;
        std::unique_ptr<IDataManager<T, std::string_view>> manager;
        std::unordered_map<std::string_view, Entry> info; // sv owned by tags from App

        nil::gate::ports::External<T>* get_port(std::string_view tag)
        {
            if (const auto it = info.find(tag); it != info.end())
            {
                return it->second.input;
            }
            throw std::runtime_error("should be unreachable");
        }

        void initialize(std::string_view tag) override
        {
            if (auto it = info.find(tag); it != info.end())
            {
                if (auto& entry = it->second; !entry.is_initialized && entry.input != nullptr)
                {
                    entry.is_initialized = true;
                    entry.input->set_value(manager->initialize(tag));
                    gate->commit();
                }
            }
        }

        void finalize(std::string_view tag) const override
        {
            if (auto it = info.find(tag); it != info.end())
            {
                it->second.single_fire->set_value(SingleFire{.value = true});
                gate->commit();
            }
        }

        void finalize(std::string_view tag, const T& data) const
        {
            if (auto it = info.find(tag); it != info.end())
            {
                manager->finalize(tag, data);
                it->second.input->set_value(data);
                // TODO: figure out how to do auto loop back when updated
                // if (should_refire?) {
                //     it->second.single_fire->set_value(SingleFire{.value = true});
                // }
                gate->commit();
            }
        }

        void add_info(std::string_view tag)
        {
            gate->post(
                [tag, this](gate::Graph& graph)
                {
                    info.emplace(
                        tag,
                        Entry{
                            .is_initialized = false,
                            .input = graph.port<T>(),
                            .enabler = graph.port<bool>(),
                            .single_fire = graph.port(SingleFire{.value = false}),
                            .requested = graph.port(false)
                        }
                    );
                }
            );
        }

        gate::ports::External<bool>* info_requested(std::string_view tag) const
        {
            if (const auto it = info.find(tag); info.end() != it)
            {
                return it->second.requested;
            }
            return nullptr;
        }

        gate::ports::External<SingleFire>* info_single_fire(std::string_view tag) const
        {
            if (const auto it = info.find(tag); info.end() != it)
            {
                return it->second.single_fire;
            }
            return nullptr;
        }
    };
}
