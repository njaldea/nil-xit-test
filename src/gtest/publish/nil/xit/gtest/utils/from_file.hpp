#pragma once

#include "../Instances.hpp"

#include <filesystem>
#include <fstream>
#include <string_view>

namespace nil::xit::gtest
{
    inline std::string_view tag_to_dir(std::string_view tag)
    {
        const auto i1 = tag.find_last_of('[') + 1;
        const auto i2 = tag.find_last_of(']');
        return tag.substr(i1, i2 - i1);
    }

    template <typename Reader, typename Writer>
        requires requires(Reader reader, Writer writer) {
            { reader(std::declval<std::istream&>()) };
            { writer(std::declval<std::ostream&>(), reader(std::declval<std::istream&>())) };
        }
    auto from_file_rw(
        std::filesystem::path source_path,
        std::string file_name,
        Reader reader,
        Writer writer
    )
    {
        using type = decltype(reader(std::declval<std::istream&>()));

        struct Loader final
        {
        public:
            Loader(
                std::filesystem::path init_source_path,
                std::string init_file_name,
                Reader init_reader,
                Writer init_writer
            )
                : source_path(std::move(init_source_path))
                , file_name(std::move(init_file_name))
                , reader(std::move(init_reader))
                , writer(std::move(init_writer))
            {
            }

            ~Loader() noexcept = default;
            Loader(const Loader&) = default;
            Loader(Loader&&) = default;
            Loader& operator=(const Loader&) = default;
            Loader& operator=(Loader&&) = default;

            type load() const
            {
                const auto path = get_instance().paths.test / source_path / file_name;
                return load_impl(path);
            }

            type load(std::string_view tag) const
            {
                const auto path
                    = get_instance().paths.test / source_path / tag_to_dir(tag) / file_name;
                return load_impl(path);
            }

            void update(const type& new_value) const
            {
                const auto path = get_instance().paths.test / source_path / file_name;
                dump(path, new_value);
            }

            void update(std::string_view tag, const type& new_value) const
            {
                const auto path
                    = get_instance().paths.test / source_path / tag_to_dir(tag) / file_name;
                dump(path, new_value);
            }

        private:
            std::filesystem::path source_path;
            std::string file_name;
            Reader reader;
            Writer writer;

            auto tag_to_dir(std::string_view tag) const
            {
                const auto i1 = tag.find_last_of('[') + 1;
                const auto i2 = tag.find_last_of(']');
                return tag.substr(i1, i2 - i1);
            }

            auto load_impl(const std::filesystem::path& path) const
            {
                if (!std::filesystem::exists(path))
                {
                    throw std::runtime_error("not found: " + path.string());
                }
                std::ifstream file(path, std::ios::binary);
                return reader(file);
            }

            auto dump(const std::filesystem::path& path, const type& new_value) const
            {
                if (!std::filesystem::exists(path))
                {
                    std::filesystem::create_directories(path.parent_path());
                }
                std::ofstream file(path, std::ios::binary);
                writer(file, new_value);
            }
        };

        return Loader(
            std::move(source_path),
            std::move(file_name),
            std::move(reader),
            std::move(writer)
        );
    }

    template <typename Reader>
        requires requires(Reader reader) {
            { reader(std::declval<std::istream&>()) };
        }
    auto from_file(std::filesystem::path source_path, std::string file_name, Reader reader)
    {
        using type = decltype(reader(std::declval<std::istream&>()));

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

            type load() const
            {
                const auto path = get_instance().paths.test / source_path / file_name;
                return load_impl(path);
            }

            type load(std::string_view tag) const
            {
                const auto path
                    = get_instance().paths.test / source_path / tag_to_dir(tag) / file_name;
                return load_impl(path);
            }

        private:
            std::filesystem::path source_path;
            std::string file_name;
            Reader reader;

            auto load_impl(const std::filesystem::path& path) const
            {
                if (!std::filesystem::exists(path))
                {
                    throw std::runtime_error("not found: " + path.string());
                }
                std::ifstream file(path, std::ios::binary);
                return reader(file);
            }
        };

        return Loader(std::move(source_path), std::move(file_name), std::move(reader));
    }

    template <typename Writer>
    struct Unloader final
    {
        Unloader(
            std::filesystem::path init_source_path,
            std::string init_file_name,
            Writer init_writer
        )
            : source_path(std::move(init_source_path))
            , file_name(std::move(init_file_name))
            , writer(std::move(init_writer))
        {
        }

        ~Unloader() noexcept = default;
        Unloader(const Unloader&) = default;
        Unloader(Unloader&&) = default;
        Unloader& operator=(const Unloader&) = default;
        Unloader& operator=(Unloader&&) = default;

        void operator()(const auto& new_value) const
        {
            const auto path = get_instance().paths.test / source_path / file_name;
            dump(path, new_value);
        }

        void operator()(const auto& new_value, std::string_view tag) const
        {
            const auto path = get_instance().paths.test / source_path / tag_to_dir(tag) / file_name;
            dump(path, new_value);
        }

    private:
        std::filesystem::path source_path;
        std::string file_name;
        Writer writer;

        auto dump(const std::filesystem::path& path, const auto& new_value) const
        {
            if (!std::filesystem::exists(path))
            {
                std::filesystem::create_directories(path.parent_path());
            }
            std::ofstream file(path, std::ios::binary);
            writer(file, new_value);
        }
    };

    template <typename Writer>
    auto to_file(std::filesystem::path source_path, std::string file_name, Writer writer)
    {
        return Unloader<Writer>(std::move(source_path), std::move(file_name), std::move(writer));
    }
}
