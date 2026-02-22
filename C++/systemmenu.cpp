#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

// Function prototypes
void displayMenu(const vector<string>& menuItems, const vector<double>& menuPrices);
void displaySystemMenu();
void addItemToReceipt(vector<string>& receiptItems, vector<double>& receiptPrices, 
                     const vector<string>& menuItems, const vector<double>& menuPrices);
void removeItemFromReceipt(vector<string>& receiptItems, vector<double>& receiptPrices);
void viewReceipt(const vector<string>& receiptItems, const vector<double>& receiptPrices);
void clearReceipt(vector<string>& receiptItems, vector<double>& receiptPrices);
bool isValidIndex(int index, const vector<string>& vec);
void waitForUser();

int main() {
    // Part 1: Setup the Menu (Fixed)
    vector<string> menuItems = {"Burger", "Fries", "Coke", "Chicken", "Fish Sandwich", "Ice Cream"};
    vector<double> menuPrices = {99.00, 49.00, 39.00, 129.00, 89.00, 59.00};
    
    // Customer Receipt (Dynamic)
    vector<string> receiptItems;
    vector<double> receiptPrices;
    
    int choice;
    
    cout << "\n========================================\n";
    cout << "   WELCOME TO FOOD ORDERING KIOSK\n";
    cout << "========================================\n";
    
    // Part 2: Menu-Driven Program using do-while loop
    do {
        displaySystemMenu();
        cout << "Enter your choice (1-7): ";
        cin >> choice;
        
        // Clear screen simulation (for Windows)
        system("cls");
        
        switch(choice) {
            case 1: // View Menu
                displayMenu(menuItems, menuPrices);
                waitForUser();
                break;
                
            case 2: // Add Item to Receipt
                addItemToReceipt(receiptItems, receiptPrices, menuItems, menuPrices);
                waitForUser();
                break;
                
            case 3: // Remove Item from Receipt
                removeItemFromReceipt(receiptItems, receiptPrices);
                waitForUser();
                break;
                
            case 4: // View Receipt
                viewReceipt(receiptItems, receiptPrices);
                waitForUser();
                break;
                
            case 5: // Clear Entire Receipt
                clearReceipt(receiptItems, receiptPrices);
                waitForUser();
                break;
                
            case 6: // Show Receipt Summary
                if (receiptItems.empty()) {
                    cout << "\n📝 Your receipt is empty.\n";
                } else {
                    cout << "\n📝 Current items in receipt: " << receiptItems.size() << "\n";
                }
                waitForUser();
                break;
                
            case 7: // Exit
                cout << "\nThank you for using our kiosk! Have a great day!\n";
                cout << "========================================\n";
                break;
                
            default:
                cout << "\n❌ Invalid choice! Please try again.\n";
                waitForUser();
        }
        
    } while (choice != 7);
    
    return 0;
}

// Function to display the system menu
void displaySystemMenu() {
    cout << "\n============= SYSTEM MENU =============\n";
    cout << "1. 🍔 View Menu\n";
    cout << "2. ➕ Add Item to Receipt\n";
    cout << "3. ➖ Remove Item from Receipt\n";
    cout << "4. 🧾 View Receipt\n";
    cout << "5. 🧹 Clear Entire Receipt\n";
    cout << "6. 📊 Show Receipt Summary\n";
    cout << "7. 🚪 Exit\n";
    cout << "========================================\n";
}

// Function to display the restaurant menu
void displayMenu(const vector<string>& menuItems, const vector<double>& menuPrices) {
    cout << "\n=========== RESTAURANT MENU ===========\n";
    cout << setw(5) << "Index" << setw(20) << "Item" << setw(15) << "Price" << endl;
    cout << "========================================\n";
    
    // Using size() inside loop
    for (size_t i = 0; i < menuItems.size(); i++) {
        cout << setw(5) << i 
             << setw(20) << menuItems[i] 
             << setw(15) << fixed << setprecision(2) << "₱" << menuPrices[i] << endl;
    }
    cout << "========================================\n";
}

// Function to add item to receipt (supports multiple quantities)
void addItemToReceipt(vector<string>& receiptItems, vector<double>& receiptPrices, 
                     const vector<string>& menuItems, const vector<double>& menuPrices) {
    
    displayMenu(menuItems, menuPrices);
    
    int menuIndex, quantity;
    
    cout << "\nEnter menu item number to add: ";
    cin >> menuIndex;
    
    // Validate index input
    if (!isValidIndex(menuIndex, menuItems)) {
        cout << "\n❌ Invalid menu item number!\n";
        return;
    }
    
    cout << "Enter quantity: ";
    cin >> quantity;
    
    // Validate quantity
    if (quantity <= 0) {
        cout << "\n❌ Invalid quantity!\n";
        return;
    }
    
    // Using push_back() to add items multiple times
    for (int i = 0; i < quantity; i++) {
        receiptItems.push_back(menuItems[menuIndex]);
        receiptPrices.push_back(menuPrices[menuIndex]);
    }
    
    cout << "\n✅ Added " << quantity << " x " << menuItems[menuIndex] 
         << " to your receipt!\n";
}

// Function to remove item from receipt
void removeItemFromReceipt(vector<string>& receiptItems, vector<double>& receiptPrices) {
    
    // Check if receipt is empty using empty()
    if (receiptItems.empty()) {
        cout << "\n❌ No items in receipt to remove!\n";
        return;
    }
    
    cout << "\n=========== CURRENT RECEIPT ===========\n";
    cout << setw(5) << "Index" << setw(20) << "Item" << setw(15) << "Price" << endl;
    cout << "========================================\n";
    
    for (size_t i = 0; i < receiptItems.size(); i++) {
        cout << setw(5) << i 
             << setw(20) << receiptItems[i] 
             << setw(15) << fixed << setprecision(2) << "₱" << receiptPrices[i] << endl;
    }
    cout << "========================================\n";
    
    int receiptIndex;
    cout << "Enter receipt item number to remove: ";
    cin >> receiptIndex;
    
    // Validate receipt index
    if (!isValidIndex(receiptIndex, receiptItems)) {
        cout << "\n❌ Invalid receipt item number!\n";
        return;
    }
    
    string removedItem = receiptItems[receiptIndex];
    
    // Using erase() to remove items
    receiptItems.erase(receiptItems.begin() + receiptIndex);
    receiptPrices.erase(receiptPrices.begin() + receiptIndex);
    
    cout << "\n✅ Removed " << removedItem << " from your receipt!\n";
}

// Function to view receipt and calculate total
void viewReceipt(const vector<string>& receiptItems, const vector<double>& receiptPrices) {
    
    cout << "\n============ YOUR RECEIPT =============\n";
    
    // Check if receipt is empty using empty()
    if (receiptItems.empty()) {
        cout << "   No items ordered yet.\n";
        cout << "========================================\n";
        return;
    }
    
    cout << setw(5) << "#" << setw(20) << "Item" << setw(15) << "Price" << endl;
    cout << "========================================\n";
    
    double total = 0.0;
    
    // Using loop to traverse vectors and compute total
    for (size_t i = 0; i < receiptItems.size(); i++) {
        cout << setw(5) << i + 1 
             << setw(20) << receiptItems[i] 
             << setw(15) << fixed << setprecision(2) << "₱" << receiptPrices[i] << endl;
        total += receiptPrices[i];
    }
    
    cout << "========================================\n";
    cout << setw(25) << "TOTAL:" << setw(15) << fixed << setprecision(2) << "₱" << total << endl;
    cout << "========================================\n";
}

// Function to clear entire receipt
void clearReceipt(vector<string>& receiptItems, vector<double>& receiptPrices) {
    
    if (receiptItems.empty()) {
        cout << "\n❌ Receipt is already empty!\n";
        return;
    }
    
    char confirm;
    cout << "\n⚠️  Are you sure you want to clear the entire receipt? (y/n): ";
    cin >> confirm;
    
    if (tolower(confirm) == 'y') {
        receiptItems.clear();
        receiptPrices.clear();
        cout << "\n✅ Receipt cleared successfully!\n";
    } else {
        cout << "\n❌ Operation cancelled.\n";
    }
}

// Helper function to validate index
bool isValidIndex(int index, const vector<string>& vec) {
    return index >= 0 && index < static_cast<int>(vec.size());
}

// Helper function to wait for user input
void waitForUser() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    system("cls");
}
