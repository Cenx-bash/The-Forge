#include <iostream>
using namespace std;

int main() {
    double sales, salary;

    cout << "Enter sales in dollars (-1 to end): ";
    cin >> sales;

    while (sales != -1) {
        salary = 200 + (0.09 * sales);

        cout << "Salary is: $" << salary << endl;

        cout << "\nEnter sales in dollars (-1 to end): ";
        cin >> sales;
    }

    return 0;
}