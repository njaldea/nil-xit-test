#pragma once

#include "../Instances.hpp"

#include <nil/xalt/literal.hpp>
#include <nil/xalt/transparent_stl.hpp>
#include <nil/xit/structs.hpp>
#include <nil/xit/test/utils.hpp>

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string_view>

namespace nil::xit::gtest
{
    namespace detail
    {
        xit::FileInfo tag_to_file_info(std::string_view tag);

        template <typename T>
        auto read(
            const xalt::transparent_umap<std::filesystem::path>& groups,
            const xit::FileInfo& file_info
        )
        {
            T data;

            auto it = groups.find(file_info.group);
            if (it != groups.end())
            {
                const auto p = it->second / file_info.path;
                if (std::filesystem::exists(p))
                {
                    std::ifstream file(p, std::ios::binary);
                    file >> data;
                }
            }
            return data;
        }

        template <typename T, nil::xalt::literal file_name>
        auto read(
            const xalt::transparent_umap<std::filesystem::path>& groups,
            const xit::FileInfo& file_info
        )
        {
            T data;
            auto it = groups.find(file_info.group);
            if (it != groups.end())
            {
                const auto p = it->second / file_info.path / nil::xalt::literal_v<file_name>;
                if (std::filesystem::exists(p))
                {
                    std::ifstream file(p, std::ios::binary);
                    file >> data;
                }
            }
            return data;
        }

        template <typename T>
        void write(
            const xalt::transparent_umap<std::filesystem::path>& groups,
            const xit::FileInfo& file_info,
            const T& data
        )
        {
            std::cout << file_info.group << std::endl;
            auto it = groups.find(file_info.group);
            if (it != groups.end())
            {
                const auto p = it->second / file_info.path;
                if (!std::filesystem::exists(p))
                {
                    std::filesystem::create_directories(p.parent_path());
                }
                std::ofstream file(p, std::ios::binary);
                file << data;
            }
            throw std::runtime_error("unreachable");
        }

        template <typename T, nil::xalt::literal file_name>
        void write(
            const xalt::transparent_umap<std::filesystem::path>& groups,
            const xit::FileInfo& file_info,
            const T& data
        )
        {
            auto it = groups.find(file_info.group);
            if (it != groups.end())
            {
                const auto p = it->second / file_info.path / nil::xalt::literal_v<file_name>;
                if (!std::filesystem::exists(p))
                {
                    std::filesystem::create_directories(p.parent_path());
                }
                std::ofstream file(p, std::ios::binary);
                file << data;
            }
            throw std::runtime_error("unreachable");
        }
    }

    template <typename T, nil::xalt::literal file_name, typename I = T>
    struct from_file;

    template <typename T, nil::xalt::literal file_name, typename I>
        requires(xalt::starts_with<file_name, "$">())
    struct from_file<T, file_name, I> final
    {
        T initialize() const
        {
            return T(detail::read<I>(
                get_instance().paths.groups,
                detail::tag_to_file_info(xalt::literal_sv<file_name>)
            ));
        }
    };

    template <typename T, nil::xalt::literal file_name, typename I>
        requires(!xalt::starts_with<file_name, "$">())
    struct from_file<T, file_name, I> final
    {
        T initialize(std::string_view tag) const
        {
            return T(detail::read<I, file_name>(
                get_instance().paths.groups,
                detail::tag_to_file_info(tag)
            ));
        }
    };

    template <typename T, nil::xalt::literal file_name, typename I = T>
    struct from_file_with_finalize;

    template <typename T, nil::xalt::literal file_name, typename I>
        requires(xalt::starts_with<file_name, "$">())
    struct from_file_with_finalize<T, file_name, I> final
    {
        T initialize() const
        {
            return T(detail::read<I>(
                get_instance().paths.groups,
                detail::tag_to_file_info(xalt::literal_sv<file_name>)
            ));
        }

        void finalize(const T& new_value) const
        {
            detail::write<I>(
                get_instance().paths.groups,
                detail::tag_to_file_info(xalt::literal_sv<file_name>),
                new_value
            );
        }
    };

    template <typename T, nil::xalt::literal file_name, typename I>
        requires(!xalt::starts_with<file_name, "$">())
    struct from_file_with_finalize<T, file_name, I> final
    {
        T initialize(std::string_view tag) const
        {
            return T(detail::read<I, file_name>(
                get_instance().paths.groups,
                detail::tag_to_file_info(tag) //
            ));
        }

        void finalize(std::string_view tag, const T& new_value) const
        {
            detail::write<I, file_name>(
                get_instance().paths.groups,
                detail::tag_to_file_info(tag),
                new_value
            );
        }
    };

    template <typename T, nil::xalt::literal file_name, typename I = T>
    struct from_file_with_update;

    template <typename T, nil::xalt::literal file_name, typename I>
        requires(xalt::starts_with<file_name, "$">())
    struct from_file_with_update<T, file_name, I> final
    {
        T initialize() const
        {
            return T(detail::read<I>(
                get_instance().paths.groups,
                detail::tag_to_file_info(xalt::literal_sv<file_name>)
            ));
        }

        void update(const T& new_value) const
        {
            detail::write<I>(
                get_instance().paths.groups,
                detail::tag_to_file_info(xalt::literal_sv<file_name>),
                new_value
            );
        }
    };

    template <typename T, nil::xalt::literal file_name, typename I>
        requires(!xalt::starts_with<file_name, "$">())
    struct from_file_with_update<T, file_name, I> final
    {
        T initialize(std::string_view tag) const
        {
            return T(detail::read<I, file_name>(
                get_instance().paths.groups,
                detail::tag_to_file_info(tag) //
            ));
        }

        void update(std::string_view tag, const T& new_value) const
            requires(!xalt::starts_with<file_name, "$">())
        {
            detail::write<I, file_name>(
                get_instance().paths.groups,
                detail::tag_to_file_info(tag),
                new_value
            );
        }
    };
}
