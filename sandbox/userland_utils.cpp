#include "userland_utils.hpp"

namespace nil::xit
{
    // this is necessary when publishing a custom data through the network going to the UI
    nlohmann::json buffer_type<nlohmann::json>::deserialize(const void* data, std::uint64_t size)
    {
        return nlohmann::json::from_msgpack(std::string_view(static_cast<const char*>(data), size));
    }

    std::vector<std::uint8_t> buffer_type<nlohmann::json>::serialize(const nlohmann::json& value)
    {
        return nlohmann::json::to_msgpack(value);
    }
}

bool operator==(const Ranges& l, const Ranges& r)
{
    return l.v1 == r.v1 && l.v2 == r.v2 && l.v3 == r.v3;
}

bool operator==(const Circle& l, const Circle& r)
{
    return l.position == r.position && l.radius == r.radius;
}

bool operator==(const Circles& l, const Circles& r)
{
    return l.x == r.x && l.x == r.y;
}

std::istream& operator>>(std::istream& iss, Circles& data)
{
    data = nlohmann::json::parse(iss);
    return iss;
}

std::ostream& operator<<(std::ostream& oss, const Circles& data)
{
    oss << nlohmann::json(data);
    return oss;
}

std::istream& operator>>(std::istream& iss, Ranges& data)
{
    auto c = char{};
    iss >> c;
    iss >> data.v1;
    iss >> c;
    iss >> data.v2;
    iss >> c;
    iss >> data.v3;
    iss >> c;
    return iss;
}

std::ostream& operator<<(std::ostream& oss, const Ranges& data)
{
    oss << '[' << data.v1 << ',' << data.v2 << ',' << data.v3 << ']';
    return oss;
}

void nlohmann::adl_serializer<Circle>::to_json(nlohmann::json& j, const Circle& v)
{
    j = nlohmann::json::object({{"position", v.position}, {"radius", v.radius}});
}

void nlohmann::adl_serializer<Circle>::from_json(const nlohmann::json& j, Circle& v)
{
    using nlohmann::json;
    v.position = j.value(json::json_pointer("/position"), std::array<double, 2>());
    v.radius = j.value(json::json_pointer("/radius"), 0.0);
}

void nlohmann::adl_serializer<Circles>::to_json(nlohmann::json& j, const Circles& v)
{
    j = nlohmann::json::object({{"x", v.x}, {"y", v.y}});
}

void nlohmann::adl_serializer<Circles>::from_json(const nlohmann::json& j, Circles& v)
{
    using nlohmann::json;
    v.x = j.value(json::json_pointer("/x"), Circle{});
    v.y = j.value(json::json_pointer("/y"), Circle{});
}
