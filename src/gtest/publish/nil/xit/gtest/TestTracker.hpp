#pragma once

#include <mutex>
#include <thread>
#include <unordered_map>

namespace nil::xit::gtest
{
    class TestTracker final
    {
    public:
        void set_result(bool value)
        {
            const auto tid = std::this_thread::get_id();
            std::lock_guard<std::mutex> lock(mutex_);
            results_[tid] = value;
        }

        bool pop_result()
        {
            const auto tid = std::this_thread::get_id();
            std::lock_guard<std::mutex> lock(mutex_);
            if (auto it = results_.find(tid); it != results_.end())
            {
                const auto value = it->second;
                results_.erase(it);
                return value;
            }
            return false;
        }

    private:
        mutable std::mutex mutex_;
        std::unordered_map<std::thread::id, bool> results_;
    };
}
