#include "AiThread.hpp"

AiThread::AiThread(urai::Ai *ai)
{
    this->ai = ai;
}

std::future<int> AiThread::decide(const ur::GameState gameState)
{
    std::unique_lock<std::mutex> lock(myLock);
    
    Task task;
    task.gameState = gameState;
    task.promise = std::promise<int>();
    std::future<int> decisionFuture = task.promise.get_future();
    queue.push(std::move(task));

    lock.unlock();
    cv.notify_all();

    return decisionFuture;
}

void AiThread::setAi(urai::Ai *ai)
{
    this->ai = ai;
}

void AiThread::threadHandler()
{
    std::unique_lock<std::mutex> lock(myLock);
    
    do {
        cv.wait(lock, [this]
			{ return (!queue.empty() || stopping); }
		);
        if(!stopping && !queue.empty()) {
            Task task = std::move(queue.front());
            queue.pop();
            
            lock.unlock();
            const int decision = ai->decide(task.gameState);
            task.promise.set_value(decision);
            lock.lock();
        }
    } while(!stopping);
}

void AiThread::start()
{
    this->thread = std::thread(&AiThread::threadHandler, this);
}

void AiThread::stop()
{
    stopping = true;
    // equivalent to notify_all here I think
    cv.notify_one();
    thread.join();
}