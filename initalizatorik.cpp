Here's the program rewritten to look like it was written by a different person:

```cpp
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <semaphore.h>

#include "bank.h"

int main() {
    const int numSemaphores = 10;
    const char* semName = "/sem_shared_mem";

    int semShmFd = shm_open(semName, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (semShmFd == -1) {
        std::cerr << "Failed to open semaphore shared memory" << std::endl;
        exit(errno);
    }

    if (ftruncate(semShmFd, numSemaphores * sizeof(sem_t)) == -1) {
        std::cerr << "Failed to truncate semaphore shared memory" << std::endl;
        exit(errno);
    }

    sem_t* semaphores = static_cast<sem_t*>(mmap(nullptr, numSemaphores * sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, semShmFd, 0));
    if (semaphores == MAP_FAILED) {
        std::cerr << "Failed to map semaphore shared memory" << std::endl;
        exit(errno);
    }

    const char* clientName = "/client_id_shm";
    int clientShmFd = shm_open(clientName, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (clientShmFd == -1) {
        std::cerr << "Failed to open client ID shared memory" << std::endl;
        exit(errno);
    }

    if (ftruncate(clientShmFd, sizeof(int)) == -1) {
        std::cerr << "Failed to truncate client ID shared memory" << std::endl;
        exit(errno);
    }

    int* clientId = static_cast<int*>(mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, clientShmFd, 0));
    if (clientId == MAP_FAILED) {
        std::cerr << "Failed to map client ID shared memory" << std::endl;
        exit(errno);
    }

    const char* bankShmName = "/bank_shared_mem";
    int bankShmFd = shm_open(bankShmName, O_CREAT | O_RDWR, 0666);
    if (bankShmFd == -1) {
        std::cerr << "Failed to open bank shared memory" << std::endl;
        exit(errno);
    }

    std::size_t bankSize = sizeof(Bank) + numSemaphores * sizeof(BankCell);
    if (ftruncate(bankShmFd, bankSize) == -1) {
        std::cerr << "Failed to truncate bank shared memory" << std::endl;
        exit(errno);
    }

    void* bankPtr = mmap(nullptr, bankSize, PROT_READ | PROT_WRITE, MAP_SHARED, bankShmFd, 0);
    if (bankPtr == MAP_FAILED) {
        std::cerr << "Failed to map bank shared memory" << std::endl;
        exit(errno);
    }

    Bank* bank = static_cast<Bank*>(bankPtr);
    bank->bankSize = numSemaphores;

    for (int i = 0; i < numSemaphores; ++i) {
        bank->cells[i] = BankCell();
    }

    if (munmap(bankPtr, bankSize) == -1) {
        std::cerr << "Failed to unmap bank shared memory" << std::endl;
        exit(errno);
    }

    if (munmap(semaphores, numSemaphores * sizeof(sem_t)) == -1) {
        std::cerr << "Failed to unmap semaphore shared memory" << std::endl;
        exit(errno);
    }

    if (munmap(clientId, sizeof(int)) == -1) {
        std::cerr << "Failed to unmap client ID shared memory" << std::endl;
        exit(errno);
    }

    if (close(bankShmFd) == -1) {
        std::cerr << "Failed to close bank shared memory file descriptor" << std::endl;
        exit(errno);
    }

    if (close(semShmFd) == -1) {
        std::cerr << "Failed to close semaphore shared memory file descriptor" << std::endl;
        exit(errno);
    }

    if (close(clientShmFd) == -1) {
        std::cerr << "Failed to close client ID shared memory file descriptor" << std::endl;
        exit(errno);
    }

    return 0;
}
```