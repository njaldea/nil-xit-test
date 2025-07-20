#pragma once

namespace nil::xit::test
{
    struct SingleFire
    {
        mutable bool value = false;

        bool operator==(const SingleFire& /* o */) const;
        bool pop() const;
    };

}
