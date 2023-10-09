#pragma once

#include "Ai.hpp"

#include <cstddef>
#include <optional>
#include <future>
#include <queue>
#include <thread>
#include <condition_variable>

// implementation referenced https://github.com/dakejahl/dispatch_queue , albeit this is a bit more specialized
// a thread that is effectively a worker thread for AI processing, and allows calls to AI to be non-blocking :)
class AiThread {
private:
    struct Task {
        ur::GameState gameState;
        std::promise<int> promise;
    };
    std::thread thread;
    urai::Ai *ai;
    std::queue<Task> queue;
    std::condition_variable cv;
    std::mutex myLock;
    std::atomic<bool> stopping = false;
    void threadHandler();
public:
    AiThread(urai::Ai *ai);
    AiThread();

    std::future<int> decide(const ur::GameState gameState);
    void start();
    void setAi(urai::Ai *ai);
    void join();
    void stop();
};