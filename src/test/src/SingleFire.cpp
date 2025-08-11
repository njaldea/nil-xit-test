#include <nil/xit/test/SingleFire.hpp>

#include <utility>

namespace nil::xit::test
{
    bool SingleFire::operator==(const SingleFire& /* o */) const
    {
        return false;
    }

    bool SingleFire::pop() const
    {
        return std::exchange(value, false);
    }
}
