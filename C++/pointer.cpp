#include <iostream>
#include <cstring>
#include <memory>
#include <vector>
#include <array>
#include <functional>

using namespace std;

// ============================================
// 1. BASIC POINTER CONCEPTS
// ============================================
void basicPointers() {
    cout << "\n=== 1. BASIC POINTER CONCEPTS ===\n";
    
    int x = 42;
    int* ptr = &x;  // ptr holds address of x
    
    cout << "Variable x:\n";
    cout << "  Value: " << x << endl;
    cout << "  Address: " << &x << endl;
    
    cout << "\nPointer ptr:\n";
    cout << "  Value (address stored): " << ptr << endl;
    cout << "  Address of pointer: " << &ptr << endl;
    cout << "  Value pointed to: " << *ptr << endl;
    
    // Modifying value through pointer
    *ptr = 100;
    cout << "\nAfter *ptr = 100:\n";
    cout << "  x is now: " << x << endl;
    
    // Pointer to pointer
    int** pptr = &ptr;
    cout << "\nPointer to pointer:\n";
    cout << "  pptr value: " << pptr << endl;
    cout << "  *pptr value: " << *pptr << endl;
    cout << "  **pptr value: " << **pptr << endl;
    
    // Null pointer
    int* nullPtr = nullptr;
    if(nullPtr == nullptr) {
        cout << "\nnullPtr is null\n";
    }
}

// ============================================
// 2. POINTER ARITHMETIC
// ============================================
void pointerArithmetic() {
    cout << "\n=== 2. POINTER ARITHMETIC ===\n";
    
    int arr[] = {10, 20, 30, 40, 50};
    int* ptr = arr;  // Points to first element
    
    cout << "Array: ";
    for(int i = 0; i < 5; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    
    cout << "\nPointer arithmetic:\n";
    cout << "  ptr     points to: " << *ptr << endl;
    cout << "  ptr+1   points to: " << *(ptr + 1) << endl;
    cout << "  ptr+2   points to: " << *(ptr + 2) << endl;
    cout << "  ptr[3]  (array notation): " << ptr[3] << endl;
    cout << "  *(arr+4): " << *(arr + 4) << endl;
    
    // Increment/decrement
    ptr++;  // Move to next element
    cout << "\nAfter ptr++:\n";
    cout << "  ptr points to: " << *ptr << endl;
    
    ptr--;  // Move back
    cout << "After ptr--:\n";
    cout << "  ptr points to: " << *ptr << endl;
    
    // Pointer differences
    int* start = arr;
    int* end = arr + 5;
    cout << "\nNumber of elements: " << (end - start) << endl;
    
    // Character pointer (special case)
    char str[] = "Hello";
    char* cptr = str;
    cout << "\nCharacter pointer:\n";
    cout << "  String: " << str << endl;
    cout << "  cptr: " << cptr << endl;
    cout << "  cptr+1: " << (cptr + 1) << endl;
    cout << "  *cptr: " << *cptr << endl;
    cout << "  *(cptr+1): " << *(cptr + 1) << endl;
}

// ============================================
// 3. DYNAMIC MEMORY ALLOCATION
// ============================================
void dynamicMemory() {
    cout << "\n=== 3. DYNAMIC MEMORY ALLOCATION ===\n";
    
    // Single variable
    int* single = new int(25);
    cout << "Dynamically allocated single int:\n";
    cout << "  Value: " << *single << endl;
    cout << "  Address: " << single << endl;
    
    // Array
    int* arr = new int[5];
    for(int i = 0; i < 5; i++) {
        arr[i] = i * 10;
    }
    
    cout << "\nDynamically allocated array:\n";
    for(int i = 0; i < 5; i++) {
        cout << "  arr[" << i << "] = " << arr[i] << endl;
    }
    
    // 2D array
    int rows = 3, cols = 4;
    int** matrix = new int*[rows];
    for(int i = 0; i < rows; i++) {
        matrix[i] = new int[cols];
        for(int j = 0; j < cols; j++) {
            matrix[i][j] = i * 10 + j;
        }
    }
    
    cout << "\n2D dynamic array (3x4):\n";
    for(int i = 0; i < rows; i++) {
        cout << "  ";
        for(int j = 0; j < cols; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    
    // MUST FREE MEMORY!
    delete single;
    delete[] arr;
    
    for(int i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    
    cout << "\nMemory freed successfully\n";
}

// ============================================
// 4. POINTERS WITH FUNCTIONS
// ============================================
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void modifyArray(int* arr, int size) {
    for(int i = 0; i < size; i++) {
        arr[i] *= 2;  // Double each element
    }
}

int* createArray(int size) {
    int* arr = new int[size];
    for(int i = 0; i < size; i++) {
        arr[i] = i * 5;
    }
    return arr;  // Return pointer to heap memory
}

void functionPointers() {
    cout << "\n=== 4. POINTERS WITH FUNCTIONS ===\n";
    
    // Passing pointers to functions
    int x = 5, y = 10;
    cout << "Before swap: x = " << x << ", y = " << y << endl;
    swap(&x, &y);
    cout << "After swap: x = " << x << ", y = " << y << endl;
    
    // Array and pointer parameter
    int arr[] = {1, 2, 3, 4, 5};
    cout << "\nBefore modifyArray: ";
    for(int val : arr) cout << val << " ";
    
    modifyArray(arr, 5);
    
    cout << "\nAfter modifyArray: ";
    for(int val : arr) cout << val << " ";
    cout << endl;
    
    // Function returning pointer
    int* dynamicArr = createArray(5);
    cout << "\nArray from createArray: ";
    for(int i = 0; i < 5; i++) {
        cout << dynamicArr[i] << " ";
    }
    cout << endl;
    delete[] dynamicArr;
    
    // Function pointers
    cout << "\nFunction Pointers:\n";
    
    // Declare function pointer
    void (*funcPtr)(int*, int) = nullptr;
    
    // Assign function to pointer
    funcPtr = modifyArray;
    
    // Call function through pointer
    int testArr[] = {10, 20, 30};
    cout << "Before function pointer call: ";
    for(int val : testArr) cout << val << " ";
    
    funcPtr(testArr, 3);
    
    cout << "\nAfter function pointer call: ";
    for(int val : testArr) cout << val << " ";
    cout << endl;
}

// ============================================
// 5. POINTERS TO STRUCTURES AND CLASSES
// ============================================
struct Point {
    int x;
    int y;
    
    void display() const {
        cout << "(" << x << ", " << y << ")";
    }
};

class Rectangle {
private:
    double width;
    double height;
    
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    double area() const {
        return width * height;
    }
    
    void scale(double factor) {
        width *= factor;
        height *= factor;
    }
    
    void display() const {
        cout << "Rectangle: " << width << " x " << height 
             << " (Area: " << area() << ")";
    }
};

void structureClassPointers() {
    cout << "\n=== 5. POINTERS TO STRUCTURES AND CLASSES ===\n";
    
    // Structure pointer
    Point p = {10, 20};
    Point* ptr = &p;
    
    cout << "Structure pointer:\n";
    cout << "  Access with (*ptr).x: " << (*ptr).x << endl;
    cout << "  Access with ptr->x: " << ptr->x << endl;
    cout << "  Access with ptr->y: " << ptr->y << endl;
    
    // Arrow operator for methods
    cout << "  Calling method: ";
    ptr->display();
    cout << endl;
    
    // Dynamic structure allocation
    Point* dynamicPoint = new Point{30, 40};
    cout << "\nDynamic structure:\n";
    cout << "  Point: ";
    dynamicPoint->display();
    delete dynamicPoint;
    
    // Class pointer
    Rectangle rect(5, 10);
    Rectangle* rectPtr = &rect;
    
    cout << "\nClass pointer:\n";
    rectPtr->display();
    cout << endl;
    
    rectPtr->scale(2);
    cout << "After scaling by 2: ";
    rectPtr->display();
    cout << endl;
    
    // Dynamic class allocation
    Rectangle* dynamicRect = new Rectangle(3, 4);
    cout << "\nDynamic class object:\n";
    cout << "  Area: " << dynamicRect->area() << endl;
    delete dynamicRect;
}

// ============================================
// 6. VOID POINTERS AND TYPE CASTING
// ============================================
void voidPointers() {
    cout << "\n=== 6. VOID POINTERS AND TYPE CASTING ===\n";
    
    // Void pointer can point to any type
    int intValue = 42;
    double doubleValue = 3.14159;
    char charValue = 'A';
    
    void* voidPtr;
    
    voidPtr = &intValue;
    cout << "Void pointer to int:\n";
    cout << "  Value: " << *(static_cast<int*>(voidPtr)) << endl;
    
    voidPtr = &doubleValue;
    cout << "Void pointer to double:\n";
    cout << "  Value: " << *(static_cast<double*>(voidPtr)) << endl;
    
    voidPtr = &charValue;
    cout << "Void pointer to char:\n";
    cout << "  Value: " << *(static_cast<char*>(voidPtr)) << endl;
    
    // Type casting with pointers
    cout << "\nType casting:\n";
    
    // C-style cast
    int* intPtr = (int*)voidPtr;
    
    // static_cast (compile-time check)
    double d = 3.14;
    voidPtr = &d;
    double* dblPtr = static_cast<double*>(voidPtr);
    cout << "  static_cast: " << *dblPtr << endl;
    
    // reinterpret_cast (bit-level reinterpretation)
    long value = 0x12345678;
    char* bytePtr = reinterpret_cast<char*>(&value);
    cout << "  reinterpret_cast - first byte: " 
         << hex << (int)*bytePtr << endl;
    
    // const_cast (add/remove const)
    const int constVal = 100;
    const int* constPtr = &constVal;
    
    // Remove const (use with caution!)
    int* mutablePtr = const_cast<int*>(constPtr);
    cout << "  Original const value: " << dec << constVal << endl;
    
    // dynamic_cast (for polymorphism)
    cout << "  Note: dynamic_cast requires polymorphic classes\n";
}

// ============================================
// 7. SMART POINTERS (C++11+)
// ============================================
#include <memory>

class Resource {
private:
    int id;
    
public:
    Resource(int i) : id(i) {
        cout << "  Resource " << id << " created\n";
    }
    
    ~Resource() {
        cout << "  Resource " << id << " destroyed\n";
    }
    
    void use() {
        cout << "  Using Resource " << id << endl;
    }
};

void smartPointers() {
    cout << "\n=== 7. SMART POINTERS (C++11+) ===\n";
    
    // unique_ptr - exclusive ownership
    cout << "\nunique_ptr (exclusive ownership):\n";
    {
        unique_ptr<Resource> uptr1(new Resource(1));
        uptr1->use();
        
        // Transfer ownership
        unique_ptr<Resource> uptr2 = move(uptr1);
        
        if(!uptr1) {
            cout << "  uptr1 is now empty\n";
        }
        
        uptr2->use();
    }  // Automatically deleted
    
    // shared_ptr - shared ownership with reference counting
    cout << "\nshared_ptr (shared ownership):\n";
    {
        shared_ptr<Resource> sptr1(new Resource(2));
        cout << "  Use count: " << sptr1.use_count() << endl;
        
        {
            shared_ptr<Resource> sptr2 = sptr1;
            cout << "  Use count after sharing: " << sptr1.use_count() << endl;
            sptr2->use();
        }
        
        cout << "  Use count after scope: " << sptr1.use_count() << endl;
        sptr1->use();
    }
    
    // weak_ptr - non-owning reference
    cout << "\nweak_ptr (non-owning reference):\n";
    {
        shared_ptr<Resource> shared(new Resource(3));
        weak_ptr<Resource> weak = shared;
        
        cout << "  shared use count: " << shared.use_count() << endl;
        
        if(auto temp = weak.lock()) {
            cout << "  Accessed via weak_ptr\n";
            temp->use();
        }
        
        shared.reset();
        
        if(weak.expired()) {
            cout << "  Resource destroyed, weak_ptr expired\n";
        }
    }
    
    // make_unique and make_shared (C++14/C++11)
    cout << "\nmake_unique and make_shared:\n";
    auto uptr = make_unique<Resource>(4);
    auto sptr = make_shared<Resource>(5);
    uptr->use();
    sptr->use();
}

// ============================================
// 8. POINTERS AND ARRAYS (ADVANCED)
// ============================================
void advancedArrayPointers() {
    cout << "\n=== 8. POINTERS AND ARRAYS (ADVANCED) ===\n";
    
    // Multidimensional arrays
    int matrix[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };
    
    cout << "2D array with pointers:\n";
    for(int i = 0; i < 3; i++) {
        cout << "  Row " << i << ": ";
        for(int j = 0; j < 4; j++) {
            cout << *(*(matrix + i) + j) << " ";
        }
        cout << endl;
    }
    
    // Array of pointers
    cout << "\nArray of pointers:\n";
    int a = 10, b = 20, c = 30;
    int* ptrArray[3] = {&a, &b, &c};
    
    for(int i = 0; i < 3; i++) {
        cout << "  ptrArray[" << i << "] points to: " << *ptrArray[i] << endl;
    }
    
    // Pointer to array
    int (*arrayPtr)[4] = matrix;  // Pointer to array of 4 ints
    cout << "\nPointer to array:\n";
    cout << "  First row, third element: " << (*arrayPtr)[2] << endl;
    
    // Command line arguments style
    cout << "\nSimulating command line arguments:\n";
    const char* argv[] = {"program", "arg1", "arg2", "arg3"};
    int argc = 4;
    
    for(int i = 0; i < argc; i++) {
        cout << "  argv[" << i << "] = " << argv[i] << endl;
    }
}

// ============================================
// 9. COMMON POINTER PITFALLS
// ============================================
void pointerPitfalls() {
    cout << "\n=== 9. COMMON POINTER PITFALLS ===\n";
    
    cout << "1. Dangling Pointers:\n";
    {
        int* ptr = new int(42);
        delete ptr;  // Memory freed
        
        // ptr is now a dangling pointer!
        // *ptr = 100;  // UNDEFINED BEHAVIOR!
        
        // Solution: Set to nullptr
        ptr = nullptr;
        cout << "  Pointer set to nullptr after delete\n";
    }
    
    cout << "\n2. Memory Leaks:\n";
    {
        // Memory allocated but never freed
        int* leak = new int(99);
        // delete leak;  // FORGOT TO DELETE - MEMORY LEAK!
        cout << "  Memory leaked (intentionally for demo)\n";
    }
    
    cout << "\n3. Double Free:\n";
    {
        int* ptr = new int(50);
        delete ptr;
        // delete ptr;  // CRASH! Double free
        cout << "  Avoid deleting same pointer twice\n";
    }
    
    cout << "\n4. Uninitialized Pointers:\n";
    {
        int* uninitialized;  // Contains garbage address
        // *uninitialized = 10;  // UNDEFINED BEHAVIOR!
        cout << "  Always initialize pointers\n";
    }
    
    cout << "\n5. Returning pointer to local variable:\n";
    {
        // BAD: Returning address of local variable
        auto badFunction = []() -> int* {
            int local = 42;
            return &local;  // WRONG! local will be destroyed
        };
        cout << "  Never return pointer to local variable\n";
    }
}

// ============================================
// 10. POINTER ALIASING AND RESTRICT
// ============================================
void pointerAliasing() {
    cout << "\n=== 10. POINTER ALIASING ===\n";
    
    // Pointer aliasing - multiple pointers to same memory
    int value = 100;
    int* ptr1 = &value;
    int* ptr2 = &value;  // Aliased pointer
    
    cout << "Pointer aliasing example:\n";
    cout << "  Original value: " << value << endl;
    
    *ptr1 = 200;
    cout << "  After *ptr1 = 200:\n";
    cout << "    value: " << value << endl;
    cout << "    *ptr2: " << *ptr2 << endl;
    
    // restrict keyword (C99, not standard C++)
    // Tells compiler pointers don't alias
    cout << "\nrestrict keyword (C concept):\n";
    cout << "  In C, restrict tells compiler pointers don't alias\n";
    cout << "  Allows better optimization\n";
    cout << "  Not part of standard C++ but some compilers support\n";
}

// ============================================
// 11. MODERN C++ POINTER PRACTICES
// ============================================
void modernPractices() {
    cout << "\n=== 11. MODERN C++ POINTER PRACTICES ===\n";
    
    cout << "1. Prefer smart pointers over raw pointers:\n";
    {
        // Old way (dangerous):
        // int* ptr = new int(42);
        // delete ptr;
        
        // Modern way:
        auto smartPtr = make_unique<int>(42);
        cout << "  Smart pointer value: " << *smartPtr << endl;
        // No delete needed!
    }
    
    cout << "\n2. Use containers instead of raw arrays:\n";
    {
        // Old way:
        // int* arr = new int[5];
        // delete[] arr;
        
        // Modern way:
        vector<int> vec = {1, 2, 3, 4, 5};
        cout << "  Vector: ";
        for(int val : vec) cout << val << " ";
        cout << endl;
    }
    
    cout << "\n3. Use references when possible:\n";
    {
        int value = 100;
        
        // Use reference when you don't need null or reassignment
        int& ref = value;
        ref = 200;
        cout << "  Using reference: " << value << endl;
        
        // Use pointer when you need null or reassignment
        int* ptr = &value;
        if(ptr) {  // Can check for null
            *ptr = 300;
        }
        cout << "  Using pointer: " << value << endl;
    }
    
    cout << "\n4. Use nullptr instead of NULL or 0:\n";
    {
        int* ptr1 = nullptr;  // Good - type safe
        // int* ptr2 = NULL;   // Old C way
        // int* ptr3 = 0;      // Bad - implicit conversion
        
        if(ptr1 == nullptr) {
            cout << "  ptr1 is null (using nullptr)\n";
        }
    }
    
    cout << "\n5. Use auto with pointers:\n";
    {
        auto ptr = make_unique<int>(42);
        auto arr = make_unique<int[]>(5);
        
        cout << "  auto with unique_ptr: " << *ptr << endl;
    }
}

// ============================================
// 12. FUNCTIONAL PROGRAMMING WITH POINTERS
// ============================================
void functionalPointers() {
    cout << "\n=== 12. FUNCTIONAL PROGRAMMING WITH POINTERS ===\n";
    
    // Function pointers for callbacks
    cout << "Function pointers as callbacks:\n";
    
    // Typedef for function pointer
    typedef void (*Callback)(int);
    
    // Functions to use as callbacks
    auto printSquare = [](int x) {
        cout << "  Square: " << x * x << endl;
    };
    
    auto printCube = [](int x) {
        cout << "  Cube: " << x * x * x << endl;
    };
    
    // Function that takes callback
    auto processNumber = [](int n, Callback callback) {
        cout << "Processing " << n << ":\n";
        callback(n);
    };
    
    processNumber(5, printSquare);
    processNumber(5, printCube);
    
    // Array of function pointers
    cout << "\nArray of function pointers:\n";
    Callback callbacks[] = {printSquare, printCube};
    
    for(int i = 0; i < 2; i++) {
        callbacks[i](3);
    }
    
    // Modern alternative: std::function
    cout << "\nUsing std::function (modern alternative):\n";
    function<void(int)> func = printSquare;
    func(4);
    
    // Function that returns function pointer
    cout << "\nFunction returning function pointer:\n";
    auto getMultiplier = [](int factor) -> function<int(int)> {
        return [factor](int x) { return x * factor; };
    };
    
    auto doubleFunc = getMultiplier(2);
    auto tripleFunc = getMultiplier(3);
    
    cout << "  Double of 5: " << doubleFunc(5) << endl;
    cout << "  Triple of 5: " << tripleFunc(5) << endl;
}

// ============================================
// MAIN FUNCTION
// ============================================
int main() {
    cout << "=== COMPREHENSIVE C++ POINTERS GUIDE ===\n";
    
    basicPointers();
    pointerArithmetic();
    dynamicMemory();
    functionPointers();
    structureClassPointers();
    voidPointers();
    smartPointers();
    advancedArrayPointers();
    pointerPitfalls();
    pointerAliasing();
    modernPractices();
    functionalPointers();
    
    cout << "\n=== POINTERS SUMMARY ===\n";
    cout << "1. Pointers store memory addresses\n";
    cout << "2. Use * to dereference (get value at address)\n";
    cout << "3. Use & to get address of variable\n";
    cout << "4. Use -> to access members through pointer\n";
    cout << "5. Always match new with delete, new[] with delete[]\n";
    cout << "6. Prefer smart pointers (unique_ptr, shared_ptr)\n";
    cout << "7. Use nullptr instead of NULL\n";
    cout << "8. Set pointers to nullptr after delete\n";
    cout << "9. Be aware of pointer arithmetic limitations\n";
    cout << "10. Understand pointer vs reference usage\n";
    
    return 0;
}
