#include "userland_utils.hpp"

namespace nil::xit
{
    // this is necessary when publishing a custom data through the network going to the UI
    nlohmann::json buffer_type<nlohmann::json>::deserialize(const void* data, std::uint64_t size)
    {
        return nlohmann::json::parse(std::string_view(static_cast<const char*>(data), size));
    }

    std::vector<std::uint8_t> buffer_type<nlohmann::json>::serialize(const nlohmann::json& value)
    {
        auto s = value.dump();
        return {s.begin(), s.end()};
    }
}

bool Ranges::operator==(const Ranges& o) const
{
    return v1 == o.v1 && v2 == o.v2 && v3 == o.v3;
}

bool Circle::operator==(const Circle& o) const
{
    return position == o.position && radius == o.radius;
}

bool Circles::operator==(const Circles& o) const
{
    return x == o.x && y == o.y;
}

nlohmann::json to_json(std::istream& iss)
{
    return nlohmann::json::parse(iss);
}

void from_json(std::ostream& oss, const nlohmann::json& data)
{
    oss << data;
}

Circles to_circles(std::istream& iss)
{
    return to_json(iss);
}

void from_circles(std::ostream& oss, const Circles& data)
{
    from_json(oss, data);
}

Ranges to_range(std::istream& iss)
{
    auto r = Ranges{};
    auto c = char{};
    iss >> c;
    iss >> r.v1;
    iss >> c;
    iss >> r.v2;
    iss >> c;
    iss >> r.v3;
    iss >> c;
    return r;
}

void from_range(std::ostream& oss, const Ranges& data)
{
    oss << '[' << data.v1 << ',' << data.v2 << ',' << data.v3 << ']';
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
