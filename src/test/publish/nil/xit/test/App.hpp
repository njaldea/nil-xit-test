#pragma once

#include "frame/expect/Info.hpp"
#include "frame/input/Global.hpp"
#include "frame/input/Test.hpp"
#include "frame/output/Info.hpp"

#include <nil/gate/IRunner.hpp>
#include <nil/gate/traits/port_override.hpp>
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

namespace nil::gate::traits
{
    template <typename T>
        requires(!requires() { std::declval<T>() == std::declval<T>(); })
    struct Port<T>
    {
        static bool is_eq(const T& /* current_value */, const T& /* new_value */)
        {
            return false;
        }
    };
}

namespace nil::xit::test
{
    class App
    {
    public:
        App(service::IService& service, std::string_view app_name, std::uint32_t jobs);

        ~App() noexcept;
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
                auto& frame = add_unique_frame(*xit, "index", std::string(path));
                add_value(frame, "tags", [this]() { return converter(installed_tags()); });
                add_signal(
                    frame,
                    "finalize",
                    [this](std::string_view tag) { return finalize_inputs(tag); }
                );
            }
            {
                auto& frame = add_tagged_frame(*xit, "frame_info");
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

            s->frame = &add_tagged_frame(*xit, std::string(id));
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

            s->frame = &add_tagged_frame(*xit, std::string(id), std::string(path));
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

            s->frame = &add_unique_frame(*xit, std::string(id));
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

            s->frame = &add_unique_frame(*xit, std::string(id), std::string(path));
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

            s->frame = &add_tagged_frame(*xit, std::string(id), std::string(path));
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

            s->frame = &add_tagged_frame(*xit, std::string(id));
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

            s->frame = &add_tagged_frame(*xit, std::string(id));
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

            s->frame = &add_tagged_frame(*xit, std::string(id), std::string(path));
            s->gate = &gate;
            add_info_on_load(s);
            add_info_on_sub(s);
            return s;
        }

        template <typename Callable, typename... Inputs, typename... Outputs, typename... Expects>
        void add_node(
            std::string_view tag,
            Callable&& callable, // <outputs..., expects> callable(inputs..., expects...)
            std::tuple<frame::input::Info<Inputs>*...> inputs,
            std::tuple<frame::output::Info<Outputs>*...> outputs,
            std::tuple<frame::expect::Info<Expects>*...> expects
        )
        {
            [&]<std::size_t... I, std::size_t... O, std::size_t... E>(
                std::index_sequence<I...> /* input indices */,
                std::index_sequence<O...> /* output indices*/,
                std::index_sequence<E...> /* expect indices */
            )
            {
                std::apply([&](const auto&... frame) { (frame->add_info(tag), ...); }, inputs);
                std::apply([&](const auto&... frame) { (frame->add_info(tag), ...); }, outputs);
                std::apply([&](const auto&... frame) { (frame->add_info(tag), ...); }, expects);

                if constexpr ((sizeof...(O) + sizeof...(E)) > 0)
                {
                    gate.post(
                        [this,
                         tag,
                         callable = std::forward<Callable>(callable),
                         inputs,
                         outputs,
                         expects](gate::Graph& graph) mutable
                        {
                            auto* enabling_node = graph.node(
                                [](std::conditional_t<true, bool, decltype(O)>... flags_o,
                                   std::conditional_t<true, bool, decltype(E)>... flags_e)
                                { return (false || ... || flags_o) || (false || ... || flags_e); },
                                {
                                    get<O>(outputs)->info_requested(tag)...,
                                    get<E>(expects)->info_requested(tag)... //
                                }
                            );

                            auto* test_node = graph.node(
                                [cb = std::forward<Callable>(callable)] //
                                (                                       //
                                    nil::gate::opt_outputs<Outputs..., Expects...> opt_outputs,
                                    bool enabled,
                                    const Inputs&... rest_i,
                                    const Expects&... rest_e
                                )
                                {
                                    if (!enabled)
                                    {
                                        return;
                                    }
                                    constexpr auto o_n = sizeof...(O);
                                    auto b = opt_outputs;
                                    auto r = cb(rest_i..., rest_e...);
                                    (get<O>(b)->set_value(std::move(get<O>(r))), ...);
                                    (get<E + o_n>(b)->set_value(std::move(get<E + o_n>(r))), ...);
                                },
                                {
                                    get<0>(enabling_node->outputs()),
                                    get<I>(inputs)->get_port(tag)...,
                                    get<E>(expects)->get_port(tag)... //
                                }
                            );

                            auto result = test_node->outputs();

                            if constexpr (sizeof...(O) > 0)
                            {
                                // iterate the output of the test and then map it with rerun tag
                                // and see if it will need to pass the new value to the UI
                                (
                                    [&]<typename OType>(
                                        frame::output::Info<OType>* output,
                                        nil::gate::ports::ReadOnly<OType>* port
                                    )
                                    {
                                        graph.node(
                                            [output,
                                             tag](const RerunTag& /* r */, const OType& output_data)
                                            { output->post(tag, output_data); },
                                            {output->info_rerun(tag), port}
                                        );
                                    }(get<O>(outputs), get<O>(result)),
                                    ...
                                );
                            }

                            if constexpr (sizeof...(E) > 0)
                            {
                                // iterate the expected of the test and then map it with rerun tag
                                // and see if it will need to pass the new value to the UI
                                (
                                    [&]<typename EType>(
                                        frame::expect::Info<EType>* expected,
                                        nil::gate::ports::ReadOnly<EType>* port
                                    )
                                    {
                                        graph.node(
                                            [expected, tag] //
                                            (const SingleFire& flag, const EType& expected_data)
                                            {
                                                if (flag.pop())
                                                {
                                                    expected->finalize(tag, expected_data);
                                                }
                                            },
                                            {expected->info_single_fire(tag), port}
                                        );
                                    }(get<E>(expects), get<E + sizeof...(O)>(result)),
                                    ...
                                );
                            }
                        }
                    );
                }
            }( //
                std::index_sequence_for<Inputs...>(),
                std::index_sequence_for<Outputs...>(),
                std::index_sequence_for<Expects...>()
            );
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

        void start()
        {
            gate.commit();
        }

    private:
        xit::Core* xit;
        nil::gate::Core gate;
        std::unique_ptr<nil::gate::IRunner> runner;

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

        template <typename T>
        void add_info_on_load(frame::output::Info<T>* s)
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
