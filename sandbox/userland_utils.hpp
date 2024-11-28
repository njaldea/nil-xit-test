#pragma once

#include <nil/xit/gtest.hpp>

#include <nil/xit/buffer_type.hpp>

#include <nlohmann/json.hpp>

struct Ranges
{
    std::int64_t v1;
    std::int64_t v2;
    std::int64_t v3;

    bool operator==(const Ranges& o) const;
};

struct Circle
{
    std::array<double, 2> position = {};
    double radius = 0.0;
};

struct Circles
{
    Circle x = {{0, 0}, 1.0};
    Circle y = {{0, 0}, 1.0};
    bool operator==(const Circles& o) const;
};

template <>
struct nlohmann::adl_serializer<Circle>
{
    static void to_json(nlohmann::json& j, const Circle& v)
    {
        j = nlohmann::json::object({{"position", v.position}, {"radius", v.radius}});
    }

    static void from_json(const nlohmann::json& j, Circle& v)
    {
        using nlohmann::json;
        v.position = j.value(json::json_pointer("/position"), std::array<double, 2>());
        v.radius = j.value(json::json_pointer("/radius"), 1.0);
    }
};

template <>
struct nlohmann::adl_serializer<Circles>
{
    static void to_json(nlohmann::json& j, const Circles& v)
    {
        j = nlohmann::json::object({{"x", v.x}, {"y", v.y}});
    }

    static void from_json(const nlohmann::json& j, Circles& v)
    {
        using nlohmann::json;
        v.x = j.value(json::json_pointer("/x"), Circle{{0, 0}, 1.0});
        v.y = j.value(json::json_pointer("/y"), Circle{{0, 0}, 1.0});
    }
};

nlohmann::json to_json(std::istream& iss);
void from_json(std::ostream& oss, const nlohmann::json& data);

Circles to_circles(std::istream& iss);
void from_circles(std::ostream& oss, const Circles& data);

Ranges to_range(std::istream& iss);
void from_range(std::ostream& oss, const Ranges& data);

template <typename T = nlohmann::json>
auto from_json_ptr(const std::string& json_ptr)
{
    struct Accessor
    {
        T get(const nlohmann::json& data) const
        {
            std::cout << data.dump() << std::endl;
            if (data.contains(json_ptr))
            {
                return data[json_ptr];
            }
            std::cout << "not found" << std::endl;
            return T();
        }

        void set(nlohmann::json& data, T new_data) const
        {
            data[json_ptr] = std::move(new_data);
        }

        nlohmann::json::json_pointer json_ptr;
    };

    return Accessor{nlohmann::json::json_pointer(json_ptr)};
}

using nil::xit::gtest::from_data;
using nil::xit::gtest::from_file;
using nil::xit::gtest::from_file_with_finalize;
using nil::xit::gtest::from_file_with_update;

namespace nil::xit
{
    // this is necessary when publishing a custom data through the network going to the UI
    template <>
    struct buffer_type<nlohmann::json>
    {
        static nlohmann::json deserialize(const void* data, std::uint64_t size);
        static std::vector<std::uint8_t> serialize(const nlohmann::json& value);
    };

    template <typename T>
        requires requires(T t) {
            { nlohmann::json(t) };
            { T(nlohmann::json()) };
        } && (!std::is_same_v<T, nlohmann::json>)
    struct buffer_type<T>
    {
        static T deserialize(const void* data, std::uint64_t size)
        {
            return buffer_type<nlohmann::json>::deserialize(data, size);
        }

        static std::vector<std::uint8_t> serialize(const T& value)
        {
            return buffer_type<nlohmann::json>::serialize(value);
        }
    };
}
