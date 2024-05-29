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
#include <string>

#include "bank.h"

// Function prototypes
std::string processInput(std::string input);

// Test function prototypes
void testCurrentBalanceValidInput(int num = 0);
void testMinimumBalanceValidInput(int num = 0);
void testMaximumBalanceValidInput(int num = 1000);
void testFreezeCellValidInput();
void testUnfreezeCellValidInput();
void testAddToAllValidInput(int num = 50);
void testSubFromAllValidInput(int num = 50);
void testSetMinValidInput();
void testSetMaxValidInput();
void testTransferValidInput();

// Test functions
void testCurrentBalanceValidInput(int num) {
    std::string input = "current 1";
    std::string expectedOutput = "Current balance for cell number 1: " + std::to_string(num);
    std::cout << "Test Case: " << input << std::endl;
    std::string result = processInput(input);
    if (result == expectedOutput) {
        std::cout << "Current Balance Test Passed" << std::endl;
    } else {
        std::cout << "Current Balance Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

void testMinimumBalanceValidInput(int num) {
    std::string input = "minimum 2";
    std::string expectedOutput = "Minimum balance for cell number 2: " + std::to_string(num);
    std::cout << "Test Case: " << input << std::endl;
    std::string result = processInput(input);
    if (result == expectedOutput) {
        std::cout << "Minimum Balance Test Passed" << std::endl;
    } else {
        std::cout << "Minimum Balance Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

void testMaximumBalanceValidInput(int num) {
    std::string input = "maximum 3";
    std::string expectedOutput = "Maximum balance for cell number 3: " + std::to_string(num);
    std::cout << "Test Case: " << input << std::endl;
    std::string result = processInput(input);
    if (result == expectedOutput) {
        std::cout << "Maximum Balance Test Passed" << std::endl;
    } else {
        std::cout << "Maximum Balance Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

void testFreezeCellValidInput() {
    std::string input = "freeze 4";
    std::string expectedOutput = "Cell number 4 successfully frozen";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = processInput(input);
    if (result == expectedOutput) {
        std::cout << "Freeze Test Passed" << std::endl;
    } else {
        std::cout << "Freeze Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

void testUnfreezeCellValidInput() {
    std::string input = "unfreeze 4";
    std::string expectedOutput = "Cell number 4 successfully unfrozen";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = processInput(input);
    if (result == expectedOutput) {
        std::cout << "Unfreeze Test Passed" << std::endl;
    } else {
        std::cout << "Unfreeze Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

void testAddToAllValidInput(int num) {
    std::string input = "addall " + std::to_string(num);
    std::string expectedOutput = "Successfully added " + std::to_string(num) + " to all cells";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = processInput(input);
    if (result == expectedOutput) {
        std::cout << "Add All Test Passed" << std::endl;
    } else {
        std::cout << "Add All Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

void testSubFromAllValidInput(int num) {
    std::string input = "suball " + std::to_string(num);
    std::string expectedOutput = "Successfully subbed " + std::to_string(num) + " from all cells";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = processInput(input);
    if (result == expectedOutput ) {
        std::cout << "Sub All Test Passed" << std::endl;
    } else {
        std::cout << "Sub All Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

void testSetMinValidInput() {
    std::string input = "setmin 1 50";
    std::string expectedOutput = "Minimum for cell 1 is set to 50";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = processInput(input);
    if (result == expectedOutput) {
        std::cout << "Set Min Test Passed" << std::endl;
    } else {
        std::cout << "Set Min Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

void testSetMaxValidInput() {
    std::string input = "setmax 2 100";
    std::string expectedOutput = "Maximum for cell 2 is set to 100";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = processInput(input);
    if (result == expectedOutput) {
        std::cout << "Set Max Test Passed" << std::endl;
    } else {
        std::cout << "Set Max Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

void testTransferValidInput() {
    std::string input = "transfer 3 4 30";
    std::string expectedOutput = "Successfully transferred 30 from 3 to 4";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = processInput(input);
    if (result == expectedOutput) {
        std::cout << "Transfer Test Passed" << std::endl;
    } else {
        std::cout << "Transfer Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

// Global variables
int client_id;
sem_t *semaphores;
Bank* bank_ptr;

int main()
{
    const int num_cells = 10;
    const char* sem_name = "/sem_shared_mem";
    int sem_shm = shm_open(sem_name,  O_RDWR, 0666);
    if(sem_shm == -1)
    {
        std::cerr <<"sem shm open"<<std::endl;
        exit(errno);
    }

    semaphores = (sem_t*)mmap(nullptr, num_cells * sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, sem_shm, 0);

    const char* client_name = "/client_id_shm";
    int client_shm = shm_open(client_name, O_RDWR, 0666);
    if(client_shm == -1)
    {
        std::cerr <<"sem shm open"<<std::endl;
        exit(errno);
    }
    int* client_id_ptr = (int*)mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, client_shm, 0);
    client_id = *client_id_ptr + 1;
    *client_id_ptr = client_id;

    if(sem_init(&semaphores[client_id - 1], 1, 1) == -1)
    {
        std::cerr << "sem_init" <<std::endl;
        exit(errno);
    }

    const char* shm_name = "/bank_shared_mem";
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if(shm_fd == -1)
    {
        std::cerr << "shm_open" <<std::endl;
        exit(errno);
    }

    std::size_t bank_size = sizeof(Bank) + num_cells * sizeof(BankCell);
    bank_ptr = (Bank*)mmap(nullptr, bank_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(bank_ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Test cases
    testCurrentBalanceValidInput();
    testMinimumBalanceValidInput();
    testMaximumBalanceValidInput();
    testFreezeCellValidInput();
    testUnfreezeCellValidInput();
    testAddToAllValidInput();
    testSubFromAllValidInput();
    testSetMinValidInput();
    testSetMaxValidInput();
    testTransferValidInput();

    // Clean up
    if (sem_destroy(&semaphores[client_id - 1]) == -1) {
        std::cerr << "sem_destroy";
        exit(EXIT_FAILURE);
    }
    *client_id_ptr -= 1;

    if(munmap(bank_ptr, bank_size) == -1)
    {
        std::cerr << "init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(munmap(semaphores, num_cells * sizeof(sem_t)) == -1)
    {
        std::cerr << "sem init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(munmap(client_id_ptr, sizeof(int)) == -1)
    {
        std::cerr << "id init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }
    if(close(shm_fd) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }
    if(close(sem_shm) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }
    if(close(client_shm) == -1)
    {
        std::cerr << "close" <<std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}

std::string processInput(std::string input)
{
    std::string str;
    std::stringstream strr(input);
    std::string temp;
    std::vector<std::string> in;
    while(strr >> temp) {
        in.push_back(temp);
    }


    return str;
}
