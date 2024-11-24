#include <iostream>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <atomic>
#include "shared_data.h"

const char *SHM_NAME = "/robot_shared_memory";
std::atomic<bool> running(true);

void attachSharedMemory(int &shm_fd, SharedData *&data) {
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // map shared memory
    data = static_cast<SharedData *>(mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
}

void robot2Task(SharedData *data) {
    using namespace std::chrono;
    auto next_time = steady_clock::now();
    double position = 0.0;

    while (running) {
        next_time += milliseconds(150); // 150ms interval

        // read robot1 position and synchronize
        pthread_mutex_lock(&data->mutex);
        double targetPosition = data->position;
        pthread_mutex_unlock(&data->mutex);

        // move towards Robot1 position
        if (position < targetPosition) {
            position += 1.0;
        } else if (position > targetPosition) {
            position -= 1.0;
        }

        std::cout << "[Robot2] Current position: " << position
                  << ", Target position: " << targetPosition << "\n";

        if (position == targetPosition) {
            std::cout << "[Robot2] Synchronized with Robot1 at position: " << position << "\n";
        }

        std::this_thread::sleep_until(next_time); // precise timing
    }
}

int main() {
    int shm_fd;
    SharedData *sharedData;

    // attach to shared memory
    attachSharedMemory(shm_fd, sharedData);

    // start Robot2 task
    std::thread robot2Thread(robot2Task, sharedData);

    std::cout << "Press Enter to stop...\n";
    std::cin.get();
    running = false;

    robot2Thread.join();

    // clean up
    munmap(sharedData, sizeof(SharedData));
    close(shm_fd);

    return 0;
}
