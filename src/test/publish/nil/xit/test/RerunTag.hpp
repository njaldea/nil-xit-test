#pragma once

namespace nil::xit::test
{
    struct RerunTag
    {
        bool operator==(const RerunTag& /* o */) const;
    };
}
