# Real-Time C++

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
