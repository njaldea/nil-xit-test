#pragma once

#include <nil/xit/buffer_type.hpp>

#include <nlohmann/json.hpp>

#include <type_traits>

struct Ranges
{
    std::int64_t v1 = {};
    std::int64_t v2 = {};
    std::int64_t v3 = {};
};

struct Circle
{
    std::array<double, 2> position = {};
    double radius = 1.0;
};

struct Circles
{
    Circle x = {};
    Circle y = {};
};

bool operator==(const Ranges& l, const Ranges& r);
bool operator==(const Circle& l, const Circle& r);
bool operator==(const Circles& l, const Circles& r);

std::istream& operator>>(std::istream& iss, Circles& data);
std::ostream& operator<<(std::ostream& oss, const Circles& data);
std::istream& operator>>(std::istream& iss, Ranges& data);
std::ostream& operator<<(std::ostream& oss, const Ranges& data);

struct from_json_ptr
{
    using type = nlohmann::json;

    explicit from_json_ptr(const std::string& init_json_ptr)
        : ptr(init_json_ptr)
    {
    }

    // clang-format off
    type& operator()(type& data) const { return data[ptr]; }
    const type& operator()(const type& data) const { return data[ptr]; }
    nlohmann::json::json_pointer ptr;
    // clang-format on
};

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

// this is necessary when publishing a custom data through the network going to the UI
template <>
struct nil::xit::buffer_type<nlohmann::json>
{
    static nlohmann::json deserialize(const void* data, std::uint64_t size);
    static std::vector<std::uint8_t> serialize(const nlohmann::json& value);
};

template <typename T>
    requires requires(T t) {
        { nlohmann::json(t) };
        { T(nlohmann::json()) };
    } && (!std::is_same_v<T, nlohmann::json>)
struct nil::xit::buffer_type<T>
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
