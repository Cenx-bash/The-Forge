#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

// ============================================
// 1. BASIC CLASS WITH METHODS
// ============================================
class BasicCalculator {
private:
    double lastResult;
    
public:
    // Constructor
    BasicCalculator() : lastResult(0) {
        cout << "BasicCalculator created\n";
    }
    
    // Destructor
    ~BasicCalculator() {
        cout << "BasicCalculator destroyed\n";
    }
    
    // 1. Regular public method
    double add(double a, double b) {
        lastResult = a + b;
        return lastResult;
    }
    
    // 2. Method with return value
    double subtract(double a, double b) {
        lastResult = a - b;
        return lastResult;
    }
    
    // 3. Const method (doesn't modify object state)
    double getLastResult() const {
        return lastResult;
    }
    
    // 4. Method returning reference
    double& getLastResultRef() {
        return lastResult;
    }
    
    // 5. Static method (belongs to class, not object)
    static void displayHelp() {
        cout << "Basic Calculator Operations:\n";
        cout << "  add(a, b) - Addition\n";
        cout << "  subtract(a, b) - Subtraction\n";
    }
};

// ============================================
// 2. METHOD TYPES AND QUALIFIERS
// ============================================
class BankAccount {
private:
    string accountNumber;
    double balance;
    mutable int accessCount;  // Can be modified in const methods
    
public:
    BankAccount(string accNum, double initialBalance) 
        : accountNumber(accNum), balance(initialBalance), accessCount(0) {}
    
    // Regular methods
    void deposit(double amount) {
        if(amount > 0) {
            balance += amount;
            cout << "Deposited: $" << amount << endl;
        }
    }
    
    bool withdraw(double amount) {
        if(amount > 0 && balance >= amount) {
            balance -= amount;
            cout << "Withdrawn: $" << amount << endl;
            return true;
        }
        return false;
    }
    
    // Const method - can't modify member variables (except mutable)
    double getBalance() const {
        accessCount++;  // OK because accessCount is mutable
        return balance;
    }
    
    // Const method that returns const reference
    const string& getAccountNumber() const {
        return accountNumber;
    }
    
    // Static method - can be called without object
    static double calculateInterest(double principal, double rate, int years) {
        return principal * pow(1 + rate/100, years);
    }
    
    // Inline method (defined inside class)
    void displayInfo() const {
        cout << "Account: " << accountNumber 
             << ", Balance: $" << balance << endl;
    }
    
    // Method with default arguments
    void transfer(BankAccount& toAccount, double amount, 
                  double fee = 5.0, string description = "Transfer") {
        if(withdraw(amount + fee)) {
            toAccount.deposit(amount);
            cout << description << " completed with fee: $" << fee << endl;
        }
    }
};

// ============================================
// 3. METHOD OVERLOADING
// ============================================
class MathOperations {
public:
    // Overloaded methods - same name, different parameters
    
    // Add two integers
    int add(int a, int b) {
        cout << "Adding integers: ";
        return a + b;
    }
    
    // Add three integers
    int add(int a, int b, int c) {
        cout << "Adding three integers: ";
        return a + b + c;
    }
    
    // Add two doubles
    double add(double a, double b) {
        cout << "Adding doubles: ";
        return a + b;
    }
    
    // Add array of integers
    int add(const int arr[], int size) {
        cout << "Adding array elements: ";
        int sum = 0;
        for(int i = 0; i < size; i++) {
            sum += arr[i];
        }
        return sum;
    }
    
    // Add variable number of integers (using initializer list - C++11)
    int add(initializer_list<int> numbers) {
        cout << "Adding variable numbers: ";
        int sum = 0;
        for(int num : numbers) {
            sum += num;
        }
        return sum;
    }
};

// ============================================
// 4. CONSTRUCTORS AND DESTRUCTORS
// ============================================
class Student {
private:
    string name;
    int age;
    vector<string> courses;
    static int totalStudents;  // Static member
    
public:
    // Static method to access static member
    static int getTotalStudents() {
        return totalStudents;
    }
    
    // Default constructor
    Student() : name("Unknown"), age(18) {
        totalStudents++;
        cout << "Default constructor called\n";
    }
    
    // Parameterized constructor
    Student(string n, int a) : name(n), age(a) {
        totalStudents++;
        cout << "Parameterized constructor called\n";
    }
    
    // Constructor with initializer list
    Student(string n, int a, initializer_list<string> crs) 
        : name(n), age(a), courses(crs) {
        totalStudents++;
        cout << "Constructor with initializer list called\n";
    }
    
    // Copy constructor
    Student(const Student& other) 
        : name(other.name), age(other.age), courses(other.courses) {
        totalStudents++;
        cout << "Copy constructor called\n";
    }
    
    // Move constructor (C++11)
    Student(Student&& other) noexcept 
        : name(move(other.name)), age(other.age), courses(move(other.courses)) {
        totalStudents++;
        cout << "Move constructor called\n";
    }
    
    // Destructor
    ~Student() {
        totalStudents--;
        cout << "Destructor called for " << name << endl;
    }
    
    // Copy assignment operator
    Student& operator=(const Student& other) {
        if(this != &other) {
            name = other.name;
            age = other.age;
            courses = other.courses;
        }
        cout << "Copy assignment operator called\n";
        return *this;
    }
    
    // Move assignment operator (C++11)
    Student& operator=(Student&& other) noexcept {
        if(this != &other) {
            name = move(other.name);
            age = other.age;
            courses = move(other.courses);
        }
        cout << "Move assignment operator called\n";
        return *this;
    }
    
    // Display method
    void display() const {
        cout << "Student: " << name << ", Age: " << age << endl;
        cout << "Courses: ";
        for(const auto& course : courses) {
            cout << course << " ";
        }
        cout << endl;
    }
    
    // Getter methods
    string getName() const { return name; }
    int getAge() const { return age; }
    
    // Setter methods
    void setName(const string& n) { name = n; }
    void setAge(int a) { 
        if(a >= 0) age = a; 
    }
    
    // Method to add course
    void addCourse(const string& course) {
        courses.push_back(course);
    }
    
    // Method returning reference to member
    vector<string>& getCourses() {
        return courses;
    }
    
    // Const version of above method
    const vector<string>& getCourses() const {
        return courses;
    }
};

// Initialize static member
int Student::totalStudents = 0;

// ============================================
// 5. OPERATOR OVERLOADING METHODS
// ============================================
class ComplexNumber {
private:
    double real;
    double imag;
    
public:
    ComplexNumber(double r = 0, double i = 0) : real(r), imag(i) {}
    
    // Getter methods
    double getReal() const { return real; }
    double getImag() const { return imag; }
    
    // Operator overloading as member methods
    
    // Addition operator
    ComplexNumber operator+(const ComplexNumber& other) const {
        return ComplexNumber(real + other.real, imag + other.imag);
    }
    
    // Subtraction operator
    ComplexNumber operator-(const ComplexNumber& other) const {
        return ComplexNumber(real - other.real, imag - other.imag);
    }
    
    // Multiplication operator
    ComplexNumber operator*(const ComplexNumber& other) const {
        return ComplexNumber(
            real * other.real - imag * other.imag,
            real * other.imag + imag * other.real
        );
    }
    
    // Compound addition assignment
    ComplexNumber& operator+=(const ComplexNumber& other) {
        real += other.real;
        imag += other.imag;
        return *this;
    }
    
    // Equality operator
    bool operator==(const ComplexNumber& other) const {
        return real == other.real && imag == other.imag;
    }
    
    // Inequality operator
    bool operator!=(const ComplexNumber& other) const {
        return !(*this == other);
    }
    
    // Prefix increment (++c)
    ComplexNumber& operator++() {
        ++real;
        return *this;
    }
    
    // Postfix increment (c++)
    ComplexNumber operator++(int) {
        ComplexNumber temp = *this;
        ++(*this);
        return temp;
    }
    
    // Function call operator - makes object callable
    ComplexNumber operator()(double r, double i) {
        real = r;
        imag = i;
        return *this;
    }
    
    // Display method
    void display() const {
        cout << real << (imag >= 0 ? " + " : " - ") << abs(imag) << "i";
    }
};

// ============================================
// 6. FRIEND FUNCTIONS AND METHODS
// ============================================
class Rectangle {
private:
    double length;
    double width;
    
public:
    Rectangle(double l = 0, double w = 0) : length(l), width(w) {}
    
    // Friend function declaration
    friend bool areEqual(const Rectangle& r1, const Rectangle& r2);
    
    // Friend class declaration
    friend class RectangleHelper;
    
    // Method to access private members
    double area() const {
        return length * width;
    }
    
private:
    // Private method - only accessible within class
    void validateDimensions() {
        if(length < 0) length = 0;
        if(width < 0) width = 0;
    }
    
public:
    // Public method that uses private method
    void setDimensions(double l, double w) {
        length = l;
        width = w;
        validateDimensions();  // Calling private method
    }
};

// Friend function definition
bool areEqual(const Rectangle& r1, const Rectangle& r2) {
    // Can access private members because it's a friend
    return r1.length == r2.length && r1.width == r2.width;
}

// Friend class
class RectangleHelper {
public:
    static double getLength(const Rectangle& rect) {
        return rect.length;  // Can access private member
    }
    
    static double getWidth(const Rectangle& rect) {
        return rect.width;   // Can access private member
    }
};

// ============================================
// 7. VIRTUAL METHODS AND POLYMORPHISM
// ============================================
class Shape {
protected:
    string color;
    
public:
    Shape(const string& c = "black") : color(c) {}
    
    // Virtual method - can be overridden
    virtual double area() const {
        cout << "Shape area (base class) - ";
        return 0;
    }
    
    // Pure virtual method - makes class abstract
    virtual void draw() const = 0;
    
    // Virtual destructor - IMPORTANT for polymorphism
    virtual ~Shape() {
        cout << "Shape destructor\n";
    }
    
    // Non-virtual method
    string getColor() const {
        return color;
    }
};

class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(double r, const string& c = "red") : Shape(c), radius(r) {}
    
    // Override virtual method
    double area() const override {
        cout << "Circle area - ";
        return 3.14159 * radius * radius;
    }
    
    void draw() const override {
        cout << "Drawing a " << color << " circle with radius " << radius << endl;
    }
    
    ~Circle() {
        cout << "Circle destructor\n";
    }
};

class RectangleShape : public Shape {
private:
    double length;
    double width;
    
public:
    RectangleShape(double l, double w, const string& c = "blue") 
        : Shape(c), length(l), width(w) {}
    
    // Override virtual method
    double area() const override {
        cout << "Rectangle area - ";
        return length * width;
    }
    
    void draw() const override {
        cout << "Drawing a " << color << " rectangle " 
             << length << "x" << width << endl;
    }
    
    ~RectangleShape() {
        cout << "Rectangle destructor\n";
    }
};

// ============================================
// 8. TEMPLATE METHODS
// ============================================
class DataProcessor {
public:
    // Template method inside class
    template<typename T>
    T process(const T& a, const T& b) {
        return a + b;  // Works for any type with + operator
    }
    
    // Template method with different types
    template<typename T1, typename T2>
    auto mixedProcess(const T1& a, const T2& b) -> decltype(a + b) {
        return a + b;
    }
    
    // Const template method
    template<typename T>
    void display(const T& value) const {
        cout << "Value: " << value << endl;
    }
};

// Template class with methods
template<typename T>
class Container {
private:
    T data;
    
public:
    Container(const T& d) : data(d) {}
    
    T getData() const {
        return data;
    }
    
    void setData(const T& d) {
        data = d;
    }
    
    // Template method inside template class
    template<typename U>
    U convertTo() const {
        return static_cast<U>(data);
    }
};

// ============================================
// 9. LAMBDA EXPRESSIONS AND FUNCTION OBJECTS
// ============================================
class Multiplier {
private:
    int factor;
    
public:
    Multiplier(int f) : factor(f) {}
    
    // Function call operator - makes object callable
    int operator()(int x) const {
        return x * factor;
    }
};

// ============================================
// DEMONSTRATION FUNCTION
// ============================================
void demonstrateMethods() {
    cout << "=== METHODS IN C++ DEMONSTRATION ===\n\n";
    
    // 1. Basic Calculator
    cout << "1. BASIC CALCULATOR METHODS:\n";
    BasicCalculator calc;
    cout << "Addition: " << calc.add(10, 5) << endl;
    cout << "Subtraction: " << calc.subtract(10, 5) << endl;
    cout << "Last result: " << calc.getLastResult() << endl;
    BasicCalculator::displayHelp();
    
    // 2. Bank Account Methods
    cout << "\n2. BANK ACCOUNT METHODS:\n";
    BankAccount acc1("ACC123", 1000);
    BankAccount acc2("ACC456", 500);
    
    acc1.deposit(200);
    acc1.withdraw(100);
    cout << "Balance: $" << acc1.getBalance() << endl;
    
    acc1.transfer(acc2, 300);
    cout << "Interest on $1000 at 5% for 3 years: $" 
         << BankAccount::calculateInterest(1000, 5, 3) << endl;
    
    // 3. Method Overloading
    cout << "\n3. METHOD OVERLOADING:\n";
    MathOperations math;
    cout << math.add(10, 20) << endl;
    cout << math.add(10.5, 20.5) << endl;
    cout << math.add(10, 20, 30) << endl;
    
    int arr[] = {1, 2, 3, 4, 5};
    cout << math.add(arr, 5) << endl;
    cout << math.add({1, 2, 3, 4, 5, 6}) << endl;
    
    // 4. Constructors and Destructors
    cout << "\n4. CONSTRUCTORS AND DESTRUCTORS:\n";
    {
        Student s1;  // Default constructor
        Student s2("Alice", 20);  // Parameterized constructor
        Student s3("Bob", 22, {"Math", "Physics", "CS"});  // With initializer list
        
        s1.display();
        s2.display();
        s3.display();
        
        cout << "Total students: " << Student::getTotalStudents() << endl;
        
        Student s4 = s2;  // Copy constructor
        Student s5 = move(s3);  // Move constructor
        
        s4.setName("Alice Clone");
        s4.display();
    }  // Destructors called here
    
    // 5. Operator Overloading
    cout << "\n5. OPERATOR OVERLOADING METHODS:\n";
    ComplexNumber c1(3, 4);
    ComplexNumber c2(1, 2);
    
    ComplexNumber c3 = c1 + c2;
    cout << "c1 + c2 = ";
    c3.display();
    cout << endl;
    
    ComplexNumber c4 = c1 * c2;
    cout << "c1 * c2 = ";
    c4.display();
    cout << endl;
    
    c1 += c2;
    cout << "After c1 += c2: ";
    c1.display();
    cout << endl;
    
    ++c2;
    cout << "After ++c2: ";
    c2.display();
    cout << endl;
    
    // 6. Friend Functions
    cout << "\n6. FRIEND FUNCTIONS AND METHODS:\n";
    Rectangle r1(10, 5);
    Rectangle r2(10, 5);
    Rectangle r3(5, 10);
    
    cout << "r1 and r2 are equal: " << boolalpha << areEqual(r1, r2) << endl;
    cout << "r1 and r3 are equal: " << areEqual(r1, r3) << endl;
    cout << "Rectangle area: " << r1.area() << endl;
    
    // 7. Virtual Methods and Polymorphism
    cout << "\n7. VIRTUAL METHODS AND POLYMORPHISM:\n";
    Shape* shapes[2];
    shapes[0] = new Circle(5);
    shapes[1] = new RectangleShape(4, 6);
    
    for(int i = 0; i < 2; i++) {
        cout << "Area: " << shapes[i]->area() << endl;
        shapes[i]->draw();
        delete shapes[i];
    }
    
    // 8. Template Methods
    cout << "\n8. TEMPLATE METHODS:\n";
    DataProcessor processor;
    cout << "Process integers: " << processor.process(10, 20) << endl;
    cout << "Process doubles: " << processor.process(10.5, 20.5) << endl;
    cout << "Mixed process: " << processor.mixedProcess(10, 20.5) << endl;
    
    Container<int> intContainer(42);
    Container<string> strContainer("Hello");
    cout << "Int container: " << intContainer.getData() << endl;
    cout << "String container: " << strContainer.getData() << endl;
    cout << "Convert to double: " << intContainer.convertTo<double>() << endl;
    
    // 9. Lambda-like function objects
    cout << "\n9. FUNCTION OBJECTS:\n";
    Multiplier times2(2);
    Multiplier times3(3);
    
    cout << "5 * 2 = " << times2(5) << endl;
    cout << "5 * 3 = " << times3(5) << endl;
    
    // Lambda expressions (C++11)
    auto lambda = [](int x, int y) -> int {
        return x * y;
    };
    
    cout << "Lambda result: " << lambda(6, 7) << endl;
}

// ============================================
// MAIN FUNCTION
// ============================================
int main() {
    demonstrateMethods();
    
    cout << "\n=== METHOD TYPES SUMMARY ===\n";
    cout << "1. Instance Methods - Called on objects\n";
    cout << "2. Static Methods - Called on class\n";
    cout << "3. Const Methods - Can't modify object\n";
    cout << "4. Virtual Methods - Enable polymorphism\n";
    cout << "5. Pure Virtual Methods - Abstract classes\n";
    cout << "6. Overloaded Methods - Same name, different params\n";
    cout << "7. Operator Methods - Overload operators\n";
    cout << "8. Template Methods - Work with any type\n";
    cout << "9. Friend Methods - Access private members\n";
    cout << "10. Inline Methods - Defined in class\n";
    
    return 0;
}
