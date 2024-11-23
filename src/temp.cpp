//ru with sudo to force priority
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <fstream>
#include <sched.h>
#include <pthread.h>

std::atomic<bool> running(true);

double readSensor() {
    static double temp = 25.0;
    temp += (rand() % 10 - 5) * 0.1; // simulate temperature fluctuation
    return temp;
}

void setRealtimePriority() {
    struct sched_param sched;
    sched.sched_priority = 20; // priority (1 to 99, higher is more critical)
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sched) != 0) {
        std::cerr << "Failed to set real-time priority\n";
    }
}

// Real-time temp monitoring task
void temperatureTask() {
    setRealtimePriority();

    using namespace std::chrono;
    auto next_time = steady_clock::now();

    while (running) {
        next_time += milliseconds(100); // 100ms period
        double temperature = readSensor();
        std::cout << "Temperature: " << temperature << "°C\n";

        // simulate processing (keep it deterministic)
        std::this_thread::sleep_for(milliseconds(5)); // Simulated work

        // wait for the next period
        std::this_thread::sleep_until(next_time);
    }
}

int main() {

    std::thread realtimeThread(temperatureTask);

    std::cout << "press Enter to stop...\n";
    std::cin.get();
    running = false;

    realtimeThread.join();
    return 0;
}
