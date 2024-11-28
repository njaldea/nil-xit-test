#pragma once

#include "../Instances.hpp"

#include <filesystem>
#include <fstream>
#include <string_view>

namespace nil::xit::gtest
{
    std::string_view tag_to_dir(std::string_view tag);

    template <typename Reader>
        requires requires(Reader reader) {
            { reader(std::declval<std::ifstream&>()) };
        }
    auto read(const std::filesystem::path& path, const Reader& reader)
    {
        using type = decltype(reader(std::declval<std::ifstream&>()));
        if (!std::filesystem::exists(path))
        {
            // throw std::runtime_error("not found: " + path.string());
            return type();
        }
        std::ifstream file(path, std::ios::binary);
        return reader(file);
    }

    template <typename Writer, typename Data>
        requires requires(Writer writer, Data data) {
            { writer(std::declval<std::ofstream&>(), data) };
        }
    void write(const std::filesystem::path& path, const Writer& writer, const Data& data)
    {
        if (!std::filesystem::exists(path))
        {
            std::filesystem::create_directories(path.parent_path());
        }
        std::ofstream file(path, std::ios::binary);
        writer(file, data);
    }

    template <typename Reader, typename Writer>
        requires requires(Reader reader, Writer writer) {
            { reader(std::declval<std::istream&>()) };
            { writer(std::declval<std::ostream&>(), reader(std::declval<std::istream&>())) };
        }
    auto from_file_with_update(std::string file_name, Reader reader, Writer writer)
    {
        using type = decltype(reader(std::declval<std::istream&>()));

        struct Loader final
        {
        public:
            Loader(std::string init_file_name, Reader init_reader, Writer init_writer)
                : file_name(std::move(init_file_name))
                , reader(std::move(init_reader))
                , writer(std::move(init_writer))
            {
            }

            type initialize() const
            {
                auto& test_path = get_instance().paths.test;
                const auto path = test_path / file_name;
                return nil::xit::gtest::read(path, reader);
            }

            type initialize(std::string_view tag) const
            {
                auto& test_path = get_instance().paths.test;
                const auto path = test_path / tag_to_dir(tag) / file_name;
                return nil::xit::gtest::read(path, reader);
            }

            void update(const type& new_value) const
            {
                auto& test_path = get_instance().paths.test;
                const auto path = test_path / file_name;
                nil::xit::gtest::write(path, writer, new_value);
            }

            void update(std::string_view tag, const type& new_value) const
            {
                auto& test_path = get_instance().paths.test;
                const auto path = test_path / tag_to_dir(tag) / file_name;
                nil::xit::gtest::write(path, writer, new_value);
            }

        private:
            std::string file_name;
            Reader reader;
            Writer writer;
        };

        return Loader(std::move(file_name), std::move(reader), std::move(writer));
    }

    template <typename Reader, typename Writer>
        requires requires(Reader reader, Writer writer) {
            { reader(std::declval<std::istream&>()) };
            { writer(std::declval<std::ostream&>(), reader(std::declval<std::istream&>())) };
        }
    auto from_file_with_finalize(std::string file_name, Reader reader, Writer writer)
    {
        using type = decltype(reader(std::declval<std::istream&>()));

        struct Loader final
        {
        public:
            Loader(std::string init_file_name, Reader init_reader, Writer init_writer)
                : file_name(std::move(init_file_name))
                , reader(std::move(init_reader))
                , writer(std::move(init_writer))
            {
            }

            type initialize() const
            {
                auto& test_path = get_instance().paths.test;
                const auto path = test_path / file_name;
                return nil::xit::gtest::read(path, reader);
            }

            type initialize(std::string_view tag) const
            {
                auto& test_path = get_instance().paths.test;
                const auto path = test_path / tag_to_dir(tag) / file_name;
                return nil::xit::gtest::read(path, reader);
            }

            void finalize(const type& new_value) const
            {
                auto& test_path = get_instance().paths.test;
                const auto path = test_path / file_name;
                nil::xit::gtest::write(path, writer, new_value);
            }

            void finalize(std::string_view tag, const type& new_value) const
            {
                auto& test_path = get_instance().paths.test;
                const auto path = test_path / tag_to_dir(tag) / file_name;
                nil::xit::gtest::write(path, writer, new_value);
            }

        private:
            std::string file_name;
            Reader reader;
            Writer writer;
        };

        return Loader(std::move(file_name), std::move(reader), std::move(writer));
    }

    template <typename Reader>
        requires requires(Reader reader) {
            { reader(std::declval<std::istream&>()) };
        }
    auto from_file(std::string file_name, Reader reader)
    {
        using type = decltype(reader(std::declval<std::istream&>()));

        struct Loader final
        {
        public:
            Loader(std::string init_file_name, Reader init_reader)
                : file_name(std::move(init_file_name))
                , reader(std::move(init_reader))
            {
            }

            type initialize() const
            {
                auto& test_path = get_instance().paths.test;
                const auto path = test_path / file_name;
                return nil::xit::gtest::read(path, reader);
            }

            type initialize(std::string_view tag) const
            {
                auto& test_path = get_instance().paths.test;
                const auto path = test_path / tag_to_dir(tag) / file_name;
                return nil::xit::gtest::read(path, reader);
            }

        private:
            std::string file_name;
            Reader reader;
        };

        return Loader(std::move(file_name), std::move(reader));
    }
}