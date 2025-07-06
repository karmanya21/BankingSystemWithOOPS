#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <ctime>
#include <algorithm>

// Transaction class to store transaction history
class Transaction {
private:
    std::string type;
    double amount;
    double balanceAfter;
    std::string timestamp;
    
public:
    Transaction(const std::string& t, double amt, double balance) 
        : type(t), amount(amt), balanceAfter(balance) {
        // Get current timestamp
        time_t now = time(0);
        timestamp = ctime(&now);
        timestamp.pop_back(); // Remove newline
    }
    
    void display() const {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Type: " << type << " | Amount: $" << amount 
                  << " | Balance: $" << balanceAfter << " | Time: " << timestamp << std::endl;
    }
    
    std::string getType() const { return type; }
    double getAmount() const { return amount; }
    std::string getTimestamp() const { return timestamp; }
};

// Abstract base class Account
class Account {
protected:
    std::string accountNumber;
    std::string holderName;
    double balance;
    std::vector<Transaction> transactionHistory;
    
public:
    Account(const std::string& accNum, const std::string& name, double initialBalance = 0.0)
        : accountNumber(accNum), holderName(name), balance(initialBalance) {
        if (initialBalance > 0) {
            transactionHistory.push_back(Transaction("Initial Deposit", initialBalance, balance));
        }
    }
    
    virtual ~Account() = default;
    
    // Pure virtual functions making this an abstract class
    virtual void deposit(double amount) = 0;
    virtual bool withdraw(double amount) = 0;
    virtual void displayAccountInfo() const = 0;
    virtual std::string getAccountType() const = 0;
    
    // Common methods for all account types
    double getBalance() const { return balance; }
    std::string getAccountNumber() const { return accountNumber; }
    std::string getHolderName() const { return holderName; }
    
    void displayTransactionHistory() const {
        std::cout << "\n=== Transaction History for " << accountNumber << " ===" << std::endl;
        if (transactionHistory.empty()) {
            std::cout << "No transactions found." << std::endl;
            return;
        }
        
        for (const auto& transaction : transactionHistory) {
            transaction.display();
        }
    }
    
    void addTransaction(const std::string& type, double amount) {
        transactionHistory.push_back(Transaction(type, amount, balance));
    }
};

// Savings Account class - inherits from Account
class SavingsAccount : public Account {
private:
    double interestRate;
    double minimumBalance;
    
public:
    SavingsAccount(const std::string& accNum, const std::string& name, 
                   double initialBalance = 0.0, double intRate = 0.04, double minBalance = 100.0)
        : Account(accNum, name, initialBalance), interestRate(intRate), minimumBalance(minBalance) {}
    
    void deposit(double amount) override {
        if (amount <= 0) {
            std::cout << "Invalid deposit amount!" << std::endl;
            return;
        }
        balance += amount;
        addTransaction("Deposit", amount);
        std::cout << "Deposited $" << std::fixed << std::setprecision(2) << amount 
                  << ". New balance: $" << balance << std::endl;
    }
    
    bool withdraw(double amount) override {
        if (amount <= 0) {
            std::cout << "Invalid withdrawal amount!" << std::endl;
            return false;
        }
        
        if (balance - amount < minimumBalance) {
            std::cout << "Withdrawal failed! Minimum balance of $" << minimumBalance 
                      << " must be maintained." << std::endl;
            return false;
        }
        
        balance -= amount;
        addTransaction("Withdrawal", amount);
        std::cout << "Withdrew $" << std::fixed << std::setprecision(2) << amount 
                  << ". New balance: $" << balance << std::endl;
        return true;
    }
    
    void applyInterest() {
        double interest = balance * interestRate / 12; // Monthly interest
        balance += interest;
        addTransaction("Interest Credit", interest);
        std::cout << "Interest of $" << std::fixed << std::setprecision(2) << interest 
                  << " applied. New balance: $" << balance << std::endl;
    }
    
    void displayAccountInfo() const override {
        std::cout << "\n=== Savings Account Information ===" << std::endl;
        std::cout << "Account Number: " << accountNumber << std::endl;
        std::cout << "Account Holder: " << holderName << std::endl;
        std::cout << "Account Type: Savings" << std::endl;
        std::cout << "Current Balance: $" << std::fixed << std::setprecision(2) << balance << std::endl;
        std::cout << "Interest Rate: " << (interestRate * 100) << "% per annum" << std::endl;
        std::cout << "Minimum Balance: $" << minimumBalance << std::endl;
    }
    
    std::string getAccountType() const override {
        return "Savings";
    }
    
    double getInterestRate() const { return interestRate; }
    double getMinimumBalance() const { return minimumBalance; }
};

// Current Account class - inherits from Account
class CurrentAccount : public Account {
private:
    double overdraftLimit;
    double overdraftFee;
    
public:
    CurrentAccount(const std::string& accNum, const std::string& name, 
                   double initialBalance = 0.0, double overdraftLim = 1000.0, double overdraftF = 25.0)
        : Account(accNum, name, initialBalance), overdraftLimit(overdraftLim), overdraftFee(overdraftF) {}
    
    void deposit(double amount) override {
        if (amount <= 0) {
            std::cout << "Invalid deposit amount!" << std::endl;
            return;
        }
        balance += amount;
        addTransaction("Deposit", amount);
        std::cout << "Deposited $" << std::fixed << std::setprecision(2) << amount 
                  << ". New balance: $" << balance << std::endl;
    }
    
    bool withdraw(double amount) override {
        if (amount <= 0) {
            std::cout << "Invalid withdrawal amount!" << std::endl;
            return false;
        }
        
        if (balance - amount < -overdraftLimit) {
            std::cout << "Withdrawal failed! Overdraft limit of $" << overdraftLimit 
                      << " exceeded." << std::endl;
            return false;
        }
        
        balance -= amount;
        addTransaction("Withdrawal", amount);
        
        // Apply overdraft fee if balance goes negative
        if (balance < 0) {
            balance -= overdraftFee;
            addTransaction("Overdraft Fee", overdraftFee);
            std::cout << "Overdraft fee of $" << overdraftFee << " applied." << std::endl;
        }
        
        std::cout << "Withdrew $" << std::fixed << std::setprecision(2) << amount 
                  << ". New balance: $" << balance << std::endl;
        return true;
    }
    
    void displayAccountInfo() const override {
        std::cout << "\n=== Current Account Information ===" << std::endl;
        std::cout << "Account Number: " << accountNumber << std::endl;
        std::cout << "Account Holder: " << holderName << std::endl;
        std::cout << "Account Type: Current" << std::endl;
        std::cout << "Current Balance: $" << std::fixed << std::setprecision(2) << balance << std::endl;
        std::cout << "Overdraft Limit: $" << overdraftLimit << std::endl;
        std::cout << "Overdraft Fee: $" << overdraftFee << std::endl;
        if (balance < 0) {
            std::cout << "*** ACCOUNT OVERDRAWN ***" << std::endl;
        }
    }
    
    std::string getAccountType() const override {
        return "Current";
    }
    
    double getOverdraftLimit() const { return overdraftLimit; }
    double getOverdraftFee() const { return overdraftFee; }
};

// Bank class to manage multiple accounts
class Bank {
private:
    std::vector<std::unique_ptr<Account>> accounts;
    std::string bankName;
    
public:
    Bank(const std::string& name) : bankName(name) {}
    
    void createSavingsAccount(const std::string& accNum, const std::string& holderName, 
                             double initialBalance = 0.0) {
        accounts.push_back(std::make_unique<SavingsAccount>(accNum, holderName, initialBalance));
        std::cout << "Savings account created successfully!" << std::endl;
    }
    
    void createCurrentAccount(const std::string& accNum, const std::string& holderName, 
                             double initialBalance = 0.0) {
        accounts.push_back(std::make_unique<CurrentAccount>(accNum, holderName, initialBalance));
        std::cout << "Current account created successfully!" << std::endl;
    }
    
    Account* findAccount(const std::string& accNum) {
        auto it = std::find_if(accounts.begin(), accounts.end(),
            [&accNum](const std::unique_ptr<Account>& acc) {
                return acc->getAccountNumber() == accNum;
            });
        return (it != accounts.end()) ? it->get() : nullptr;
    }
    
    void displayAllAccounts() const {
        std::cout << "\n=== All Accounts in " << bankName << " ===" << std::endl;
        if (accounts.empty()) {
            std::cout << "No accounts found." << std::endl;
            return;
        }
        
        for (const auto& account : accounts) {
            std::cout << "Account: " << account->getAccountNumber() 
                      << " | Holder: " << account->getHolderName()
                      << " | Type: " << account->getAccountType()
                      << " | Balance: $" << std::fixed << std::setprecision(2) 
                      << account->getBalance() << std::endl;
        }
    }
    
    void applyInterestToSavingsAccounts() {
        std::cout << "\n=== Applying Monthly Interest ===" << std::endl;
        for (auto& account : accounts) {
            SavingsAccount* savingsAcc = dynamic_cast<SavingsAccount*>(account.get());
            if (savingsAcc) {
                std::cout << "Account " << savingsAcc->getAccountNumber() << ": ";
                savingsAcc->applyInterest();
            }
        }
    }
    
    std::string getBankName() const { return bankName; }
};

// Menu-driven interface
class BankingSystem {
private:
    Bank bank;
    
public:
    BankingSystem() : bank("ABC Bank") {}
    
    void displayMenu() {
        std::cout << "\n========== " << bank.getBankName() << " Banking System ==========\n";
        std::cout << "1. Create Savings Account\n";
        std::cout << "2. Create Current Account\n";
        std::cout << "3. Deposit Money\n";
        std::cout << "4. Withdraw Money\n";
        std::cout << "5. Check Account Balance\n";
        std::cout << "6. View Account Details\n";
        std::cout << "7. View Transaction History\n";
        std::cout << "8. View All Accounts\n";
        std::cout << "9. Apply Interest to Savings Accounts\n";
        std::cout << "10. Exit\n";
        std::cout << "Enter your choice: ";
    }
    
    void run() {
        int choice;
        std::string accNum, holderName;
        double amount;
        
        while (true) {
            displayMenu();
            std::cin >> choice;
            
            switch (choice) {
                case 1:
                    std::cout << "Enter account number: ";
                    std::cin >> accNum;
                    std::cout << "Enter account holder name: ";
                    std::cin.ignore();
                    std::getline(std::cin, holderName);
                    std::cout << "Enter initial deposit (0 for no deposit): ";
                    std::cin >> amount;
                    bank.createSavingsAccount(accNum, holderName, amount);
                    break;
                    
                case 2:
                    std::cout << "Enter account number: ";
                    std::cin >> accNum;
                    std::cout << "Enter account holder name: ";
                    std::cin.ignore();
                    std::getline(std::cin, holderName);
                    std::cout << "Enter initial deposit (0 for no deposit): ";
                    std::cin >> amount;
                    bank.createCurrentAccount(accNum, holderName, amount);
                    break;
                    
                case 3:
                    std::cout << "Enter account number: ";
                    std::cin >> accNum;
                    if (Account* acc = bank.findAccount(accNum)) {
                        std::cout << "Enter deposit amount: ";
                        std::cin >> amount;
                        acc->deposit(amount);
                    } else {
                        std::cout << "Account not found!" << std::endl;
                    }
                    break;
                    
                case 4:
                    std::cout << "Enter account number: ";
                    std::cin >> accNum;
                    if (Account* acc = bank.findAccount(accNum)) {
                        std::cout << "Enter withdrawal amount: ";
                        std::cin >> amount;
                        acc->withdraw(amount);
                    } else {
                        std::cout << "Account not found!" << std::endl;
                    }
                    break;
                    
                case 5:
                    std::cout << "Enter account number: ";
                    std::cin >> accNum;
                    if (Account* acc = bank.findAccount(accNum)) {
                        std::cout << "Current balance: $" << std::fixed << std::setprecision(2) 
                                  << acc->getBalance() << std::endl;
                    } else {
                        std::cout << "Account not found!" << std::endl;
                    }
                    break;
                    
                case 6:
                    std::cout << "Enter account number: ";
                    std::cin >> accNum;
                    if (Account* acc = bank.findAccount(accNum)) {
                        acc->displayAccountInfo();
                    } else {
                        std::cout << "Account not found!" << std::endl;
                    }
                    break;
                    
                case 7:
                    std::cout << "Enter account number: ";
                    std::cin >> accNum;
                    if (Account* acc = bank.findAccount(accNum)) {
                        acc->displayTransactionHistory();
                    } else {
                        std::cout << "Account not found!" << std::endl;
                    }
                    break;
                    
                case 8:
                    bank.displayAllAccounts();
                    break;
                    
                case 9:
                    bank.applyInterestToSavingsAccounts();
                    break;
                    
                case 10:
                    std::cout << "Thank you for using " << bank.getBankName() 
                              << " Banking System!" << std::endl;
                    return;
                    
                default:
                    std::cout << "Invalid choice! Please try again." << std::endl;
            }
        }
    }
};

// Main function
int main() {
    std::cout << "Welcome to the Banking System!\n";
    
    BankingSystem bankingSystem;
    bankingSystem.run();
    
    return 0;
}