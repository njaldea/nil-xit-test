#pragma once

#include <nil/xit/buffer_type.hpp>

#include <nlohmann/json.hpp>
#include <type_traits>

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

    bool operator==(const Circle& o) const;
};

struct Circles
{
    Circle x = {};
    Circle y = {};
    bool operator==(const Circles& o) const;
};

nlohmann::json to_json(std::istream& iss);
void from_json(std::ostream& oss, const nlohmann::json& data);

Circles to_circles(std::istream& iss);
void from_circles(std::ostream& oss, const Circles& data);

Ranges to_range(std::istream& iss);
void from_range(std::ostream& oss, const Ranges& data);

inline auto from_json_ptr(const std::string& json_ptr)
{
    struct Accessor
    {
        // clang-format off
        using type = nlohmann::json;
        type& operator()(type& data) const { return data[ptr]; }
        const type& operator()(const type& data) const { return data[ptr]; }
        nlohmann::json::json_pointer ptr;
        // clang-format on
    };

    return Accessor{nlohmann::json::json_pointer(json_ptr)};
}

template <>
struct nlohmann::adl_serializer<Circle>
{
    static void to_json(nlohmann::json& j, const Circle& v);
    static void from_json(const nlohmann::json& j, Circle& v);
};

template <>
struct nlohmann::adl_serializer<Circles>
{
    static void to_json(nlohmann::json& j, const Circles& v);
    static void from_json(const nlohmann::json& j, Circles& v);
};

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
