#ifndef BANKCELL_H
#define BANKCELL_H

class BankCell {
public:
    BankCell();

    int getMinBalance() const;
    int getMaxBalance() const;
    int getCurrentBalance() const;
    void freeze();
    void unfreeze();
    bool isFrozen() const;
    bool sendAmount(int amount);
    bool receiveAmount(int amount);
    void setMinAmount(int amount);
    void setMaxAmount(int amount);

private:
    int currentBalance;
    int minAmount = 0;
    int maxAmount = 1000;
    bool frozen = false;
};

#endif 