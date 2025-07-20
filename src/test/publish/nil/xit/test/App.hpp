#pragma once

#include "frame/expect/Info.hpp"
#include "frame/input/Global.hpp"
#include "frame/input/Test.hpp"
#include "frame/output/Info.hpp"

#include <nil/gate/traits/compare.hpp>
#include <nil/service/structs.hpp>
#include <nil/xit/add_frame.hpp>
#include <nil/xit/buffer_type.hpp>
#include <nil/xit/structs.hpp>
#include <nil/xit/tagged/on_load.hpp>
#include <nil/xit/tagged/on_sub.hpp>
#include <nil/xit/unique/add_signal.hpp>

#include <nil/xalt/transparent_stl.hpp>

#include <filesystem>
#include <set>
#include <string_view>
#include <type_traits>
#include <utility>

template <typename T>
    requires(!requires() { std::declval<T>() == std::declval<T>(); })
struct nil::gate::traits::compare<T>
{
    static bool match(const T& /* l */, const T& /* r */)
    {
        return true;
    }
};

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

        std::span<const std::string_view> installed_tags() const;
        // marked
        std::span<const std::string_view> installed_tag_inputs(std::string_view tag) const;
        // marked
        std::span<const std::string_view> installed_tag_outputs(std::string_view tag) const;
        // marked
        std::span<const std::string_view> installed_tag_expects(std::string_view tag) const;

        std::string_view add_info(
            std::string tag,
            std::vector<std::string_view> inputs,
            std::vector<std::string_view> outputs,
            std::vector<std::string_view> expects
        );

        void add_main(std::string_view path)
        {
            static constexpr auto converter = [](std::span<const std::string_view> ids)
            {
                std::ostringstream oss;
                const auto size = ids.size();
                auto i = 0UL;
                for (const auto& id : ids)
                {
                    oss << id;
                    i += 1;
                    if (i < size)
                    {
                        oss << ',';
                    }
                }
                return oss.str();
            };

            {
                auto& frame = add_unique_frame(xit, "index", std::string(path));
                add_value(frame, "tags", [this]() { return converter(installed_tags()); });
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
                    [this](std::string_view tag) { return converter(installed_tag_inputs(tag)); }
                );
                add_value(
                    frame,
                    "outputs",
                    [this](std::string_view tag) { return converter(installed_tag_outputs(tag)); }
                );
                add_value(
                    frame,
                    "expects",
                    [this](std::string_view tag) { return converter(installed_tag_expects(tag)); }
                );
            }
        }

        template <typename T>
        frame::input::test::Info<T>* add_test_input(
            std::unique_ptr<frame::IDataManager<T, std::string_view>> manager,
            std::string_view id
        )
        {
            auto p = std::make_unique<frame::input::test::Info<T>>();
            auto* s = p.get();
            input_frames.emplace(*frames.emplace(id).first, std::move(p));

            s->frame = &add_tagged_frame(xit, std::string(id));
            s->gate = &gate;
            s->manager = std::move(manager);
            return s;
        }

        template <typename T>
        frame::input::test::Info<T>* add_test_input(
            std::unique_ptr<frame::IDataManager<T, std::string_view>> manager,
            std::string_view id,
            std::string_view path
        )
        {
            auto p = std::make_unique<frame::input::test::Info<T>>();
            auto* s = p.get();
            input_frames.emplace(*frames.emplace(id).first, std::move(p));

            s->frame = &add_tagged_frame(xit, std::string(id), std::string(path));
            s->gate = &gate;
            s->manager = std::move(manager);
            return s;
        }

        template <typename T>
        frame::input::global::Info<T>* add_global_input(
            std::unique_ptr<frame::IDataManager<T>> manager,
            std::string_view id
        )
        {
            auto p = std::make_unique<frame::input::global::Info<T>>();
            auto* s = p.get();
            input_frames.emplace(*frames.emplace(id).first, std::move(p));

            s->frame = &add_unique_frame(xit, std::string(id));
            s->gate = &gate;
            s->manager = std::move(manager);
            return s;
        }

        template <typename T>
        frame::input::global::Info<T>* add_global_input(
            std::unique_ptr<frame::IDataManager<T>> manager,
            std::string_view id,
            std::string_view path
        )
        {
            auto p = std::make_unique<frame::input::global::Info<T>>();
            auto* s = p.get();
            input_frames.emplace(*frames.emplace(id).first, std::move(p));

            s->frame = &add_unique_frame(xit, std::string(id), std::string(path));
            s->gate = &gate;
            s->manager = std::move(manager);
            return s;
        }

        template <typename T>
        frame::expect::Info<T>* add_expect(
            std::unique_ptr<frame::IDataManager<T, std::string_view>> manager,
            std::string_view id,
            std::string_view path
        )
        {
            auto p = std::make_unique<frame::expect::Info<T>>();
            auto* s = p.get();
            expect_frames.emplace(*frames.emplace(id).first, std::move(p));

            s->frame = &add_tagged_frame(xit, std::string(id), std::string(path));
            s->gate = &gate;
            s->manager = std::move(manager);
            add_info_on_sub(s);
            return s;
        }

        template <typename T>
        frame::expect::Info<T>* add_expect(
            std::unique_ptr<frame::IDataManager<T, std::string_view>> manager,
            std::string_view id
        )
        {
            auto p = std::make_unique<frame::expect::Info<T>>();
            auto* s = p.get();
            expect_frames.emplace(*frames.emplace(id).first, std::move(p));

            s->frame = &add_tagged_frame(xit, std::string(id));
            s->gate = &gate;
            s->manager = std::move(manager);
            add_info_on_sub(s);
            return s;
        }

        template <typename T>
        frame::output::Info<T>* add_output(std::string_view id)
        {
            auto p = std::make_unique<frame::output::Info<T>>();
            auto* s = p.get();
            output_frames.emplace(*frames.emplace(id).first, std::move(p));

            s->frame = &add_tagged_frame(xit, std::string(id));
            s->gate = &gate;
            add_info_on_load(s);
            add_info_on_sub(s);
            return s;
        }

        template <typename T>
        frame::output::Info<T>* add_output(std::string_view id, std::string_view path)
        {
            auto p = std::make_unique<frame::output::Info<T>>();
            auto* s = p.get();
            output_frames.emplace(*frames.emplace(id).first, std::move(p));

            s->frame = &add_tagged_frame(xit, std::string(id), std::string(path));
            s->gate = &gate;
            add_info_on_load(s);
            add_info_on_sub(s);
            return s;
        }

        template <typename Callable, typename... Inputs, typename... Outputs, typename... Expects>
        void add_node(
            std::string_view tag,
            Callable callable,
            std::tuple<Inputs*...> inputs,
            std::tuple<Outputs*...> outputs,
            std::tuple<Expects*...> expects
        )
        {
            constexpr auto i_size = sizeof...(Inputs);
            constexpr auto o_size = sizeof...(Outputs);
            constexpr auto e_size = sizeof...(Expects);
            constexpr auto i_seq = std::make_index_sequence<i_size>();
            constexpr auto o_seq = std::make_index_sequence<o_size>();
            constexpr auto e_seq = std::make_index_sequence<e_size>();

            std::apply([&](const auto&... frame) { (frame->add_info(tag), ...); }, inputs);
            std::apply([&](const auto&... frame) { (frame->add_info(tag), ...); }, outputs);
            std::apply([&](const auto&... frame) { (frame->add_info(tag), ...); }, expects);

            if constexpr ((o_size + e_size) > 0)
            {
                auto wrapped_cb =              //
                    [cb = std::move(callable)] //
                    (                          //
                        const nil::gate::Core& core,
                        nil::gate::async_outputs<
                            typename Outputs::type...,
                            typename Expects::type...> asyncs,
                        bool enabled,
                        const typename Inputs::type&... rest_i, //
                        const typename Expects::type&... rest_e //
                    )
                {
                    if (enabled)
                    {
                        [&]<std::size_t... X>(std::index_sequence<X...>)
                        {
                            auto batch = core.batch(asyncs);
                            auto result = cb(rest_i..., rest_e...);
                            (get<X>(batch)->set_value(std::move(get<X>(result))), ...);
                            core.commit();
                        }(std::make_index_sequence<o_size + e_size>());
                    }
                };

                auto result = add_node_impl(
                    tag,
                    std::move(wrapped_cb),
                    add_node_enabler(tag, outputs, o_seq, expects, e_seq),
                    inputs,
                    expects,
                    i_seq,
                    e_seq
                );

                // iterate the output of the test and then map it with rerun tag
                // and see if it will need to pass the new value to the UI
                [&]<std::size_t... OI>(std::index_sequence<OI...>)
                {
                    (
                        [&](auto* output, auto port)
                        {
                            using output_t = std::remove_cvref_t<decltype(*output)>::type;
                            gate.node(
                                [output, tag](RerunTag, const output_t& output_data)
                                { output->post(tag, output_data); },
                                {output->info_rerun(tag), port}
                            );
                        }(std::get<OI>(outputs), get<OI>(result)),
                        ...
                    );
                }(o_seq);

                // iterate the expected of the test and then map it with rerun tag
                // and see if it will need to pass the new value to the UI
                [&]<std::size_t... EI>(std::index_sequence<EI...>)
                {
                    (
                        [&](auto* expected, auto port)
                        {
                            using expected_t = std::remove_cvref_t<decltype(*expected)>::type;
                            gate.node(
                                [expected, tag] //
                                (const SingleFire& flag, const expected_t& expected_data)
                                {
                                    if (flag.pop())
                                    {
                                        expected->finalize(tag, expected_data);
                                    }
                                },
                                {expected->info_single_fire(tag), port}
                            );
                        }(std::get<EI + sizeof...(Outputs)>(expects),
                          get<EI + sizeof...(Outputs)>(result)),
                        ...
                    );
                }(e_seq);
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
        frame::expect::Info<T>* get_expect(std::string_view id) const
        {
            if (auto it = expect_frames.find(id); it != expect_frames.end())
            {
                return static_cast<frame::expect::Info<T>*>(it->second.get());
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
        xit::C xit;
        nil::gate::Core gate;

        std::set<std::string> frames;
        // frame id to frame info - sv owned by frames
        std::unordered_map<std::string_view, std::unique_ptr<frame::input::IInfo>> input_frames;
        std::unordered_map<std::string_view, std::unique_ptr<frame::output::IInfo>> output_frames;
        std::unordered_map<std::string_view, std::unique_ptr<frame::expect::IInfo>> expect_frames;

        struct TagInfo
        {
            std::vector<std::string_view> inputs;
            std::vector<std::string_view> outputs;
            std::vector<std::string_view> expects;
        };

        std::set<std::string> tags;
        std::vector<std::string_view> tags_view;
        // tag id to tag info - sv owned by tags
        std::unordered_map<std::string_view, TagInfo> tag_info;

        void finalize_inputs(std::string_view tag) const;

        template <typename Outputs, std::size_t... O, typename Expects, std::size_t... E>
        nil::gate::ports::ReadOnly<bool>* add_node_enabler(
            std::string_view tag,
            const Outputs& outputs,
            std::index_sequence<O...> /* seq */,
            const Expects& expects,
            std::index_sequence<E...> /* seq */
        )
        {
            if constexpr (sizeof...(O) + sizeof...(E) > 0)
            {
                return get<0>(gate.node(
                    [](std::conditional_t<true, bool, decltype(O)>... flags_o,
                       std::conditional_t<true, bool, decltype(E)>... flags_e)
                    { return (false || ... || flags_o) || ((false || ... || flags_e)); },
                    {get<O>(outputs)->info_requested(tag)...,
                     get<E>(expects)->info_requested(tag)...}
                ));
            }
            else
            {
                // will not run tests that does not have any outputs
                return get<0>(gate.node([]() { return false; }));
            }
        }

        template <typename T, typename Inputs, typename Expects, std::size_t... I, std::size_t... E>
        auto add_node_impl(
            [[maybe_unused]] std::string_view tag,
            T callable,
            nil::gate::ports::ReadOnly<bool>* is_enabled,
            const Inputs& inputs,
            const Expects& expects,
            std::index_sequence<I...> /* seq */,
            std::index_sequence<E...> /* seq */
        )
        {
            return gate.node(
                std::move(callable),
                {is_enabled, get<I>(inputs)->get_input(tag)..., get<E>(expects)->get_expect(tag)...}
            );
        }

        template <typename T, template <typename> typename Info>
        void add_info_on_load(Info<T>* s)
        {
            on_load(
                *s->frame,
                [s](std::string_view tag)
                {
                    if (const auto rerun = s->info_rerun(tag); rerun != nullptr)
                    {
                        rerun->set_value({});
                        s->gate->commit();
                    }
                }
            );
        }

        template <typename T, template <typename> typename Info>
        void add_info_on_sub(Info<T>* s)
        {
            on_sub(
                *s->frame,
                [this, s](std::string_view tag, std::size_t count)
                {
                    if (auto* requested = s->info_requested(tag); requested != nullptr)
                    {
                        requested->set_value(count > 0);
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
                        for (const auto& f : this->installed_tag_expects(tag))
                        {
                            if (const auto it = this->expect_frames.find(f.substr(0, f.size() - 4));
                                it != this->expect_frames.end())
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
