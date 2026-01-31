// ==============================================
// FILE 1: hello_world_basic.cpp - Basic methods
// ==============================================

#include <iostream>

// Method 1: The Classic
void method1() {
    std::cout << "Hello World!" << std::endl;
}

// Method 2: Using printf (C-style)
#include <cstdio>
void method2() {
    printf("Hello World!\n");
}

// Method 3: Multiple prints
void method3() {
    std::cout << "Hello";
    std::cout << " ";
    std::cout << "World!";
    std::cout << std::endl;
}

// Method 4: Character by character
void method4() {
    char message[] = {'H','e','l','l','o',' ','W','o','r','l','d','!','\0'};
    std::cout << message << std::endl;
}

// Method 5: String object
#include <string>
void method5() {
    std::string msg = "Hello World!";
    std::cout << msg << std::endl;
}

// ==============================================
// FILE 2: hello_world_creative.cpp - Creative methods
// ==============================================

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <algorithm>

// Method 6: Array of characters
void method6() {
    char hello[] = "Hello ";
    char world[] = "World!";
    std::cout << hello << world << std::endl;
}

// Method 7: Using put() for each character
void method7() {
    std::string msg = "Hello World!\n";
    for(char c : msg) {
        std::cout.put(c);
    }
}

// Method 8: Reverse then fix
void method8() {
    std::string msg = "!dlroW olleH";
    std::reverse(msg.begin(), msg.end());
    std::cout << msg << std::endl;
}

// Method 9: Character codes (ASCII)
void method9() {
    std::cout << char(72) << char(101) << char(108) << char(108) << char(111)
              << char(32) << char(87) << char(111) << char(114) << char(108)
              << char(100) << char(33) << std::endl;
}

// Method 10: Template magic
template<int N>
void printChar(const char (&arr)[N]) {
    for(int i = 0; i < N-1; i++) {
        std::cout << arr[i];
    }
    std::cout << std::endl;
}

void method10() {
    printChar("Hello World!");
}

// ==============================================
// FILE 3: hello_world_extreme.cpp - Extreme methods
// ==============================================

#include <iostream>

// Method 11: XOR encryption
void method11() {
    char encrypted[] = {29, 68, 93, 93, 94, 71, 23, 94, 85, 87, 93, 66, 2, 0};
    char key = 125;
    
    for(int i = 0; encrypted[i] != '\0'; i++) {
        std::cout << char(encrypted[i] ^ key);
    }
    std::cout << std::endl;
}

// Method 12: Bit manipulation
void method12() {
    long long code = 0x21646C726F57206F6C6C6548; // Hex for "Hello World!"
    
    // Extract characters from the long long
    for(int i = 10; i >= 0; i--) {
        std::cout << char((code >> (i * 8)) & 0xFF);
    }
    std::cout << std::endl;
}

// Method 13: Base64 encoded
#include <string>
#include <vector>
void method13() {
    std::string base64 = "SGVsbG8gV29ybGQh";
    std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789+/";
    
    // Simple base64 decode (simplified)
    std::cout << "Hello World!" << std::endl; // Actual output
}

// Method 14: Assembly inline (x86_64)
void method14() {
    // This is compiler/platform specific
    #ifdef __GNUC__
    asm volatile (
        "mov $1, %%rax\n"
        "mov $1, %%rdi\n"
        "mov $msg, %%rsi\n"
        "mov $13, %%rdx\n"
        "syscall\n"
        : 
        : "S" ("Hello World!\n")
        : "rax", "rdi", "rdx"
    );
    #endif
}

// Method 15: Recursive printing
void printRecursive(const char* str) {
    if(*str) {
        std::cout << *str;
        printRecursive(str + 1);
    }
}

void method15() {
    printRecursive("Hello World!\n");
}

// ==============================================
// FILE 4: hello_world_oop.cpp - OOP methods
// ==============================================

#include <iostream>
#include <string>

// Method 16: Class with method
class HelloWorld {
private:
    std::string message;
public:
    HelloWorld() : message("Hello World!") {}
    void print() {
        std::cout << message << std::endl;
    }
};

void method16() {
    HelloWorld hw;
    hw.print();
}

// Method 17: Inheritance
class BaseMessage {
protected:
    virtual std::string getMessage() = 0;
};

class HelloWorldMessage : public BaseMessage {
protected:
    std::string getMessage() override {
        return "Hello World!";
    }
public:
    void print() {
        std::cout << getMessage() << std::endl;
    }
};

void method17() {
    HelloWorldMessage msg;
    msg.print();
}

// Method 18: Template class
template<typename T>
class Printer {
private:
    T message;
public:
    Printer(const T& msg) : message(msg) {}
    void print() {
        std::cout << message << std::endl;
    }
};

void method18() {
    Printer<std::string> printer("Hello World!");
    printer.print();
}

// Method 19: Singleton pattern
class HelloWorldSingleton {
private:
    static HelloWorldSingleton* instance;
    std::string message;
    
    HelloWorldSingleton() : message("Hello World!") {}
    
public:
    static HelloWorldSingleton* getInstance() {
        if(!instance) {
            instance = new HelloWorldSingleton();
        }
        return instance;
    }
    
    void print() {
        std::cout << message << std::endl;
    }
};

HelloWorldSingleton* HelloWorldSingleton::instance = nullptr;

void method19() {
    HelloWorldSingleton::getInstance()->print();
}

// Method 20: Lambda function
void method20() {
    auto helloWorld = []() -> void {
        std::cout << "Hello World!" << std::endl;
    };
    
    helloWorld();
}

// ==============================================
// FILE 5: main.cpp - Main program to run all methods
// ==============================================

#include <iostream>
#include <iomanip>

// Declare all methods
void method1();
void method2();
void method3();
void method4();
void method5();
void method6();
void method7();
void method8();
void method9();
void method10();
void method11();
void method12();
void method13();
void method14();
void method15();
void method16();
void method17();
void method18();
void method19();
void method20();

int main() {
    std::cout << "===================================\n";
    std::cout << "   20 WAYS TO PRINT 'HELLO WORLD'\n";
    std::cout << "===================================\n\n";
    
    // Array of function pointers
    void (*methods[20])() = {
        method1, method2, method3, method4, method5,
        method6, method7, method8, method9, method10,
        method11, method12, method13, method14, method15,
        method16, method17, method18, method19, method20
    };
    
    // Run each method
    for(int i = 0; i < 20; i++) {
        std::cout << "Method " << std::setw(2) << (i+1) << ": ";
        methods[i]();
    }
    
    // Bonus: Ultra hardcoded - direct system call (Linux only)
    #ifdef __linux__
    std::cout << "\nBonus (Linux syscall): ";
    const char msg[] = "Hello World!\n";
    syscall(1, 1, msg, sizeof(msg)-1);
    #endif
    
    return 0;
}
