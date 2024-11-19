#pragma once

#include <nil/xit/test.hpp>

#include <filesystem>
#include <fstream>

namespace nil::xit::gtest
{
    template <typename T>
    auto from_data(T data)
    {
        struct Loader final
        {
            auto operator()(std::string_view /* tag */) const
            {
                return data;
            }

            auto operator()() const
            {
                return data;
            }

            T data;
        };

        return Loader{std::move(data)};
    }

    template <typename C, typename M>
    auto from_member(M C::*member_ptr)
    {
        struct Accessor
        {
            M get(const C& data) const
            {
                return data.*member_ptr;
            }

            void set(C& data, M new_data) const
            {
                data.*member_ptr = std::move(new_data);
            }

            M C::*member_ptr;
        };

        return Accessor{member_ptr};
    }

    template <typename... T>
        requires(std::is_same_v<T, std::remove_cvref_t<T>> && ...)
    struct type
    {
    };

    template <size_t N>
    struct StringLiteral
    {
        // NOLINTNEXTLINE
        constexpr StringLiteral(const char (&str)[N])
        {
            std::copy_n(&str[0], N, &value[0]);
        }

        // NOLINTNEXTLINE
        char value[N];
    };

    template <StringLiteral S, typename... T>
    struct Frame;

    template <StringLiteral S, typename T>
    struct Frame<S, T>
    {
        using type = T;
        static constexpr auto* value = &S.value[0];
    };

    template <typename... T>
    struct InputFrames;
    template <typename... T>
    struct OutputFrames;

    template <typename... T>
    struct InputData
    {
        std::tuple<const T* const...> data;
    };

    template <typename... T>
    struct OutputData
    {
        std::tuple<T* const...> data;
    };

    template <std::size_t I, typename... T>
        requires(I < sizeof...(T))
    const auto& get(const InputData<T...>& o)
    {
        return *std::get<I>(o.data);
    }

    template <std::size_t I, typename... T>
        requires(I < sizeof...(T))
    auto& get(OutputData<T...>& o)
    {
        return *std::get<I>(o.data);
    }

    template <typename I, typename O>
    struct Test;

    template <typename... I, typename... O>
    struct Test<InputFrames<I...>, OutputFrames<O...>>
    {
        Test() = default;
        virtual ~Test() noexcept = default;
        Test(Test&&) = delete;
        Test(const Test&) = delete;
        Test& operator=(Test&&) = delete;
        Test& operator=(const Test&) = delete;

        using base_t = Test<InputFrames<I...>, OutputFrames<O...>>;
        using inputs_t = InputData<typename I::type...>;
        using outputs_t = OutputData<typename O::type...>;

        virtual void setup() {};
        virtual void teardown() {};
        virtual void run(const inputs_t& xit_inputs, outputs_t& xit_outputs) = 0;
    };

    template <StringLiteral... T>
    struct Input;
    template <StringLiteral... T>
    struct Output;

    template <StringLiteral... I, StringLiteral... O>
    struct Test<Input<I...>, Output<O...>>
        : Test<InputFrames<Frame<I>...>, OutputFrames<Frame<O>...>>
    {
    };

    namespace builders
    {
        using nil::xit::test::App;

        template <typename P, typename... I, typename... O>
        void install(
            App& app,
            std::string_view tag,
            type<Test<InputFrames<I...>, OutputFrames<O...>>> /* type */
        )
        {
            using base_t = Test<InputFrames<I...>, OutputFrames<O...>>;
            app.add_node(
                tag,
                [](const typename I::type&... args) -> std::tuple<typename O::type...>
                {
                    using inputs_t = typename base_t::inputs_t;
                    using outputs_t = typename base_t::outputs_t;
                    std::tuple<typename O::type...> result;
                    try
                    {
                        P p;
                        p.setup();
                        auto inputs = inputs_t{{&args...}};
                        auto outputs
                            = std::apply([](auto&... o) { return outputs_t{{&o...}}; }, result);
                        p.run(inputs, outputs);
                        p.teardown();
                    }
                    catch (const std::exception&)
                    {
                    }
                    catch (...)
                    {
                    }
                    return result;
                },
                std::make_tuple(app.get_input<typename I::type>(I::value)...), // NOLINT
                std::make_tuple(app.get_output<typename O::type>(O::value)...) // NOLINT
            );
        }

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
                setter(
                    std::declval<T&>(),
                    std::declval<decltype(getter(std::declval<const T&>()))>()
                )
            } -> std::same_as<void>;
        };

        struct IFrame
        {
            virtual ~IFrame() = default;
            IFrame() = default;
            IFrame(IFrame&&) = delete;
            IFrame(const IFrame&) = delete;
            IFrame& operator=(IFrame&&) = delete;
            IFrame& operator=(const IFrame&) = delete;
            virtual void install(App& app, const std::filesystem::path& path) = 0;
        };

        namespace input
        {
            namespace tagged
            {
                using nil::xit::test::frame::input::tagged::Info;

                template <typename T>
                struct Frame final: IFrame
                {
                    using type = T;

                    Frame(
                        std::string init_id,
                        std::filesystem::path init_file,
                        std::function<T(std::string_view)> init_initializer
                    )
                        : IFrame()
                        , id(std::move(init_id))
                        , file(std::move(init_file))
                        , initializer(std::move(init_initializer))
                    {
                    }

                    void install(App& app, const std::filesystem::path& path) override
                    {
                        auto* frame = app.add_tagged_input(id, path / file, initializer);
                        for (const auto& value_installer : values)
                        {
                            value_installer(*frame);
                        }
                    }

                    template <typename Getter, typename Setter>
                        requires(is_compatible_getter_setter<Getter, Setter, T>)
                    Frame<T>& value(std::string value_id, Getter getter, Setter setter)
                    {
                        using getter_return_t
                            = std::remove_cvref_t<decltype(getter(std::declval<const T&>()))>;
                        values.emplace_back(
                            [value_id = std::move(value_id),
                             getter = std::move(getter),
                             setter = std::move(setter)](Info<T>& info) {
                                info.template add_value<getter_return_t>(
                                    value_id,
                                    std::move(getter),
                                    std::move(setter)
                                );
                            }
                        );
                        return *this;
                    }

                    template <is_compatible_accessor<T> Accessor>
                    Frame<T>& value(std::string value_id, Accessor accessor)
                    {
                        using getter_return_t = decltype(accessor.get(std::declval<const T&>()));
                        return value(
                            std::move(value_id),
                            [accessor](const T& value) { return accessor.get(value); },
                            [accessor](T& value, getter_return_t new_value)
                            { return accessor.set(value, std::move(new_value)); }
                        );
                    }

                    template <typename U>
                    Frame<T>& value(std::string value_id, U T::*member)
                    {
                        return value(std::move(value_id), from_member(member));
                    }

                    Frame<T>& value(std::string value_id)
                    {
                        return value(
                            std::move(value_id),
                            [](const T& value) { return value; },
                            [](T& value, T new_value) { value = std::move(new_value); }
                        );
                    }

                    std::string id;
                    std::filesystem::path file;
                    std::function<T(std::string_view)> initializer;
                    std::vector<std::function<void(Info<T>&)>> values;
                };
            }

            namespace unique
            {
                using nil::xit::test::frame::input::unique::Info;

                template <typename T>
                struct Frame final: IFrame
                {
                    using type = T;

                    Frame(
                        std::string init_id,
                        std::filesystem::path init_file,
                        std::function<T()> init_initializer
                    )
                        : IFrame()
                        , id(std::move(init_id))
                        , file(std::move(init_file))
                        , initializer(std::move(init_initializer))
                    {
                    }

                    void install(App& app, const std::filesystem::path& path) override
                    {
                        auto* frame = app.add_unique_input(id, path / file, initializer);
                        for (const auto& value_installer : values)
                        {
                            value_installer(*frame);
                        }
                    }

                    template <typename Getter, typename Setter>
                        requires(is_compatible_getter_setter<Getter, Setter, T>)
                    Frame<T>& value(std::string value_id, Getter getter, Setter setter)
                    {
                        using getter_return_t = decltype(getter(std::declval<const T&>()));
                        values.emplace_back(
                            [value_id = std::move(value_id),
                             getter = std::move(getter),
                             setter = std::move(setter)](Info<T>& info) {
                                info.template add_value<getter_return_t>(
                                    value_id,
                                    std::move(getter),
                                    std::move(setter)
                                );
                            }
                        );
                        return *this;
                    }

                    template <is_compatible_accessor<T> Accessor>
                    Frame<T>& value(std::string value_id, Accessor accessor)
                    {
                        using getter_return_t = decltype(accessor.get(std::declval<const T&>()));
                        return value(
                            std::move(value_id),
                            [accessor](const T& value) { return accessor.get(value); },
                            [accessor](T& value, getter_return_t new_value)
                            { return accessor.set(value, std::move(new_value)); }
                        );
                    }

                    template <typename U>
                    Frame<T>& value(std::string value_id, U T::*member)
                    {
                        return value(std::move(value_id), from_member(member));
                    }

                    Frame<T>& value(std::string value_id)
                    {
                        return value(
                            std::move(value_id),
                            [](const T& value) { return value; },
                            [](T& value, T new_value) { value = std::move(new_value); }
                        );
                    }

                    std::string id;
                    std::filesystem::path file;
                    std::function<T()> initializer;
                    std::vector<std::function<void(Info<T>&)>> values;
                };
            }
        }

        namespace output
        {
            using nil::xit::test::frame::output::Info;

            template <typename T>
            struct Frame final: IFrame
            {
                using type = T;

                Frame(std::string init_id, std::filesystem::path init_file)
                    : IFrame()
                    , id(std::move(init_id))
                    , file(std::move(init_file))
                {
                }

                void install(App& app, const std::filesystem::path& path) override
                {
                    auto* frame = app.add_output<T>(id, path / file);
                    for (const auto& value_installer : values)
                    {
                        value_installer(*frame);
                    }
                }

                template <typename Getter>
                    requires requires(Getter getter) {
                        { getter(std::declval<T>()) };
                    }
                Frame<T>& value(std::string value_id, Getter getter)
                {
                    using getter_return_t
                        = std::remove_cvref_t<decltype(getter(std::declval<const T&>()))>;
                    values.emplace_back(
                        [value_id = std::move(value_id), getter = std::move(getter)](Info<T>& info)
                        { info.template add_value<getter_return_t>(value_id, std::move(getter)); }
                    );
                    return *this;
                }

                template <typename Accessor>
                    requires requires(Accessor accessor) {
                        { accessor.get(std::declval<T>()) };
                    }
                Frame<T>& value(std::string value_id, Accessor accessor)
                {
                    return value(
                        std::move(value_id),
                        [accessor](const T& value) { return accessor.get(value); }
                    );
                }

                template <typename U>
                Frame<T>& value(std::string value_id, U T::*member)
                {
                    return value(std::move(value_id), from_member(member));
                }

                Frame<T>& value(std::string value_id)
                {
                    return value(std::move(value_id), [](const T& value) { return value; });
                }

                std::string id;
                std::filesystem::path file;
                std::vector<std::function<void(Info<T>&)>> values;
            };
        }

        namespace main
        {
            template <typename Converter>
                requires requires(Converter converter) {
                    { converter(std::declval<std::vector<std::string>>()) };
                }
            struct Frame final: IFrame
            {
                explicit Frame(std::filesystem::path init_file, Converter init_converter)
                    : IFrame()
                    , file(std::move(init_file))
                    , converter(std::move(init_converter))
                {
                }

                void install(App& app, const std::filesystem::path& path) override
                {
                    app.add_main(path / file, converter);
                }

                std::filesystem::path file;
                Converter converter;
            };
        }

        template <typename T, typename... Args>
        concept is_initializer = requires(T initializer) {
            { initializer(std::declval<Args>()...) };
        };

        class MainBuilder final
        {
        public:
            template <typename FromVS>
                requires requires(FromVS converter) {
                    { converter(std::declval<std::vector<std::string>>()) };
                }
            void create_main(std::filesystem::path file, FromVS converter)
            {
                frame = std::make_unique<main::Frame<FromVS>>(
                    std::move(file),
                    std::move(std::move(converter))
                );
            }

            void install(App& app, const std::filesystem::path& path) const;

        private:
            std::unique_ptr<IFrame> frame;
        };

        class FrameBuilder final
        {
        public:
            template <typename Initializer>
                requires(!is_initializer<Initializer, std::string_view>)
            auto& create_tagged_input(
                std::string id,
                std::filesystem::path file,
                Initializer initializer
            )
            {
                return create_tagged_input(
                    std::move(id),
                    std::move(file),
                    from_data(std::move(initializer))
                );
            }

            template <typename Initializer>
                requires(is_initializer<Initializer, std::string_view>)
            auto& create_tagged_input(
                std::string id,
                std::filesystem::path file,
                Initializer initializer
            )
            {
                using type = decltype(initializer(std::declval<std::string_view>()));
                return static_cast<input::tagged::Frame<type>&>(
                    *frames.emplace_back(std::make_unique<input::tagged::Frame<type>>(
                        std::move(id),
                        std::move(file),
                        std::move(initializer)
                    ))
                );
            }

            template <typename Initializer>
                requires(!is_initializer<Initializer>)
            auto& create_unique_input(
                std::string id,
                std::filesystem::path file,
                Initializer initializer
            )
            {
                return create_unique_input(
                    std::move(id),
                    std::move(file),
                    from_data(std::move(initializer))
                );
            }

            template <typename Initializer>
                requires(is_initializer<Initializer>)
            auto& create_unique_input(
                std::string id,
                std::filesystem::path file,
                Initializer initializer
            )
            {
                using type = decltype(initializer());
                return static_cast<input::unique::Frame<type>&>(
                    *frames.emplace_back(std::make_unique<input::unique::Frame<type>>(
                        std::move(id),
                        std::move(file),
                        std::move(initializer)
                    ))
                );
            }

            template <typename T>
            auto& create_output(std::string id, std::filesystem::path file, type<T> /* type */ = {})
            {
                return static_cast<output::Frame<T>&>(*frames.emplace_back(
                    std::make_unique<output::Frame<T>>(std::move(id), std::move(file))
                ));
            }

            void install(App& app, const std::filesystem::path& path) const;

        private:
            std::vector<std::unique_ptr<IFrame>> frames;
        };

        class TestBuilder final
        {
        public:
            template <typename T>
            void add_test(std::string suite_id, std::string test_id, std::filesystem::path path)
            {
                tests.emplace_back(
                    [suite_id = std::move(suite_id),
                     test_id = std::move(test_id),
                     path = std::move(path)](App& app, const std::filesystem::path& relative_path)
                    {
                        for (const auto& dir :
                             std::filesystem::directory_iterator(relative_path / path))
                        {
                            if (dir.is_directory())
                            {
                                auto tag                             //
                                    = suite_id                       //
                                    + '.'                            //
                                    + test_id                        // NOLINT
                                    + '['                            //
                                    + dir.path().filename().string() //
                                    + ']';
                                nil::xit::gtest::builders::install<T>(
                                    app,
                                    tag,
                                    type<typename T::base_t>()
                                );
                            }
                        }
                    }
                );
            }

            void install(App& app, const std::filesystem::path& path) const;

        private:
            std::vector<std::function<void(App&, const std::filesystem::path&)>> tests;
        };
    }

    struct Instances
    {
        struct
        {
            std::filesystem::path server;
            std::filesystem::path main_ui;
            std::filesystem::path ui;
            std::filesystem::path test;
        } paths;

        nil::xit::gtest::builders::MainBuilder main_builder;
        nil::xit::gtest::builders::FrameBuilder frame_builder;
        nil::xit::gtest::builders::TestBuilder test_builder;
    };

    Instances& get_instance();

    template <typename Reader>
        requires requires(Reader reader) {
            { reader(std::declval<std::istream&>()) };
        }
    auto from_file(std::filesystem::path source_path, std::string file_name, Reader reader)
    {
        struct Loader final
        {
        public:
            Loader(
                std::filesystem::path init_source_path,
                std::string init_file_name,
                Reader init_reader
            )
                : source_path(std::move(init_source_path))
                , file_name(std::move(init_file_name))
                , reader(std::move(init_reader))
            {
            }

            ~Loader() noexcept = default;
            Loader(const Loader&) = default;
            Loader(Loader&&) = default;
            Loader& operator=(const Loader&) = default;
            Loader& operator=(Loader&&) = default;

            auto operator()(std::string_view tag) const
            {
                const auto i1 = tag.find_last_of('[') + 1;
                const auto i2 = tag.find_last_of(']');
                return load(
                    get_instance().paths.test / source_path / tag.substr(i1, i2 - i1) / file_name
                );
            }

            auto operator()() const
            {
                return load(get_instance().paths.test / source_path / file_name);
            }

        private:
            std::filesystem::path source_path;
            std::string file_name;
            Reader reader;

            auto load(const std::filesystem::path& path) const
            {
                if (!std::filesystem::exists(path))
                {
                    throw std::runtime_error("not found: " + path.string());
                }
                std::ifstream file(path, std::ios::binary);
                return reader(file);
            }
        };

        return Loader{std::move(source_path), std::move(file_name), std::move(reader)};
    }

    int main(int argc, const char** argv);
}

template <typename... T>
struct std::tuple_size<nil::xit::gtest::InputData<T...>>
    : std::integral_constant<std::size_t, sizeof...(T)>
{
};

template <typename... T>
struct std::tuple_size<nil::xit::gtest::OutputData<T...>>
    : std::integral_constant<std::size_t, sizeof...(T)>
{
};

template <std::size_t I, typename... T>
    requires(I < sizeof...(T))
struct std::tuple_element<I, nil::xit::gtest::InputData<T...>>
{
    using type = std::remove_cvref_t<
        decltype(*std::get<I>(std::declval<nil::xit::gtest::InputData<T...>>().data))>;
};

template <std::size_t I, typename... T>
    requires(I < sizeof...(T))
struct std::tuple_element<I, nil::xit::gtest::OutputData<T...>>
{
    using type = std::remove_cvref_t<
        decltype(*std::get<I>(std::declval<nil::xit::gtest::OutputData<T...>>().data))>;
};

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_WRAP(A) A
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_CONCAT_D(A, B) A##B
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_CONCAT(A, B) XIT_CONCAT_D(A, B)
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_IIFE(X) []() { X; return 0; }()
// clang-format on

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_TEST(SUITE, CASE, DIRECTORY)                                                           \
    struct xit_test_##SUITE##_##CASE: XIT_WRAP(SUITE)                                              \
    {                                                                                              \
        void run(const inputs_t& xit_inputs, outputs_t& xit_outputs) override;                     \
    };                                                                                             \
    const auto v_xit_test_##SUITE##_##CASE                                                         \
        = XIT_IIFE(nil::xit::gtest::get_instance()                                                 \
                       .test_builder.add_test<xit_test_##SUITE##_##CASE>(#SUITE, #CASE, DIRECTORY) \
        );                                                                                         \
    void xit_test_##SUITE##_##CASE::run(const inputs_t& xit_inputs, outputs_t& xit_outputs)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_FRAME_MAIN(PATH, CONVERTER)                                                            \
    const auto& XIT_CONCAT(xit_test_frame_, __COUNTER__)                                           \
        = XIT_IIFE(nil::xit::gtest::get_instance().main_builder.create_main(                       \
            PATH,                                                                                  \
            [](const std::vector<std::string>& v) { return CONVERTER(v); }                         \
        ))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_FRAME_DETAIL(ID, IMPL)                                                                 \
    template <>                                                                                    \
    struct nil::xit::gtest::Frame<ID>                                                              \
        : nil::xit::gtest::                                                                        \
              Frame<ID, std::remove_cvref_t<decltype(nil::xit::gtest::get_instance().IMPL)>::type> \
    {                                                                                              \
    };                                                                                             \
    const auto& XIT_CONCAT(xit_test_frame_, __COUNTER__) = nil::xit::gtest::get_instance().IMPL

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_FRAME_TAGGED_INPUT(ID, PATH, INITIALIZER)                                              \
    XIT_FRAME_DETAIL(ID, frame_builder.create_tagged_input(ID, PATH, INITIALIZER))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_FRAME_UNIQUE_INPUT(ID, PATH, INITIALIZER)                                              \
    XIT_FRAME_DETAIL(ID, frame_builder.create_unique_input(ID, PATH, INITIALIZER))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_FRAME_OUTPUT(ID, PATH, TYPE)                                                           \
    XIT_FRAME_DETAIL(ID, frame_builder.create_output<TYPE>(ID, PATH))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_PATH_SERVER_DIRECTORY(PATH)                                                            \
    const auto v_xit_path_server = XIT_IIFE(nil::xit::gtest::get_instance().paths.server = (PATH))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_PATH_MAIN_UI_DIRECTORY(PATH)                                                           \
    const auto v_xit_path_main_ui = XIT_IIFE(nil::xit::gtest::get_instance().paths.main_ui = (PATH))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_PATH_UI_DIRECTORY(PATH)                                                                \
    const auto v_xit_path_ui = XIT_IIFE(nil::xit::gtest::get_instance().paths.ui = (PATH))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_PATH_TEST_DIRECTORY(PATH)                                                              \
    const auto v_xit_path_test = XIT_IIFE(nil::xit::gtest::get_instance().paths.test = (PATH))
