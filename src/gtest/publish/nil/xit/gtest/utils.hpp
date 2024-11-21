#pragma once

#include "Instances.hpp"

#include <filesystem>
#include <fstream>
#include <string_view>

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
                const auto path
                    = get_instance().paths.test / source_path / tag.substr(i1, i2 - i1) / file_name;
                return load(path);
            }

            auto operator()() const
            {
                const auto path = get_instance().paths.test / source_path / file_name;
                return load(path);
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
}
