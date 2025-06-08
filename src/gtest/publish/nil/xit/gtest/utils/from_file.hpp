#pragma once

#include "../Instances.hpp"

#include <nil/xalt/literal.hpp>
#include <nil/xalt/transparent_stl.hpp>
#include <nil/xit/buffer_type.hpp>
#include <nil/xit/structs.hpp>
#include <nil/xit/test/utils.hpp>

#include <filesystem>
#include <fstream>
#include <stdexcept>
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

    namespace detail
    {
        xit::FileInfo tag_to_file_info(std::string_view tag);

        template <typename T>
        auto read(
            const xalt::transparent_umap<std::filesystem::path>& groups,
            const xit::FileInfo& file_info
        )
        {
            auto it = groups.find(file_info.group);
            if (it != groups.end())
            {
                const auto p = it->second / file_info.path;
                if (std::filesystem::exists(p))
                {
                    std::ifstream file(p, std::ios::binary);
                    return file_codec<T>::read(file);
                }
            }
            return T{};
        }

        template <typename T, xalt::literal file_name>
        auto read(
            const xalt::transparent_umap<std::filesystem::path>& groups,
            const xit::FileInfo& file_info
        )
        {
            auto it = groups.find(file_info.group);
            if (it != groups.end())
            {
                const auto p = it->second / file_info.path / xalt::literal_v<file_name>;
                if (std::filesystem::exists(p))
                {
                    std::ifstream file(p, std::ios::binary);
                    return file_codec<T>::read(file);
                }
            }
            return T();
        }

        template <typename T>
        void write(
            const xalt::transparent_umap<std::filesystem::path>& groups,
            const xit::FileInfo& file_info,
            const T& data
        )
        {
            auto it = groups.find(file_info.group);
            if (it != groups.end())
            {
                const auto p = it->second / file_info.path;
                if (!std::filesystem::exists(p))
                {
                    std::filesystem::create_directories(p.parent_path());
                }
                std::ofstream file(p, std::ios::binary);
                file_codec<T>::write(file, data);
                return;
            }
            throw std::runtime_error("unreachable");
        }

        template <typename T, xalt::literal file_name>
        void write(
            const xalt::transparent_umap<std::filesystem::path>& groups,
            const xit::FileInfo& file_info,
            const T& data
        )
        {
            auto it = groups.find(file_info.group);
            if (it != groups.end())
            {
                const auto p = it->second / file_info.path / xalt::literal_v<file_name>;
                if (!std::filesystem::exists(p))
                {
                    std::filesystem::create_directories(p.parent_path());
                }
                std::ofstream file(p, std::ios::binary);
                file_codec<T>::write(file, data);
                return;
            }
            throw std::runtime_error("unreachable");
        }
    }

    template <typename T, xalt::literal file_name>
    struct from_file;

    template <typename T, xalt::literal file_name>
        requires(xalt::starts_with<file_name, "$">())
    struct from_file<T, file_name> final
    {
        T initialize() const
        {
            return detail::read<T>(get_instance().paths.groups, detail::get_file_info<file_name>());
        }
    };

    template <typename T, xalt::literal file_name>
        requires(!xalt::starts_with<file_name, "$">())
    struct from_file<T, file_name> final
    {
        T initialize(std::string_view tag) const
        {
            return detail::read<T, file_name>(
                get_instance().paths.groups,
                detail::tag_to_file_info(tag)
            );
        }
    };

    template <typename T, xalt::literal file_name>
    struct from_file_with_finalize;

    template <typename T, xalt::literal file_name>
        requires(xalt::starts_with<file_name, "$">())
    struct from_file_with_finalize<T, file_name> final
    {
        T initialize() const
        {
            return from_file<T, file_name>().initialize();
        }

        void finalize(const T& new_value) const
        {
            detail::write<T>(
                get_instance().paths.groups,
                detail::get_file_info<file_name>(),
                new_value
            );
        }
    };

    template <typename T, xalt::literal file_name>
        requires(!xalt::starts_with<file_name, "$">())
    struct from_file_with_finalize<T, file_name> final
    {
        T initialize(std::string_view tag) const
        {
            return from_file<T, file_name>().initialize(tag);
        }

        void finalize(std::string_view tag, const T& new_value) const
        {
            detail::write<T, file_name>(
                get_instance().paths.groups,
                detail::tag_to_file_info(tag),
                new_value
            );
        }
    };

    template <typename T, xalt::literal file_name>
    struct from_file_with_update;

    template <typename T, xalt::literal file_name>
        requires(xalt::starts_with<file_name, "$">())
    struct from_file_with_update<T, file_name> final
    {
        T initialize() const
        {
            return from_file<T, file_name>().initialize();
        }

        void update(const T& new_value) const
        {
            from_file_with_finalize<T, file_name>().finalize(new_value);
        }
    };

    template <typename T, xalt::literal file_name>
        requires(!xalt::starts_with<file_name, "$">())
    struct from_file_with_update<T, file_name> final
    {
        T initialize(std::string_view tag) const
        {
            return from_file<T, file_name>().initialize(tag);
        }

        void update(std::string_view tag, const T& new_value) const
            requires(!xalt::starts_with<file_name, "$">())
        {
            from_file_with_finalize<T, file_name>().finalize(tag, new_value);
        }
    };
}
