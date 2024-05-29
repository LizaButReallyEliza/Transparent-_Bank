#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <semaphore.h>

#include "bank.h"

std::string processCommand(const std::string& input);

int clientIndex;
sem_t* semaphores;
Bank* bankPtr;
int* clientId;

int main() {
    const int numSemaphores = 10;
    const char* semaphoreName = "/sem_shared_mem";

    int semShmFd = shm_open(semaphoreName, O_RDWR, 0666);
    if (semShmFd == -1) {
        std::cerr << "Failed to open semaphore shared memory" << std::endl;
        exit(errno);
    }

    semaphores = static_cast<sem_t*>(mmap(nullptr, numSemaphores * sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, semShmFd, 0));

    const char* clientShmName = "/client_id_shm";
    int clientShmFd = shm_open(clientShmName, O_RDWR, 0666);
    if (clientShmFd == -1) {
        std::cerr << "Failed to open client ID shared memory" << std::endl;
        exit(errno);
    }

    clientId = static_cast<int*>(mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, clientShmFd, 0));
    (*clientId)++;
    clientIndex = *clientId - 1;

    if (sem_init(&semaphores[clientIndex], 1, 1) == -1) {
        std::cerr << "Failed to initialize semaphore" << std::endl;
        exit(errno);
    }

    const char* shmName = "/bank_shared_mem";
    int shmFd = shm_open(shmName, O_RDWR, 0666);
    if (shmFd == -1) {
        std::cerr << "Failed to open bank shared memory" << std::endl;
        exit(errno);
    }

    std::size_t bankSize = sizeof(Bank) + numSemaphores * sizeof(BankCell);
    bankPtr = static_cast<Bank*>(mmap(nullptr, bankSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));
    if (bankPtr == MAP_FAILED) {
        std::cerr << "Failed to map bank shared memory" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }
        std::cout << processCommand(input) << std::endl;
    }

    if (sem_destroy(&semaphores[clientIndex]) == -1) {
        std::cerr << "Failed to destroy semaphore" << std::endl;
        exit(EXIT_FAILURE);
    }

    (*clientId)--;
    if (munmap(bankPtr, bankSize) == -1) {
        std::cerr << "Failed to unmap bank shared memory" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (munmap(semaphores, numSemaphores * sizeof(sem_t)) == -1) {
        std::cerr << "Failed to unmap semaphore shared memory" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (munmap(clientId, sizeof(int)) == -1) {
        std::cerr << "Failed to unmap client ID shared memory" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (close(shmFd) == -1) {
        std::cerr << "Failed to close shared memory file descriptor" << std::endl;
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

    return 0;
}

std::string processCommand(const std::string& input) {
    std::stringstream inputStream(input);
    std::string token;
    std::vector<std::string> tokens;

    while (inputStream >> token) {
        tokens.push_back(token);
    }

    std::string response;
    int cellIndex = std::stoi(tokens[1]) - 1;

    if (tokens.size() == 2) {
        sem_wait(&semaphores[clientIndex]);

        if (tokens[0] == "current") {
            int balance = bankPtr->getCellCurrentBalance(cellIndex);
            response = (balance == -1) ? "Invalid cell ID" : "Current balance for cell " + tokens[1] + ": " + std::to_string(balance);
        } else if (tokens[0] == "minimum") {
            int minBalance = bankPtr->getCellMinBalance(cellIndex);
            response = (minBalance == -1) ? "Invalid cell ID" : "Minimum balance for cell " + tokens[1] + ": " + std::to_string(minBalance);
        } else if (tokens[0] == "maximum") {
            int maxBalance = bankPtr->getCellMaxBalance(cellIndex);
            response = (maxBalance == -1) ? "Invalid cell ID" : "Maximum balance for cell " + tokens[1] + ": " + std::to_string(maxBalance);
        } else if (tokens[0] == "freeze") {
            bool success = bankPtr->freezeCell(cellIndex);
            response = success ? "Cell " + tokens[1] + " successfully frozen" : "Invalid cell ID";
        } else if (tokens[0] == "unfreeze") {
            bool success = bankPtr->unfreezeCell(cellIndex);
            response = success ? "Cell " + tokens[1] + " successfully unfrozen" : "Invalid cell ID";
        } else if (tokens[0] == "addall") {
            bool success = bankPtr->addToAll(std::stoi(tokens[1]));
            response = success ? "Successfully added " + tokens[1] + " to all cells" : "Invalid operation";
        } else if (tokens[0] == "suball") {
            bool success = bankPtr->subtractFromAll(std::stoi(tokens[1]));
            response = success ? "Successfully subtracted " + tokens[1] + " from all cells" : "Invalid operation";
        } else {
            response = "Invalid command";
        }

        sem_post(&semaphores[clientIndex]);
    } else if (tokens.size() == 3) {
        sem_wait(&semaphores[clientIndex]);

        if (tokens[0] == "setmin") {
            bool success = bankPtr->setCellMinAmount(cellIndex, std::stoi(tokens[2]));
            response = success ? "Minimum balance for cell " + tokens[1] + " set to " + tokens[2] : "Invalid operation";
        } else if (tokens[0] == "setmax") {
            bool success = bankPtr->setCellMaxAmount(cellIndex, std::stoi(tokens[2]));
            response = success ? "Maximum balance for cell " + tokens[1] + " set to " + tokens[2] : "Invalid operation";
        } else {
            response = "Invalid command";
        }

        sem_post(&semaphores[clientIndex]);
    } else if (tokens.size() == 4) {
        sem_wait(&semaphores[clientIndex]);

        if (tokens[0] == "transfer") {
            bool success = bankPtr->transfer(std::stoi(tokens[1]) - 1, std::stoi(tokens[2]) - 1, std::stoi(tokens[3]));
            response = success ? "Successfully transferred " + tokens[3] + " from cell " + tokens[1] + " to cell " + tokens[2] : "Invalid operation";
        } else {
            response = "Invalid command";
        }

        sem_post(&semaphores[clientIndex]);
    } else {
        response = "Invalid command";
    }

    return response;
}
