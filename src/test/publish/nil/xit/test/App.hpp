#pragma once

#include "frame/input/Tagged.hpp"
#include "frame/input/Unique.hpp"
#include "frame/output/Info.hpp"

#include <nil/service/structs.hpp>
#include <nil/xit/add_frame.hpp>
#include <nil/xit/structs.hpp>
#include <nil/xit/tagged/on_load.hpp>
#include <nil/xit/tagged/on_sub.hpp>
#include <nil/xit/unique/add_signal.hpp>

#include <filesystem>
#include <string_view>

namespace nil::xit::test
{
    template <typename T>
    using flag_t = bool;

    class App
    {
    public:
        App(nil::service::S service, std::string_view app_name);
        App(nil::service::HTTPService& service, std::string_view app_name);

        ~App() noexcept = default;
        App(App&&) = delete;
        App(const App&) = delete;
        App& operator=(App&&) = delete;
        App& operator=(const App&) = delete;

        const std::vector<std::string>& installed_tags() const;
        const std::vector<std::string>& installed_tag_inputs(std::string_view tag) const;
        const std::vector<std::string>& installed_tag_outputs(std::string_view tag) const;

        void add_info(
            std::string_view tag,
            std::vector<std::string> inputs,
            std::vector<std::string> outputs
        )
        {
            tags.emplace_back(tag);
            tag_inputs.emplace(tag, std::move(inputs));
            tag_outputs.emplace(tag, std::move(outputs));
        }

        template <typename FromVS>
            requires requires(FromVS converter) {
                { converter(std::declval<std::vector<std::string>>()) };
            }
        void add_main(const std::filesystem::path& path, FromVS converter)
        {
            {
                auto& f = add_unique_frame(xit, "index", path);
                add_value(f, "tags", [=, this]() { return converter(installed_tags()); });
                add_signal(
                    f,
                    "finalize",
                    [this](std::string_view tag) { return finalize_inputs(tag); }
                );
            }
            {
                auto& f = add_tagged_frame(xit, "frame_info");
                add_value(
                    f,
                    "inputs",
                    [=, this](std::string_view tag) //
                    { return converter(installed_tag_inputs(tag)); }
                );
                add_value(
                    f,
                    "outputs",
                    [=, this](std::string_view tag)
                    { return converter(installed_tag_outputs(tag)); }
                );
            }
        }

        template <typename T>
        frame::input::tagged::Info<T>* add_tagged_input(
            std::string id,
            std::filesystem::path path,
            std::unique_ptr<typename frame::input::tagged::Info<T>::IDataManager> manager
        )
        {
            auto* s = make_frame<frame::input::tagged::Info<T>>(id, input_frames);
            s->frame = &add_tagged_frame(xit, std::move(id), std::move(path));
            s->gate = &gate;
            s->manager = std::move(manager);
            return s;
        }

        template <typename T>
        frame::input::unique::Info<T>* add_unique_input(
            std::string id,
            std::filesystem::path path,
            std::unique_ptr<typename frame::input::unique::Info<T>::IDataManager> manager
        )
        {
            auto* s = make_frame<frame::input::unique::Info<T>>(id, input_frames);
            s->frame = &add_unique_frame(xit, std::move(id), std::move(path));
            s->gate = &gate;
            s->manager = std::move(manager);
            return s;
        }

        template <typename T>
        frame::output::Info<T>* add_output(std::string id, std::filesystem::path path)
        {
            auto* s = make_frame<frame::output::Info<T>>(id, output_frames);
            s->frame = &add_tagged_frame(xit, std::move(id), std::move(path));
            on_load(
                *s->frame,
                [s, g = &this->gate](std::string_view tag)
                {
                    if (const auto it = s->rerun.find(tag); it != s->rerun.end())
                    {
                        it->second->set_value({});
                        g->commit();
                    }
                }
            );
            on_sub(
                *s->frame,
                [s, g = &this->gate](std::string_view tag, std::size_t count)
                {
                    if (const auto it = s->requested.find(tag); it != s->requested.end())
                    {
                        it->second->set_value(count > 0);
                    }
                }
            );
            return s;
        }

        template <typename Callable, typename... Inputs, typename... Outputs>
        void add_node(
            std::string_view tag,
            Callable callable,
            std::tuple<Inputs...> inputs,
            std::tuple<Outputs...> outputs
        )
        {
            constexpr auto input_size = sizeof...(Inputs);
            constexpr auto output_size = sizeof...(Outputs);
            auto output_enablers = [&]<std::size_t... I>(std::index_sequence<I...>)
            {
                return std::make_tuple(([&](){
                    auto* output = std::get<I>(outputs);
                    auto* output_enabler_edge = gate.edge(false);
                    output->requested.emplace(tag, output_enabler_edge).first;
                    return output_enabler_edge;
                })()...);
            }(std::make_index_sequence<output_size>());

            auto [is_enabled] = gate.node(
                [](flag_t<Outputs>... flags) { return (flags || ...); },
                output_enablers
            );
            if constexpr (output_size > 0 && input_size > 0)
            {
                auto gate_outputs = gate.node(
                    std::move(callable),
                    std::apply(
                        [&](auto*... i)
                        { return std::make_tuple(is_enabled, i->get_input(tag)...); },
                        inputs
                    )
                );
                [&]<std::size_t... I>(std::index_sequence<I...>)
                {
                    (([&](){
                        auto* output = std::get<I>(outputs);
                        using info_t = std::remove_cvref_t<decltype(*output)>;
                        using output_t = info_t::type;
                        const auto& [key, rerun]
                            = *output->rerun.emplace(tag, gate.edge(RerunTag())).first;
                        gate.node(
                            [output, t = &key](RerunTag, const output_t& output_data)
                            {
                                for (const auto& value : output->values)
                                {
                                    value(*t, output_data);
                                }
                            },
                            {rerun, get<I>(gate_outputs)}
                        );
                    })(), ...);
                }(std::make_index_sequence<sizeof...(Outputs)>());
            }
            else if constexpr (output_size > 0 && input_size == 0)
            {
                auto gate_outputs = gate.node(std::move(callable), {is_enabled});
                [&]<std::size_t... I>(std::index_sequence<I...>)
                {
                    (([&](){
                        auto* output = std::get<I>(outputs);
                        using info_t = std::remove_cvref_t<decltype(*output)>;
                        using output_t = info_t::type;
                        const auto& [key, rerun]
                            = *output->rerun.emplace(tag, gate.edge(RerunTag())).first;
                        gate.node(
                            [output, t = &key](RerunTag, const output_t& output_data)
                            {
                                for (const auto& value : output->values)
                                {
                                    value(*t, output_data);
                                }
                            },
                            {rerun, get<I>(gate_outputs)}
                        );
                    })(), ...);
                }(std::make_index_sequence<sizeof...(Outputs)>());
            }
            else if constexpr (output_size == 0 && input_size > 0)
            {
                gate.node(
                    std::move(callable),
                    std::apply(
                        [&](auto*... i)
                        { return std::make_tuple(is_enabled, i->get_input(tag)...); },
                        inputs
                    )
                );
            }
            else if constexpr (output_size == 0 && input_size == 0)
            {
                gate.node(std::move(callable), {is_enabled});
            }
        }

        template <typename T>
        frame::input::Info<T>* get_input(std::string_view id) const
        {
            if (auto it = input_frames.find(id); it != input_frames.end())
            {
                return static_cast<frame::input::Info<T>*>(it->second.get());
            }
            return nullptr;
        }

        template <typename T>
        frame::output::Info<T>* get_output(std::string_view id) const
        {
            if (auto it = output_frames.find(id); it != output_frames.end())
            {
                return static_cast<frame::output::Info<T>*>(it->second.get());
            }
            return nullptr;
        }

        void finalize_inputs(std::string_view tag) const;

    private:
        nil::xit::C xit;
        nil::gate::Core gate;

        // clang-format off
        transparent::hash_map<std::unique_ptr<frame::input::IInfo>> input_frames;   // frame_id to info
        transparent::hash_map<std::unique_ptr<frame::output::IInfo>> output_frames; // frame_id to info

        std::vector<std::string> tags;                                              // tags
        transparent::hash_map<std::vector<std::string>> tag_inputs;                 // tag to frame_id
        transparent::hash_map<std::vector<std::string>> tag_outputs;                // tag to frame_id
        std::vector<std::string> blank;
        // clang-format on

        template <typename T>
        T* make_frame(std::string_view id, auto& frames)
        {
            auto t = std::make_unique<T>();
            auto p = t.get();
            frames.emplace(id, std::move(t));
            return p;
        }
    };
}
