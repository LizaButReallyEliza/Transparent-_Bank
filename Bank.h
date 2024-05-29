#ifndef BANK_H
#define BANK_H

#include "bank_cell.h"
#include <vector>

class Bank {
public:
    Bank(int* bankArray, unsigned int size);

    int getCellCurrentBalance(int num) const;
    int getCellMinBalance(int num) const;
    int getCellMaxBalance(int num) const;
    bool freezeCell(int num);
    bool unfreezeCell(int num);
    bool transfer(int from, int to, int amount);
    bool addToAll(int amount);
    bool subtractFromAll(int amount);
    bool setCellMinAmount(int num, int amount);
    bool setCellMaxAmount(int num, int amount);

    BankCell& operator[](unsigned int index);

private:
    unsigned int bankSize;
    std::vector<BankCell> cells;
};

#endif 
