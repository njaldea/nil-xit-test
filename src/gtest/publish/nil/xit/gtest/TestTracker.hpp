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

    struct TestTrackerScope
    {
        explicit TestTrackerScope(TestTracker* init_res);

        bool pop();

        TestTrackerScope(TestTrackerScope&&) = delete;
        TestTrackerScope(const TestTrackerScope&) = delete;
        TestTrackerScope& operator=(TestTrackerScope&&) = delete;
        TestTrackerScope& operator=(const TestTrackerScope&) = delete;

        ~TestTrackerScope();

        TestTracker* res;
    };
}
