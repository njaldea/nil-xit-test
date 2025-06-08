#pragma once

#include "frame/input/Global.hpp"
#include "frame/input/Test.hpp"
#include "frame/output/Info.hpp"

#include <nil/service/structs.hpp>
#include <nil/xit/add_frame.hpp>
#include <nil/xit/buffer_type.hpp>
#include <nil/xit/structs.hpp>
#include <nil/xit/tagged/on_load.hpp>
#include <nil/xit/tagged/on_sub.hpp>
#include <nil/xit/unique/add_signal.hpp>

#include <nil/xalt/transparent_stl.hpp>

#include <filesystem>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <utility>

namespace nil::xit::test
{
    class App
    {
    public:
        App(service::P service, std::string_view app_name);

        ~App() noexcept = default;
        App(App&&) = delete;
        App(const App&) = delete;
        App& operator=(App&&) = delete;
        App& operator=(const App&) = delete;

        void set_groups(const xalt::transparent_umap<std::filesystem::path>& paths);

        std::span<const std::string> installed_tags() const;
        // marked
        std::span<const std::string_view> installed_tag_inputs(std::string_view tag) const;
        // marked
        std::span<const std::string_view> installed_tag_outputs(std::string_view tag) const;

        void add_info(
            std::string tag,
            std::vector<std::string_view> inputs,
            std::vector<std::string_view> outputs
        );

        template <typename FromVS>
            requires std::is_invocable_v<FromVS, std::vector<std::string>>
        void add_main(FileInfo file_info, const FromVS& converter)
        {
            if constexpr (!std::is_same_v<
                              decltype(installed_tags()),
                              decltype(converter(installed_tags()))>)
            {
                {
                    auto& frame = add_unique_frame(xit, "index", std::move(file_info));
                    add_value(
                        frame,
                        "tags",
                        [converter, this]() { return converter(installed_tags()); }
                    );
                    add_signal(
                        frame,
                        "finalize",
                        [this](std::string_view tag) { return finalize_inputs(tag); }
                    );
                }
                {
                    auto& frame = add_tagged_frame(xit, "frame_info");
                    add_value(
                        frame,
                        "inputs",
                        [converter, this](std::string_view tag)
                        { return converter(installed_tag_inputs(tag)); }
                    );
                    add_value(
                        frame,
                        "outputs",
                        [converter, this](std::string_view tag)
                        { return converter(installed_tag_outputs(tag)); }
                    );
                }
            }
            else
            {
                add_unique_frame(xit, "index", std::move(file_info));
            }
        }

        template <typename T>
        frame::input::test::Info<T>* add_test_input(
            std::string id,
            FileInfo file_info,
            std::unique_ptr<typename frame::input::test::Info<T>::IDataManager> manager
        )
        {
            auto* s = make_frame<frame::input::test::Info<T>>(id, input_frames);
            s->frame = &add_tagged_frame(xit, std::move(id), std::move(file_info));
            s->gate = &gate;
            s->manager = std::move(manager);
            return s;
        }

        template <typename T>
        frame::input::test::Info<T>* add_test_input(
            std::string id,
            std::unique_ptr<typename frame::input::test::Info<T>::IDataManager> manager
        )
        {
            auto* s = make_frame<frame::input::test::Info<T>>(id, input_frames);
            s->frame = &add_tagged_frame(xit, std::move(id));
            s->gate = &gate;
            s->manager = std::move(manager);
            return s;
        }

        template <typename T>
        frame::input::global::Info<T>* add_global_input(
            std::string id,
            FileInfo file_info,
            std::unique_ptr<typename frame::input::global::Info<T>::IDataManager> manager
        )
        {
            auto* s = make_frame<frame::input::global::Info<T>>(id, input_frames);
            s->frame = &add_unique_frame(xit, std::move(id), std::move(file_info));
            s->gate = &gate;
            s->manager = std::move(manager);
            return s;
        }

        template <typename T>
        frame::input::global::Info<T>* add_global_input(
            std::string id,
            std::unique_ptr<typename frame::input::global::Info<T>::IDataManager> manager
        )
        {
            auto* s = make_frame<frame::input::global::Info<T>>(id, input_frames);
            s->frame = &add_unique_frame(xit, std::move(id));
            s->gate = &gate;
            s->manager = std::move(manager);
            return s;
        }

        template <typename T>
        frame::output::Info<T>* add_output(std::string id, FileInfo file_info)
        {
            auto* s = make_frame<frame::output::Info<T>>(id, output_frames);
            s->frame = &add_tagged_frame(xit, std::move(id), std::move(file_info));
            add_output_detail(s);
            return s;
        }

        template <typename T>
        frame::output::Info<T>* add_output(std::string id)
        {
            auto* s = make_frame<frame::output::Info<T>>(id, output_frames);
            s->frame = &add_tagged_frame(xit, std::move(id));
            add_output_detail(s);
            return s;
        }

        template <typename Callable, typename... Inputs, typename... Outputs>
        void add_node(
            std::string_view tag,
            Callable callable,
            std::tuple<Inputs*...> inputs,
            std::tuple<Outputs*...> outputs
        )
        {
            constexpr auto i_size = sizeof...(Inputs);
            constexpr auto o_size = sizeof...(Outputs);
            constexpr auto i_seq = std::make_index_sequence<i_size>();
            constexpr auto o_seq = std::make_index_sequence<o_size>();
            auto* enabler = add_node_enabler(tag, outputs, o_seq);

            if constexpr (o_size == 0)
            {
            }
            else
            {
                auto wrapped_cb =                     //
                    [cb = std::move(callable), o_seq] //
                    (                                 //
                        const nil::gate::Core& core,
                        nil::gate::async_outputs<typename Outputs::type...> asyncs,
                        bool enabled,
                        const typename Inputs::type&... rest //
                    )
                {
                    if (enabled)
                    {
                        App::for_each(
                            core.batch(asyncs),
                            cb(rest...),
                            o_seq,
                            [](auto* l, auto& r) { l->set_value(std::move(r)); }
                        );
                        core.commit();
                    }
                };

                for_each(
                    outputs,
                    add_node_impl(tag, std::move(wrapped_cb), enabler, inputs, i_seq),
                    o_seq,
                    [&](auto* output, auto* port)
                    {
                        using output_t = std::remove_cvref_t<decltype(*output)>::type;
                        const auto& [key, rerun]
                            = *output->rerun.emplace(tag, gate.port(RerunTag())).first;
                        gate.node(
                            [output, t = std::string_view(key)] //
                            (RerunTag, const output_t& output_data)
                            {
                                for (const auto& value : output->values)
                                {
                                    value(t, output_data);
                                }
                            },
                            {rerun, port}
                        );
                    }
                );
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
        xit::C xit;
        nil::gate::Core gate;

        using frame_id_to_i_info = xalt::transparent_umap<std::unique_ptr<frame::input::IInfo>>;
        using frame_id_to_o_info = xalt::transparent_umap<std::unique_ptr<frame::output::IInfo>>;
        using tag_to_frame_id = xalt::transparent_umap<std::vector<std::string_view>>;

        frame_id_to_i_info input_frames;
        frame_id_to_o_info output_frames;

        std::vector<std::string> tags;
        tag_to_frame_id tag_inputs;
        tag_to_frame_id tag_outputs;

        std::vector<std::string_view> blank;

        template <typename T>
        T* make_frame(std::string_view id, auto& frames)
        {
            auto t = std::make_unique<T>();
            auto p = t.get();
            frames.emplace(id, std::move(t));
            return p;
        }

        template <typename Outputs, std::size_t... I>
        nil::gate::ports::ReadOnly<bool>* add_node_enabler(
            std::string_view tag,
            const Outputs& outputs,
            std::index_sequence<I...> /* seq */
        )
        {
            if constexpr (sizeof...(I) > 0)
            {
                return get<0>(gate.node(
                    [](std::conditional_t<true, bool, decltype(I)>... flags)
                    { return (false || ... || flags); },
                    {get<I>(outputs)->requested.emplace(tag, gate.port(false)).first->second...}
                ));
            }
            else
            {
                // will not run tests that does not have any outputs
                // TODO: include inputs?
                return get<0>(gate.node([]() { return false; }));
            }
        }

        template <typename T, typename Inputs, std::size_t... I>
        auto add_node_impl(
            [[maybe_unused]] std::string_view tag,
            T callable,
            nil::gate::ports::ReadOnly<bool>* is_enabled,
            const Inputs& inputs,
            std::index_sequence<I...> /* seq */
        )
        {
            return gate.node(std::move(callable), {is_enabled, get<I>(inputs)->get_input(tag)...});
        }

        template <typename P, typename L, typename R, std::size_t... I>
        static void for_each(L&& a, R&& b, std::index_sequence<I...> /* seq */, const P& predicate)
        {
            ([&]() { predicate(get<I>(a), get<I>(b)); }(), ...);
        }

        template <typename T>
        void add_output_detail(frame::output::Info<T>* s)
        {
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
                [this, s](std::string_view tag, std::size_t count)
                {
                    if (const auto it = s->requested.find(tag); it != s->requested.end())
                    {
                        it->second->set_value(count > 0);
                    }
                    if (count == 1)
                    {
                        for (const auto& f : this->installed_tag_inputs(tag))
                        {
                            if (const auto it = this->input_frames.find(f.substr(0, f.size() - 4));
                                it != this->input_frames.end())
                            {
                                it->second->initialize(tag);
                            }
                        }
                    }
                }
            );
        }
    };
}
