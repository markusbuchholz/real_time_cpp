//ru with sudo to force priority

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cmath>
#include <sched.h>
#include <pthread.h>

std::atomic<bool> running(true);

void setRealtimePriority() {
    struct sched_param sched;
    sched.sched_priority = 30; // High priority
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sched) != 0) {
        std::cerr << "Failed to set real-time priority\n";
    }
}

// sim process variable (temp)
double processVariable = 20.0; // initial value

// PID 
void pidControllerTask(double setpoint, double kp, double ki, double kd) {
    setRealtimePriority();

    using namespace std::chrono;
    auto next_time = steady_clock::now();

    double error = 0, prev_error = 0;
    double integral = 0;
    double dt = 0.1; // 100ms

    while (running) {
        next_time += milliseconds(100); // interval

        error = setpoint - processVariable;

        integral += error * dt;
        double derivative = (error - prev_error) / dt;
        double output = kp * error + ki * integral + kd * derivative;

        processVariable += output * dt;

        std::cout << "Setpoint: " << setpoint << ", Process Variable: " << processVariable
                  << ", Output: " << output << "\n";

        prev_error = error;

        std::this_thread::sleep_until(next_time);
    }
}

int main() {
    std::thread pidThread(pidControllerTask, 50.0, 1.0, 0.1, 0.01); // setpoint, Kp, Ki, Kd

    std::cout << "press Enter to stop...\n";
    std::cin.get();
    running = false;

    pidThread.join();
    return 0;
}
