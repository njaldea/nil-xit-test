#pragma once

#include "frame/input/Tagged.hpp"
#include "frame/input/Unique.hpp"
#include "frame/output/Info.hpp"

#include <nil/service/structs.hpp>
#include <nil/xit/add_frame.hpp>
#include <nil/xit/structs.hpp>

#include <filesystem>
#include <string_view>

namespace nil::xit::test
{
    class App
    {
    public:
        App(nil::service::S& service, std::string_view app_name);
        App(nil::service::HTTPService& service, std::string_view app_name);

        ~App() noexcept = default;
        App(App&&) = delete;
        App(const App&) = delete;
        App& operator=(App&&) = delete;
        App& operator=(const App&) = delete;

        const std::vector<std::string>& installed_tags() const;
        const std::vector<std::string>& installed_frame_inputs(std::string_view tag) const;
        const std::vector<std::string>& installed_frame_outputs(std::string_view tag) const;

        void add_info(
            std::string_view tag,
            std::vector<std::string> inputs,
            std::vector<std::string> outputs
        )
        {
            tags.emplace_back(tag);
            frame_inputs.emplace(tag, std::move(inputs));
            frame_outputs.emplace(tag, std::move(outputs));
        }

        template <typename FromVS>
            requires requires(FromVS converter) {
                { converter(std::declval<std::vector<std::string>>()) };
            }
        void add_main(const std::filesystem::path& path, FromVS converter)
        {
            {
                auto& f = add_unique_frame(xit, "demo", path);
                add_value(f, "tags", [=, this]() { return converter(installed_tags()); });
            }
            {
                auto& f = add_tagged_frame(xit, "frame_info");
                add_value(
                    f,
                    "inputs",
                    [=, this](std::string_view tag)
                    { return converter(installed_frame_inputs(tag)); }
                );
                add_value(
                    f,
                    "outputs",
                    [=, this](std::string_view tag)
                    { return converter(installed_frame_outputs(tag)); }
                );
            }
        }

        template <typename T>
        frame::input::tagged::Info<T>* add_tagged_input(
            std::string id,
            std::filesystem::path path,
            std::function<T(std::string_view)> initializer
        )
        {
            auto* s = make_frame<frame::input::tagged::Info<T>>(id, input_frames);
            s->frame = &add_tagged_frame(xit, std::move(id), std::move(path));
            s->gate = &gate;
            s->initializer = std::move(initializer);
            return s;
        }

        template <typename T>
        frame::input::unique::Info<T>* add_unique_input(
            std::string id,
            std::filesystem::path path,
            std::function<T()> initializer
        )
        {
            auto* s = make_frame<frame::input::unique::Info<T>>(id, input_frames);
            s->frame = &add_unique_frame(xit, std::move(id), std::move(path));
            s->gate = &gate;
            s->initializer = std::move(initializer);
            return s;
        }

        template <typename T>
        frame::output::Info<T>* add_output(std::string id, std::filesystem::path path)
        {
            auto* s = make_frame<frame::output::Info<T>>(id, output_frames);
            s->frame = &add_tagged_frame(
                xit,
                std::move(id),
                std::move(path),
                [s, g = &this->gate](std::string_view tag)
                {
                    if (const auto it = s->rerun.find(tag); it != s->rerun.end())
                    {
                        it->second->set_value({});
                        g->commit();
                    }
                }
            );
            return s;
        }

        template <typename Callable, typename... Inputs, typename... Outputs>
            requires(sizeof...(Outputs) > 0 && sizeof...(Inputs) > 0)
        void add_node(
            std::string_view tag,
            Callable callable,
            std::tuple<Inputs...> inputs,
            std::tuple<Outputs...> outputs
        )
        {
            auto gate_outputs = gate.node(
                std::move(callable),
                std::apply(
                    [&](auto*... i) { return std::make_tuple(i->get_input(tag)...); },
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

    private:
        nil::xit::C xit;
        nil::gate::Core gate;

        transparent::hash_map<std::unique_ptr<frame::IInfo>> input_frames;
        transparent::hash_map<std::unique_ptr<frame::IInfo>> output_frames;

        std::vector<std::string> tags;
        transparent::hash_map<std::vector<std::string>> frame_inputs;
        transparent::hash_map<std::vector<std::string>> frame_outputs;
        std::vector<std::string> blank;

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
