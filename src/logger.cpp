//run with sudo to force priority

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <sched.h>
#include <pthread.h>

std::atomic<bool> running(true);

void setRealtimePriority() {
    struct sched_param sched;
    sched.sched_priority = 10;
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sched) != 0) {
        std::cerr << "Failed to set real-time priority\n";
    }
}

double readSensor() {
    static double value = 0.0;
    value += (rand() % 100) / 100.0; // Simulated sensor data
    return value;
}

void dataLoggerTask() {
    setRealtimePriority();

    std::ofstream logfile("sensor_data.log");
    if (!logfile.is_open()) {
        std::cerr << "Failed to open log file\n";
        return;
    }

    using namespace std::chrono;
    auto next_time = steady_clock::now();

    while (running) {
        next_time += milliseconds(50); // 50ms interval
        double sensorData = readSensor();
        logfile << "Timestamp: " << duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count()
                << ", Sensor Data: " << sensorData << "\n";

        logfile.flush(); // Ensure data is written to disk

        std::this_thread::sleep_until(next_time);
    }

    logfile.close();
}

int main() {
    std::thread loggerThread(dataLoggerTask);

    std::cout << "Press Enter to stop...\n";
    std::cin.get();
    running = false;

    loggerThread.join();
    return 0;
}
