# Real-Time C++

A real-time (RT) system is one where the CPU allocates time to tasks based on a specific scheduling "recipe" (algorithm) to meet timing constraints and ensure predictable behavior. <br>
The correctness of the system depends not just on the results of computations but also on the timing of when those computations are performed.<br>
[Rate Monotonic Scheduling](https://en.wikipedia.org/wiki/Rate-monotonic_scheduling) (RMS) is one of the most commonly used algorithms for real-time systems, especially for systems with periodic tasks and fixed priorities.<br> 
In C++ we often use POSIX threads (pthread) to implement (RMS) and follow real-time (RT) principles.<br>
[POSIX threads](https://en.wikipedia.org/wiki/Pthreads) provide the tools to create, manage, and control threads, including setting priorities and scheduling policies required for RMS in RT systems.<br>

Usage:

```cpp
#include <pthread.h>
#include <sched.h>

//...
pthread_t thread;                      // thread identifier
struct sched_param param;              // scheduling parameters
param.sched_priority = 20;             // set the desired priority (1–99)

if (pthread_setschedparam(thread, SCHED_FIFO, &param) != 0) {
    perror("pthread_setschedparam failed");
    exit(EXIT_FAILURE);
}

//...
```

```bash

 g++ -std=c++20 pid.cpp -o pid
 
 sudo ./pid

```

## Real-Time Robot App

Robot1 :
Updates its position every 100ms and writes to shared memory.

Robot2 :
Reads Robot1's position from shared memory every 150ms and synchronizes its position.

Communication Mechanism:
POSIX Shared Memory:
Provides a shared memory region (shm_open) that both programs can access.
Uses ```pthread_mutex``` to ensure safe concurrent access to shared data.
Both processes must attach to the same shared memory object.


```bash
cd src/robot_app

g++ -std=c++20 -pthread -o robot1 robot_1.cpp

g++ -std=c++20 -pthread -o robot2 robot_2.cpp

sudo ./robot_1

sudo ./robot_2
```



## Links

- [real-time-cpp](https://github.com/ckormanyos/real-time-cpp)
