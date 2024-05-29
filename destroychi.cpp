Here is the program rewritten to look like it was written by a different person:

```cpp
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

#include "bank.h"

int main() {
    const char* semName = "/sem_shared_mem";
    int semShmFd = shm_open(semName, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (semShmFd == -1) {
        std::cerr << "Failed to open semaphore shared memory" << std::endl;
        exit(errno);
    }

    const char* clientName = "/client_id_shm";
    int clientShmFd = shm_open(clientName, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (clientShmFd == -1) {
        std::cerr << "Failed to open client ID shared memory" << std::endl;
        exit(errno);
    }

    const char* bankShmName = "/bank_shared_mem";
    int bankShmFd = shm_open(bankShmName, O_RDWR, 0666);
    if (bankShmFd == -1) {
        std::cerr << "Failed to open bank shared memory" << std::endl;
        exit(errno);
    }

    if (shm_unlink(semName) == -1) {
        std::cerr << "Failed to unlink semaphore shared memory" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (shm_unlink(clientName) == -1) {
        std::cerr << "Failed to unlink client ID shared memory" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (shm_unlink(bankShmName) == -1) {
        std::cerr << "Failed to unlink bank shared memory" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (close(semShmFd) == -1) {
        std::cerr << "Failed to close semaphore shared memory file descriptor" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (close(clientShmFd) == -1) {
        std::cerr << "Failed to close client ID shared memory file descriptor" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (close(bankShmFd) == -1) {
        std::cerr << "Failed to close bank shared memory file descriptor" << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
```