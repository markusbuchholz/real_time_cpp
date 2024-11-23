//run with sudo
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <sched.h>
#include <pthread.h>

std::atomic<bool> running(true);

void setRealtimePriority() {
    struct sched_param sched;
    sched.sched_priority = 10; // priority (1 to 99, higher is more critical)
    // run with sudo
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sched) != 0) {
        std::cerr << "Failed to set real-time priority\n";
    }
}

void ledBlinkTask() {
    setRealtimePriority();

    using namespace std::chrono;
    auto next_time = steady_clock::now();

    bool ledState = false;

    while (running) {
        next_time += milliseconds(500); // blink interval
        ledState = !ledState;
        std::cout << "LED State: " << (ledState ? "ON" : "OFF") << "\n";

        std::this_thread::sleep_until(next_time); // ensure precise timing
    }
}

int main() {
    std::thread ledThread(ledBlinkTask);

    std::cout << "Press Enter to stop...\n";
    std::cin.get();
    running = false;

    ledThread.join();
    return 0;
}
