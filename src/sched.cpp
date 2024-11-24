//run with sudo

#include <iostream>
#include <pthread.h>
#include <unistd.h> // for sleep
#include <sched.h>  // for scheduling policies

void* task1(void* arg) {
    struct sched_param param;
    param.sched_priority = 20; // higher priority for shorter period
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    while (true) {
        std::cout << "[Task 1] Executing...\n";
        usleep(100000); // 100ms period
    }
    return nullptr;
}

void* task2(void* arg) {
    struct sched_param param;
    param.sched_priority = 10; // lower priority for longer period
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    while (true) {
        std::cout << "[Task 2] Executing...\n";
        usleep(200000); // 200ms period
    }
    return nullptr;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, nullptr, task1, nullptr);
    pthread_create(&t2, nullptr, task2, nullptr);

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);

    return 0;
}
