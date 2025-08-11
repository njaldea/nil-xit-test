#pragma once

#include <nil/xalt/literal.hpp>
#include <nil/xalt/transparent_stl.hpp>
#include <nil/xit/buffer_type.hpp>
#include <nil/xit/structs.hpp>
#include <nil/xit/test/utils.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>

namespace nil::xit::gtest
{
    template <typename T>
    struct file_codec
    {
        static void write(std::ostream& oss, const T& data)
        {
            auto buffer = xit::buffer_type<T>::serialize(data);
            std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<std::uint8_t>(oss));
        }

        static T read(std::istream& iss)
        {
            std::string s((std::istreambuf_iterator<char>(iss)), std::istreambuf_iterator<char>());
            return xit::buffer_type<T>::deserialize(s.c_str(), s.size());
        }
    };

    std::filesystem::path resolve_from_tag(std::string_view tag, std::string_view path);
    std::filesystem::path resolve_from_path(std::string_view path);

    namespace detail
    {
        template <typename T>
        auto read(const std::filesystem::path& path)
        {
            if (std::filesystem::exists(path))
            {
                std::ifstream file(path, std::ios::binary);
                return file_codec<T>::read(file);
            }
            return T{};
        }

        template <typename T>
        void write(const std::filesystem::path& path, const T& data)
        {
            if (!std::filesystem::exists(path))
            {
                std::filesystem::create_directories(path.parent_path());
            }
            std::ofstream file(path, std::ios::binary);
            file_codec<T>::write(file, data);
        }
    }

    template <typename T, xalt::literal file_name>
    struct from_file;

    template <typename T, xalt::literal file_name>
    struct from_file_with_finalize;

    template <typename T, xalt::literal file_name>
    struct from_file_with_update;

    template <typename T, xalt::literal file_name>
        requires(xalt::starts_with<file_name, "$">())
    struct from_file<T, file_name> final
    {
        T initialize() const
        {
            return detail::read<T>(resolve_from_path(xalt::literal_sv<file_name>));
        }
    };

    template <typename T, xalt::literal file_name>
        requires(xalt::starts_with<file_name, "$">())
    struct from_file_with_finalize<T, file_name> final
    {
        T initialize() const
        {
            return detail::read<T>(resolve_from_path(xalt::literal_sv<file_name>));
        }

        void finalize(const T& new_value) const
        {
            detail::write<T>(resolve_from_path(xalt::literal_sv<file_name>), new_value);
        }
    };

    template <typename T, xalt::literal file_name>
        requires(xalt::starts_with<file_name, "$">())
    struct from_file_with_update<T, file_name> final
    {
        T initialize() const
        {
            return detail::read<T>(resolve_from_path(xalt::literal_sv<file_name>));
        }

        void update(const T& new_value) const
        {
            detail::write<T>(resolve_from_path(xalt::literal_sv<file_name>), new_value);
        }
    };

    template <typename T, xalt::literal file_name>
        requires(!xalt::starts_with<file_name, "$">())
    struct from_file<T, file_name> final
    {
        T initialize(std::string_view tag) const
        {
            return detail::read<T>(resolve_from_tag(tag, xalt::literal_sv<file_name>));
        }
    };

    template <typename T, xalt::literal file_name>
        requires(!xalt::starts_with<file_name, "$">())
    struct from_file_with_finalize<T, file_name> final
    {
        T initialize(std::string_view tag) const
        {
            return detail::read<T>(resolve_from_tag(tag, xalt::literal_sv<file_name>));
        }

        void finalize(std::string_view tag, const T& new_value) const
        {
            detail::write<T>(resolve_from_tag(tag, xalt::literal_sv<file_name>), new_value);
        }
    };

    template <typename T, xalt::literal file_name>
        requires(!xalt::starts_with<file_name, "$">())
    struct from_file_with_update<T, file_name> final
    {
        T initialize(std::string_view tag) const
        {
            return detail::read<T>(resolve_from_tag(tag, xalt::literal_sv<file_name>));
        }

        void update(std::string_view tag, const T& new_value) const
            requires(!xalt::starts_with<file_name, "$">())
        {
            detail::write<T>(resolve_from_tag(tag, xalt::literal_sv<file_name>), new_value);
        }
    };
}
