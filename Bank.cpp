#include "bank.h"
#include <iostream>

Bank::Bank(int* bankArray, unsigned int size) : bankSize(size), cells(size) {
    for (unsigned int i = 0; i < size; ++i) {
        cells[i].currentBalance = bankArray[i];
    }
}

int Bank::getCellCurrentBalance(int num) const {
    if (num < 0 || num >= bankSize) {
        return -1;
    }
    return cells[num].getCurrentBalance();
}

int Bank::getCellMinBalance(int num) const {
    if (num < 0 || num >= bankSize) {
        return -1;
    }
    return cells[num].getMinBalance();
}

int Bank::getCellMaxBalance(int num) const {
    if (num < 0 || num >= bankSize) {
        return -1;
    }
    return cells[num].getMaxBalance();
}

bool Bank::freezeCell(int num) {
    if (num < 0 || num >= bankSize) {
        return false;
    }
    cells[num].freeze();
    return true;
}

bool Bank::unfreezeCell(int num) {
    if (num < 0 || num >= bankSize) {
        return false;
    }
    cells[num].unfreeze();
    return true;
}

bool Bank::transfer(int from, int to, int amount) {
    if (from < 0 || from >= bankSize || to < 0 || to >= bankSize || amount < 0) {
        return false;
    }
    if (cells[from].isFrozen() || cells[to].isFrozen()) {
        return false;
    }
    if (cells[from].sendAmount(amount)) {
        if (cells[to].receiveAmount(amount)) {
            return true;
        }
        cells[from].receiveAmount(amount); // Rollback if transfer fails
    }
    return false;
}

bool Bank::addToAll(int amount) {
    if (amount < 0) {
        return false;
    }
    for (unsigned int i = 0; i < bankSize; ++i) {
        if (cells[i].isFrozen() || !cells[i].receiveAmount(amount)) {
            for (unsigned int j = 0; j < i; ++j) {
                cells[j].sendAmount(amount); // Rollback
            }
            return false;
        }
    }
    return true;
}

bool Bank::subtractFromAll(int amount) {
    if (amount < 0) {
        return false;
    }
    for (unsigned int i = 0; i < bankSize; ++i) {
        if (cells[i].isFrozen() || !cells[i].sendAmount(amount)) {
            for (unsigned int j = 0; j < i; ++j) {
                cells[j].receiveAmount(amount); // Rollback
            }
            return false;
        }
    }
    return true;
}

bool Bank::setCellMinAmount(int num, int amount) {
    if (num < 0 || num >= bankSize) {
        return false;
    }
    if (amount > cells[num].getCurrentBalance() || amount >= cells[num].getMaxBalance() || cells[num].isFrozen()) {
        return false;
    }
    cells[num].setMinAmount(amount);
    return true;
}

bool Bank::setCellMaxAmount(int num, int amount) {
    if (num < 0 || num >= bankSize) {
        return false;
    }
    if (amount < cells[num].getCurrentBalance() || amount <= cells[num].getMinBalance() || cells[num].isFrozen()) {
        return false;
    }
    cells[num].setMaxAmount(amount);
    return true;
}

BankCell& Bank::operator[](unsigned int index) {
    return cells[index];
}
