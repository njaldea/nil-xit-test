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

nlohmann::json to_json(std::istream& iss)
{
    return nlohmann::json::parse(iss);
}

void from_json(std::ostream& oss, const nlohmann::json& data)
{
    oss << data;
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
