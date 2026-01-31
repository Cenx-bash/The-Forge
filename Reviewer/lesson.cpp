#include <iostream>
#include <cmath>  // For math functions

// Function declaration
int add(int a, int b);  // Prototype
void greet(std::string name);
double calculateCircleArea(double radius);

// Main function
int main() {
    // 1. Using functions
    int result = add(10, 20);
    std::cout << "Sum: " << result << std::endl;
    
    // 2. Void function
    greet("Alice");
    
    // 3. Function with return value
    double area = calculateCircleArea(5.0);
    std::cout << "Area: " << area << std::endl;
    
    // 4. Function overloading
    std::cout << "Max(10, 20): " << findMax(10, 20) << std::endl;
    std::cout << "Max(10.5, 20.3): " << findMax(10.5, 20.3) << std::endl;
    
    return 0;
}

// Function definitions

// Simple addition function
int add(int a, int b) {
    return a + b;
}

// Void function (no return)
void greet(std::string name) {
    std::cout << "Hello, " << name << "!" << std::endl;
}

// Function with calculation
double calculateCircleArea(double radius) {
    const double PI = 3.14159;
    return PI * radius * radius;
}

// Function overloading - same name, different parameters
int findMax(int a, int b) {
    return (a > b) ? a : b;
}

double findMax(double a, double b) {
    return (a > b) ? a : b;
}
