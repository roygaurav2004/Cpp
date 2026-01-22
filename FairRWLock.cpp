#include <mutex>
#include <condition_variable>

class FairRWLock {
private:
    int active_readers;
    int active_writers; // 0 or 1
    
    std::mutex state_mtx;      // Protects the counters
    std::mutex turnstile;      // Ensures FIFO order
    std::condition_variable cv;

public:
    FairRWLock() : active_readers(0), active_writers(0) {}

    void read_lock() {
        std::unique_lock<std::mutex> wait_line(turnstile);
        wait_line.unlock(); // Release immediately so other readers can get in line

        std::unique_lock<std::mutex> lock(state_mtx);
        cv.wait(lock, [this] { return active_writers == 0; });
        
        ++active_readers;
    }

    void read_unlock() {
        std::unique_lock<std::mutex> lock(state_mtx);
        --active_readers;
        if (active_readers == 0) {
            cv.notify_all(); // Wake up waiting writers
        }
    }

    void write_lock() {
        // We DO NOT unlock. We hold this until we are ready to write.
        // This blocks any subsequent readers or writers from "jumping" the queue.
        turnstile.lock(); 

        std::unique_lock<std::mutex> lock(state_mtx);
        cv.wait(lock, [this] { return active_readers == 0 && active_writers == 0; });
        
        active_writers = 1;
    }

    void write_unlock() {
        {
            std::unique_lock<std::mutex> lock(state_mtx);
            active_writers = 0;
        }
        // Release lock for further requests
        turnstile.unlock();
        cv.notify_all();
    }
};