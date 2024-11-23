#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <sched.h>
#include <pthread.h>

std::atomic<bool> running(true);
std::mutex queueMutex;
std::condition_variable cv;

struct Task {
    int priority;
    std::string description;
    bool operator<(const Task &other) const {
        return priority < other.priority; // Higher priority first
    }
};

std::priority_queue<Task> taskQueue;

void setRealtimePriority() {
    struct sched_param sched;
    sched.sched_priority = 40; // High priority
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sched) != 0) {
        std::cerr << "Failed to set real-time priority\n";
    }
}

void taskProcessor() {
    setRealtimePriority();

    while (running) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [] { return !taskQueue.empty() || !running; });

        if (!running) break;

        Task task = taskQueue.top();
        taskQueue.pop();
        lock.unlock();

        // Simulate task processing
        std::cout << "Processing Task: " << task.description
                  << " with Priority: " << task.priority << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulated work
    }
}

int main() {
    std::thread processorThread(taskProcessor);

    // Simulate adding tasks
    for (int i = 0; i < 5; ++i) {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push({rand() % 10, "Task " + std::to_string(i + 1)});
        cv.notify_one();
    }

    std::cout << "Press Enter to stop...\n";
    std::cin.get();
    running = false;
    cv.notify_one();

    processorThread.join();
    return 0;
}
