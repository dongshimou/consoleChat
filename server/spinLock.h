#pragma once
#include <atomic>
class SpinLock {
public:
    SpinLock()noexcept {
        m_lock.clear();
    }
    void lock() {
        while (m_lock.test_and_set(std::memory_order_acquire));
    }
    void unlock() {
        m_lock.clear(std::memory_order_release);
    }
    SpinLock(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
private:
    std::atomic_flag m_lock;
};