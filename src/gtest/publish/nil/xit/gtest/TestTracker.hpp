#pragma once

#include <mutex>
#include <thread>
#include <unordered_map>

namespace nil::xit::gtest
{
    class TestTracker final
    {
    public:
        void set_result(bool value);
        bool pop_result();

    private:
        mutable std::mutex mutex_;
        std::unordered_map<std::thread::id, bool> results_;
    };
}
