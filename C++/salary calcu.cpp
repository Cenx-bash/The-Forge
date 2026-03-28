#include <iostream>
using namespace std;

int main() {
    double hours, rate, salary;

    cout << "Enter hours worked (-1 to end): ";
    cin >> hours;

    while (hours != -1) {
        cout << "Enter hourly rate: ";
        cin >> rate;

        if (hours <= 40) {
            salary = hours * rate;
        } else {
            salary = (40 * rate) + ((hours - 40) * rate * 1.5);
        }

        cout << "Salary is: $" << salary << endl;

        cout << "\nEnter hours worked (-1 to end): ";
        cin >> hours;
    }

    return 0;
}