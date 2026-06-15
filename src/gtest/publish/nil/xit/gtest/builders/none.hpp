#pragma once

#include <nil/service/structs.hpp>

namespace nil::xit::gtest::builders
{
    struct none
    {
    };
}

namespace nil::service
{
    template <>
    struct codec<xit::gtest::builders::none>
    {
        static std::size_t size(const xit::gtest::builders::none& message)
        {
            (void)message;
            return 0;
        }

        static std::size_t serialize(void* output, const xit::gtest::builders::none& data)
        {
            (void)output;
            (void)data;
            return 0;
        }

        static xit::gtest::builders::none deserialize(const void* input, std::uint64_t size)
        {
            (void)input;
            (void)size;
            return {};
        }
    };
}
