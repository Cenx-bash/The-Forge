#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>

// ============================================
// BASE CLASS: Demonstrating basic encapsulation
// ============================================
class BankAccount {
private:
    // PRIVATE MEMBERS: Fully encapsulated
    std::string accountNumber;
    std::string accountHolder;
    double balance;
    static int totalAccounts;  // Static member for class-level data
    
protected:
    // PROTECTED MEMBERS: Accessible to derived classes only
    double minimumBalance;
    
public:
    // CONSTRUCTORS
    BankAccount(const std::string& holder, double initialDeposit = 0.0) 
        : accountHolder(holder), balance(initialDeposit) {
        // Generate a unique account number
        accountNumber = "ACC" + std::to_string(++totalAccounts);
        minimumBalance = 0.0;
        std::cout << "Account created: " << accountNumber << " for " << accountHolder << std::endl;
    }
    
    // DESTRUCTOR
    virtual ~BankAccount() {
        std::cout << "Account " << accountNumber << " closed." << std::endl;
    }
    
    // PUBLIC INTERFACE: Getter methods (read-only access)
    std::string getAccountNumber() const { return accountNumber; }
    std::string getAccountHolder() const { return accountHolder; }
    double getBalance() const { return balance; }
    static int getTotalAccounts() { return totalAccounts; }
    
    // PUBLIC INTERFACE: Methods to manipulate account with validation
    virtual void deposit(double amount) {
        if (amount <= 0) {
            throw std::invalid_argument("Deposit amount must be positive");
        }
        balance += amount;
        std::cout << "Deposited $" << std::fixed << std::setprecision(2) 
                  << amount << " to account " << accountNumber << std::endl;
    }
    
    virtual void withdraw(double amount) {
        if (amount <= 0) {
            throw std::invalid_argument("Withdrawal amount must be positive");
        }
        if (balance - amount < minimumBalance) {
            throw std::runtime_error("Insufficient funds");
        }
        balance -= amount;
        std::cout << "Withdrawn $" << std::fixed << std::setprecision(2) 
                  << amount << " from account " << accountNumber << std::endl;
    }
    
    virtual void displayInfo() const {
        std::cout << "\n=== Account Information ===" << std::endl;
        std::cout << "Account Number: " << accountNumber << std::endl;
        std::cout << "Account Holder: " << accountHolder << std::endl;
        std::cout << "Balance: $" << std::fixed << std::setprecision(2) << balance << std::endl;
    }
    
    // STATIC METHOD
    static void displayBankStats() {
        std::cout << "\n=== Bank Statistics ===" << std::endl;
        std::cout << "Total Accounts: " << totalAccounts << std::endl;
    }
};

// Initialize static member
int BankAccount::totalAccounts = 0;

// ============================================
// DERIVED CLASS: SavingsAccount (Inheritance)
// ============================================
class SavingsAccount : public BankAccount {
private:
    double interestRate;
    double monthlyWithdrawalLimit;
    double withdrawnThisMonth;
    
public:
    SavingsAccount(const std::string& holder, double initialDeposit, double rate = 2.5)
        : BankAccount(holder, initialDeposit), interestRate(rate),
          monthlyWithdrawalLimit(1000.0), withdrawnThisMonth(0.0) {
        minimumBalance = 100.0;  // Savings account requires minimum balance
    }
    
    // Override withdraw method to add monthly limit check
    void withdraw(double amount) override {
        if (withdrawnThisMonth + amount > monthlyWithdrawalLimit) {
            throw std::runtime_error("Monthly withdrawal limit exceeded");
        }
        
        // Call base class withdraw method
        BankAccount::withdraw(amount);
        withdrawnThisMonth += amount;
    }
    
    // Add interest to the account
    void applyInterest() {
        double interest = getBalance() * (interestRate / 100.0);
        deposit(interest);
        std::cout << "Interest of $" << std::fixed << std::setprecision(2)
                  << interest << " applied to account " << getAccountNumber() << std::endl;
    }
    
    // Reset monthly withdrawal counter
    void resetMonthlyWithdrawal() {
        withdrawnThisMonth = 0.0;
        std::cout << "Monthly withdrawal counter reset for account " 
                  << getAccountNumber() << std::endl;
    }
    
    // Getter for interest rate
    double getInterestRate() const { return interestRate; }
    
    // Override displayInfo to show additional savings account info
    void displayInfo() const override {
        BankAccount::displayInfo();
        std::cout << "Account Type: Savings Account" << std::endl;
        std::cout << "Interest Rate: " << interestRate << "%" << std::endl;
        std::cout << "Monthly Withdrawal Limit: $" << std::fixed << std::setprecision(2) 
                  << monthlyWithdrawalLimit << std::endl;
        std::cout << "Withdrawn This Month: $" << std::fixed << std::setprecision(2) 
                  << withdrawnThisMonth << std::endl;
        std::cout << "Minimum Balance: $" << std::fixed << std::setprecision(2) 
                  << minimumBalance << std::endl;
    }
};

// ============================================
// DERIVED CLASS: CheckingAccount (Inheritance)
// ============================================
class CheckingAccount : public BankAccount {
private:
    double overdraftLimit;
    int freeTransactions;
    int transactionCount;
    
public:
    CheckingAccount(const std::string& holder, double initialDeposit)
        : BankAccount(holder, initialDeposit), overdraftLimit(500.0),
          freeTransactions(10), transactionCount(0) {
        minimumBalance = -overdraftLimit;  // Can go negative up to overdraft limit
    }
    
    // Override withdraw to allow overdraft
    void withdraw(double amount) override {
        if (getBalance() - amount < minimumBalance) {
            throw std::runtime_error("Overdraft limit exceeded");
        }
        
        BankAccount::withdraw(amount);
        transactionCount++;
        
        // Charge fee if exceeded free transactions
        if (transactionCount > freeTransactions) {
            double fee = 2.50;
            BankAccount::withdraw(fee);  // Call base class method directly
            std::cout << "Transaction fee of $" << std::fixed << std::setprecision(2) 
                      << fee << " charged." << std::endl;
        }
    }
    
    // Override deposit as well to count transactions
    void deposit(double amount) override {
        BankAccount::deposit(amount);
        transactionCount++;
        
        // Charge fee if exceeded free transactions
        if (transactionCount > freeTransactions) {
            double fee = 2.50;
            BankAccount::withdraw(fee);
            std::cout << "Transaction fee of $" << std::fixed << std::setprecision(2) 
                      << fee << " charged." << std::endl;
        }
    }
    
    // Reset transaction counter (typically monthly)
    void resetTransactionCount() {
        transactionCount = 0;
        std::cout << "Transaction counter reset for account " 
                  << getAccountNumber() << std::endl;
    }
    
    // Override displayInfo
    void displayInfo() const override {
        BankAccount::displayInfo();
        std::cout << "Account Type: Checking Account" << std::endl;
        std::cout << "Overdraft Limit: $" << std::fixed << std::setprecision(2) 
                  << overdraftLimit << std::endl;
        std::cout << "Free Transactions: " << freeTransactions << std::endl;
        std::cout << "Transactions This Month: " << transactionCount << std::endl;
    }
};

// ============================================
// COMPOSITION EXAMPLE: Customer with multiple accounts
// ============================================
class Customer {
private:
    std::string name;
    std::string customerId;
    std::vector<BankAccount*> accounts;  // Aggregation: Customer "has" accounts
    
public:
    Customer(const std::string& customerName) : name(customerName) {
        static int customerCounter = 1000;
        customerId = "CUST" + std::to_string(customerCounter++);
    }
    
    ~Customer() {
        // Clean up dynamically allocated accounts
        for (auto account : accounts) {
            delete account;
        }
    }
    
    // Getter methods
    std::string getName() const { return name; }
    std::string getCustomerId() const { return customerId; }
    
    // Create a new account for this customer
    template<typename T>
    T* createAccount(double initialDeposit = 0.0) {
        T* newAccount = new T(name, initialDeposit);
        accounts.push_back(newAccount);
        return newAccount;
    }
    
    // Display all accounts for this customer
    void displayAllAccounts() const {
        std::cout << "\n=== Customer: " << name << " (" << customerId << ") ===" << std::endl;
        std::cout << "Number of Accounts: " << accounts.size() << std::endl;
        
        for (const auto& account : accounts) {
            account->displayInfo();
        }
    }
    
    // Get total balance across all accounts
    double getTotalBalance() const {
        double total = 0.0;
        for (const auto& account : accounts) {
            total += account->getBalance();
        }
        return total;
    }
};

// ============================================
// MAIN FUNCTION: Demonstration
// ============================================
int main() {
    std::cout << "=== BANKING SYSTEM DEMONSTRATION ===\n" << std::endl;
    
    try {
        // Create customers
        Customer customer1("John Doe");
        Customer customer2("Jane Smith");
        
        // Customer 1 creates accounts
        std::cout << "\n--- Creating accounts for " << customer1.getName() << " ---" << std::endl;
        SavingsAccount* johnSavings = customer1.createAccount<SavingsAccount>(500.0);
        CheckingAccount* johnChecking = customer1.createAccount<CheckingAccount>(200.0);
        
        // Customer 2 creates accounts
        std::cout << "\n--- Creating accounts for " << customer2.getName() << " ---" << std::endl;
        SavingsAccount* janeSavings = customer2.createAccount<SavingsAccount>(1500.0);
        CheckingAccount* janeChecking = customer2.createAccount<CheckingAccount>(300.0);
        
        // Demonstrate transactions
        std::cout << "\n--- Performing Transactions ---" << std::endl;
        
        // Deposit and withdraw from John's accounts
        johnSavings->deposit(200.0);
        johnSavings->withdraw(50.0);
        johnSavings->applyInterest();
        
        johnChecking->deposit(100.0);
        johnChecking->withdraw(250.0);  // Using overdraft
        
        // Deposit to Jane's account
        janeSavings->deposit(500.0);
        janeSavings->applyInterest();
        
        // Try to exceed withdrawal limit (will throw exception)
        try {
            janeSavings->withdraw(1200.0);  // Exceeds monthly limit
        } catch (const std::runtime_error& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
        
        // Display account information
        std::cout << "\n--- Account Information ---" << std::endl;
        customer1.displayAllAccounts();
        customer2.displayAllAccounts();
        
        // Display total balances
        std::cout << "\n--- Total Balances ---" << std::endl;
        std::cout << customer1.getName() << " total balance: $" 
                  << std::fixed << std::setprecision(2) << customer1.getTotalBalance() << std::endl;
        std::cout << customer2.getName() << " total balance: $" 
                  << std::fixed << std::setprecision(2) << customer2.getTotalBalance() << std::endl;
        
        // Demonstrate polymorphism
        std::cout << "\n--- Polymorphism Demonstration ---" << std::endl;
        std::vector<BankAccount*> allAccounts;
        allAccounts.push_back(johnSavings);
        allAccounts.push_back(johnChecking);
        allAccounts.push_back(janeSavings);
        allAccounts.push_back(janeChecking);
        
        for (auto account : allAccounts) {
            account->displayInfo();
        }
        
        // Bank statistics
        BankAccount::displayBankStats();
        
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n=== PROGRAM END ===" << std::endl;
    return 0;
}
