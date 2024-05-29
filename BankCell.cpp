#include "bank_cell.h"
#include <iostream>

BankCell::BankCell() : currentBalance(0), frozen(false) {}

int BankCell::getMinBalance() const {
    return minAmount;
}

int BankCell::getMaxBalance() const {
    return maxAmount;
}

int BankCell::getCurrentBalance() const {
    return currentBalance;
}

void BankCell::freeze() {
    frozen = true;
}

void BankCell::unfreeze() {
    frozen = false;
}

bool BankCell::isFrozen() const {
    return frozen;
}

bool BankCell::receiveAmount(int amount) {
    if (currentBalance + amount <= maxAmount) {
        currentBalance += amount;
        return true;
    }
    return false;
}

bool BankCell::sendAmount(int amount) {
    if (currentBalance - amount >= minAmount) {
        currentBalance -= amount;
        return true;
    }
    return false;
}

void BankCell::setMinAmount(int amount) {
    minAmount = amount;
}

void BankCell::setMaxAmount(int amount) {
    maxAmount = amount;
}
