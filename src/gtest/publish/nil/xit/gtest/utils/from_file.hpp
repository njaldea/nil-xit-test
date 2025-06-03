#pragma once

#include "../Instances.hpp"

#include <nil/xit/test/utils.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>

namespace nil::xit::gtest
{
    namespace detail
    {
        std::string_view tag_to_dir(std::string_view tag);

        template <typename T, nil::xalt::literal file_name>
        auto read(const std::filesystem::path& path)
        {
            T data;
            const auto p = path / nil::xalt::literal_v<file_name>;
            if (std::filesystem::exists(p))
            {
                std::ifstream file(p, std::ios::binary);
                file >> data;
            }
            return data;
        }

        template <typename T, nil::xalt::literal file_name>
        void write(const std::filesystem::path& path, const T& data)
        {
            const auto p = path / nil::xalt::literal_v<file_name>;
            if (!std::filesystem::exists(p))
            {
                std::filesystem::create_directories(p.parent_path());
            }
            std::ofstream file(p, std::ios::binary);
            file << data;
        }
    }

    template <typename T, nil::xalt::literal file_name, typename I = T>
    struct from_file final
    {
        T initialize() const
        {
            return detail::read<I, file_name>(get_instance().paths.test);
        }

        T initialize(std::string_view tag) const
        {
            return detail::read<I, file_name>(get_instance().paths.test / detail::tag_to_dir(tag));
        }
    };

    template <typename T, nil::xalt::literal file_name, typename I = T>
    struct from_file_with_finalize final
    {
        T initialize() const
        {
            return T(detail::read<I, file_name>(get_instance().paths.test));
        }

        T initialize(std::string_view tag) const
        {
            return T(detail::read<I, file_name>(get_instance().paths.test / detail::tag_to_dir(tag)));
        }

        void finalize(const T& new_value) const
        {
            detail::write<I, file_name>(get_instance().paths.test, new_value);
        }

        void finalize(std::string_view tag, const T& new_value) const
        {
            detail::write<I, file_name>(
                get_instance().paths.test / detail::tag_to_dir(tag),
                new_value
            );
        }
    };

    template <typename T, nil::xalt::literal file_name, typename I = T>
    struct from_file_with_update final
    {
        T initialize() const
        {
            return T(detail::read<I, file_name>(get_instance().paths.test));
        }

        T initialize(std::string_view tag) const
        {
            return T(detail::read<I, file_name>(get_instance().paths.test / detail::tag_to_dir(tag)));
        }

        void update(const T& new_value) const
        {
            detail::write<I, file_name>(get_instance().paths.test, new_value);
        }

        void update(std::string_view tag, const T& new_value) const
        {
            detail::write<I, file_name>(
                get_instance().paths.test / detail::tag_to_dir(tag),
                new_value
            );
        }
    };
}
