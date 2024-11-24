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

void initializeSharedMemory(int &shm_fd, SharedData *&data) {
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // resize shared memory
    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // map shared memory
    data = static_cast<SharedData *>(mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // init mutex
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&data->mutex, &attr);
    data->position = 0.0;
}

void robot1Task(SharedData *data) {
    using namespace std::chrono;
    auto next_time = steady_clock::now();
    double position = 0.0;

    while (running) {
        next_time += milliseconds(100); // 100ms interval

        // update position in shared memory
        pthread_mutex_lock(&data->mutex);
        position += 1.0; // move forward
        data->position = position;
        std::cout << "[Robot1] Updated position to: " << position << "\n";
        pthread_mutex_unlock(&data->mutex);

        std::this_thread::sleep_until(next_time); // precise timing
    }
}

int main() {
    int shm_fd;
    SharedData *sharedData;

    // init shared mem
    initializeSharedMemory(shm_fd, sharedData);

    // start robot1 task
    std::thread robot1Thread(robot1Task, sharedData);

    std::cout << "Press Enter to stop...\n";
    std::cin.get();
    running = false;

    robot1Thread.join();

    // clean up
    munmap(sharedData, sizeof(SharedData));
    close(shm_fd);
    shm_unlink(SHM_NAME);

    return 0;
}
