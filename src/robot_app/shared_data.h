// shared_data.h
#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <pthread.h>

struct SharedData {
    pthread_mutex_t mutex; // Mutex for synchronization
    double position;       // Shared position data
};

#endif // SHARED_DATA_H
