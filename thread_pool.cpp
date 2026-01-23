#include<iostream>
#include<vector>
#include<thread>
#include<queue>
#include<functional>
#include<condition_variable>
#include<mutex>

using namespace std;

class ThreadPool{
public:
    ThreadPool(int n_threads) : numThreads(n_threads) {
        for(int i = 0; i < numThreads; ++i){
            workers.emplace_back(&ThreadPool::worker, this);
        }
    }

    ~ThreadPool(){
        {
            unique_lock<mutex> lock(queueMutex);
            stop = true;
        }
        cv.notify_all();
        for(auto& worker : workers){
            worker.join();
        }
    }

    template<typename F, typename... Args>
    void execute(F&& f, Args&&... args){
        auto task = bind(forward<F>(f), forward<Args>(args)...);
        {
            unique_lock<mutex> lock(queueMutex);
            if(stop) throw runtime_error("ThreadPool has been stopped");
            tasks.emplace(task);
        }
        cv.notify_one();
    }

private:
    vector<thread> workers;
    int numThreads;
    queue<function<void()>> tasks;
    mutex queueMutex;
    condition_variable cv;
    bool stop = false;

    void worker(){
        while(true){
            unique_lock<mutex> lock(queueMutex);
            cv.wait(lock, [this](){ return stop || !tasks.empty(); });
            if(stop && tasks.empty()) return;

            auto task = move(tasks.front());
            tasks.pop();
            lock.unlock();
            task();
        }
    }
};

int main(){
    ThreadPool pool(4);
    for(int i = 0; i < 10; ++i){
        pool.execute([i](){
            cout << "Task " << i << " is being processed by thread with id : " << this_thread::get_id() << endl;
        });
    }
    return 0;
}