#include <iostream>
#include <mutex>
#include <condition_variable>

// Writer First Preference

class RWLock {
private:
    int active_readers;     // Number of readers currently reading
    int active_writers;     // Number of writers currently writing (0 or 1)
    int waiting_writers;    // Number of writers waiting to write
    
    std::mutex mtx;
    std::condition_variable cv;

public:
    RWLock() : active_readers(0), active_writers(0), waiting_writers(0) {}

    // Reader Lock
    void read_lock() {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Wait if there is an active writer OR if any writer is waiting
        // This prevents "Writer Starvation"
        cv.wait(lock, [this] {
            return active_writers == 0 && waiting_writers == 0;
        });
        
        ++active_readers;
    }

    // Reader Unlock
    void read_unlock() {
        std::unique_lock<std::mutex> lock(mtx);
        --active_readers;
        
        // If I am the last reader, notify waiting writers
        if (active_readers == 0) {
            cv.notify_all();
        }
    }

    // Writer Lock
    void write_lock() {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Announce intent to write immediately
        ++waiting_writers;
        
        // Wait until there are no active readers and no active writers
        cv.wait(lock, [this] {
            return active_readers == 0 && active_writers == 0;
        });
        
        // I am no longer waiting, I am now active
        --waiting_writers;
        ++active_writers;
    }

    // Writer Unlock
    void write_unlock() {
        std::unique_lock<std::mutex> lock(mtx);
        --active_writers;
        
        // Notify all because either a waiting writer or waiting readers can proceed
        cv.notify_all();
    }
};