#include <atomic>

class Spinlock {
private:
    std::atomic<bool> locked{false};
    
public:
    // Should spin until the lock is successfully acquired
    // compare_exchange_strong to atomically change locked from false to true
    void lock() {
        bool expected = false;
        while (!locked.compare_exchange_strong(expected, true)) {
            expected = false;
        }
    }
    
    // Should release the lock by setting locked to false
    void unlock() {
        locked.store(false, std::memory_order_release);
    }
};