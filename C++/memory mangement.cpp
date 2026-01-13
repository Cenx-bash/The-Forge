#include <iostream>
#include <cstring>     // For memset, memcpy
#include <memory>      // For smart pointers
#include <vector>
#include <new>         // For bad_alloc, placement new

using namespace std;

// Custom class for demonstration
class Resource {
private:
    int id;
    char* data;
    
public:
    Resource(int id, const char* text) : id(id) {
        cout << "  Constructor called for Resource " << id << endl;
        data = new char[strlen(text) + 1];
        strcpy(data, text);
    }
    
    ~Resource() {
        cout << "  Destructor called for Resource " << id << endl;
        delete[] data;
    }
    
    void display() const {
        cout << "  Resource " << id << ": " << data << endl;
    }
    
    int getId() const { return id; }
};

// ============================================
// 1. STACK vs HEAP Allocation
// ============================================
void stackVsHeap() {
    cout << "\n=== 1. STACK vs HEAP Allocation ===\n";
    
    // Stack allocation (automatic memory management)
    cout << "\na) STACK Allocation:\n";
    {
        int stackVar = 42;                    // Allocated on stack
        int stackArray[5] = {1, 2, 3, 4, 5};  // Allocated on stack
        Resource stackResource(1, "Stack");   // Constructor called
        
        cout << "  Stack variable: " << stackVar << endl;
        cout << "  Stack array[2]: " << stackArray[2] << endl;
        stackResource.display();
        
        // Automatic destruction when scope ends
        // Destructor automatically called for stackResource
    }
    cout << "  Stack objects automatically destroyed\n";
    
    // Heap allocation (manual memory management)
    cout << "\nb) HEAP Allocation:\n";
    {
        int* heapVar = new int(100);          // Allocated on heap
        int* heapArray = new int[5]{10, 20, 30, 40, 50};
        Resource* heapResource = new Resource(2, "Heap");
        
        cout << "  Heap variable: " << *heapVar << endl;
        cout << "  Heap array[2]: " << heapArray[2] << endl;
        heapResource->display();
        
        // MANUAL cleanup required!
        delete heapVar;                       // Free single int
        delete[] heapArray;                   // Free array
        delete heapResource;                  // Free object (calls destructor)
        
        cout << "  Heap objects manually deleted\n";
    }
}

// ============================================
// 2. Common Memory Issues
// ============================================
void memoryIssues() {
    cout << "\n=== 2. Common Memory Issues ===\n";
    
    // a) Memory leak
    cout << "\na) Memory Leak:\n";
    for(int i = 0; i < 3; i++) {
        int* leak = new int(i);
        // OOPS! Forgot to delete - MEMORY LEAK!
        // Correct: delete leak;
    }
    cout << "  3 memory blocks leaked!\n";
    
    // b) Dangling pointer
    cout << "\nb) Dangling Pointer:\n";
    int* ptr = new int(42);
    cout << "  Original value: " << *ptr << endl;
    delete ptr;  // Memory freed
    
    // ptr is now a dangling pointer!
    // *ptr = 100; // UNDEFINED BEHAVIOR!
    
    // Solution: Set pointer to null after delete
    ptr = nullptr;
    cout << "  Pointer set to nullptr after delete\n";
    
    // c) Double delete
    cout << "\nc) Double Delete:\n";
    int* doublePtr = new int(99);
    delete doublePtr;
    // delete doublePtr; // CRASH! Double delete
    
    doublePtr = nullptr;  // Prevent double delete
    delete doublePtr;     // Safe - deleting nullptr is OK
    cout << "  Deleting nullptr is safe\n";
}

// ============================================
// 3. SMART POINTERS (C++11 and later)
// ============================================
void smartPointers() {
    cout << "\n=== 3. SMART POINTERS ===\n";
    
    // a) unique_ptr (exclusive ownership)
    cout << "\na) unique_ptr (exclusive ownership):\n";
    {
        unique_ptr<Resource> uptr1(new Resource(1, "unique_ptr1"));
        uptr1->display();
        
        // unique_ptr cannot be copied, only moved
        unique_ptr<Resource> uptr2 = move(uptr1);
        
        if(!uptr1) {
            cout << "  uptr1 is now empty (ownership transferred)\n";
        }
        
        uptr2->display();
        // Automatically deleted when scope ends
    }
    cout << "  unique_ptr automatically deleted\n";
    
    // b) shared_ptr (shared ownership with reference counting)
    cout << "\nb) shared_ptr (shared ownership):\n";
    {
        shared_ptr<Resource> sptr1(new Resource(2, "shared_ptr"));
        cout << "  Use count: " << sptr1.use_count() << endl;
        
        {
            shared_ptr<Resource> sptr2 = sptr1;  // Copy increases ref count
            cout << "  Use count after copy: " << sptr1.use_count() << endl;
            sptr2->display();
        }  // sptr2 destroyed, ref count decreases
        
        cout << "  Use count after sptr2 destroyed: " << sptr1.use_count() << endl;
        sptr1->display();
        // Automatically deleted when ref count reaches 0
    }
    cout << "  shared_ptr automatically deleted\n";
    
    // c) weak_ptr (non-owning reference to shared_ptr)
    cout << "\nc) weak_ptr (non-owning reference):\n";
    {
        shared_ptr<Resource> shared(new Resource(3, "shared for weak"));
        weak_ptr<Resource> weak = shared;
        
        cout << "  shared use count: " << shared.use_count() << endl;
        
        // Convert weak_ptr to shared_ptr to access
        if(auto temp = weak.lock()) {
            cout << "  Access via weak_ptr successful\n";
            temp->display();
        }
        
        // Reset shared_ptr
        shared.reset();
        
        if(!weak.lock()) {
            cout << "  Object destroyed, weak_ptr expired\n";
        }
    }
}

// ============================================
// 4. RAW POINTER OPERATIONS
// ============================================
void rawPointerOperations() {
    cout << "\n=== 4. RAW POINTER OPERATIONS ===\n";
    
    // a) new and delete for arrays
    cout << "\na) Array Allocation:\n";
    {
        int size = 5;
        int* arr = new int[size];  // Allocate array
        
        for(int i = 0; i < size; i++) {
            arr[i] = i * 10;
        }
        
        cout << "  Array: ";
        for(int i = 0; i < size; i++) {
            cout << arr[i] << " ";
        }
        cout << endl;
        
        delete[] arr;  // Use delete[] for arrays
    }
    
    // b) Pointer arithmetic
    cout << "\nb) Pointer Arithmetic:\n";
    {
        int arr[] = {10, 20, 30, 40, 50};
        int* ptr = arr;
        
        cout << "  *ptr: " << *ptr << endl;
        cout << "  *(ptr + 2): " << *(ptr + 2) << endl;
        cout << "  ptr[3]: " << ptr[3] << endl;
        
        ptr++;
        cout << "  After ptr++: " << *ptr << endl;
    }
    
    // c) memset and memcpy
    cout << "\nc) memset and memcpy:\n";
    {
        int arr1[5];
        int arr2[5] = {1, 2, 3, 4, 5};
        
        // Set all bytes to 0
        memset(arr1, 0, sizeof(arr1));
        cout << "  After memset - arr1[0]: " << arr1[0] << endl;
        
        // Copy memory
        memcpy(arr1, arr2, sizeof(arr2));
        cout << "  After memcpy - arr1[2]: " << arr1[2] << endl;
    }
}

// ============================================
// 5. MEMORY MANAGEMENT PATTERNS
// ============================================
void memoryPatterns() {
    cout << "\n=== 5. MEMORY MANAGEMENT PATTERNS ===\n";
    
    // a) RAII (Resource Acquisition Is Initialization)
    cout << "\na) RAII Pattern:\n";
    {
        // Resource managed by object lifetime
        Resource res1(1, "RAII managed");
        res1.display();
        // Automatically destroyed when scope ends
    }
    cout << "  Resource automatically cleaned up\n";
    
    // b) Copy constructor and assignment (Rule of Three)
    cout << "\nb) Deep Copy Example:\n";
    {
        Resource original(2, "Original");
        Resource copy = original;  // Copy constructor (should be implemented!)
        copy.display();
    }
    
    // c) Move semantics (C++11)
    cout << "\nc) Move Semantics (C++11):\n";
    {
        vector<Resource> resources;
        
        // Without move (copy)
        Resource temp1(3, "Temporary");
        // resources.push_back(temp1);  // Copy
        
        // With move
        resources.push_back(Resource(4, "Moved"));
        cout << "  Resource moved into vector\n";
    }
}

// ============================================
// 6. ADVANCED TOPICS
// ============================================
void advancedTopics() {
    cout << "\n=== 6. ADVANCED TOPICS ===\n";
    
    // a) Placement new
    cout << "\na) Placement New:\n";
    {
        // Allocate memory
        void* memory = malloc(sizeof(Resource));
        
        // Construct object in pre-allocated memory
        Resource* obj = new(memory) Resource(1, "Placement New");
        obj->display();
        
        // Explicitly call destructor
        obj->~Resource();
        
        // Free memory
        free(memory);
        cout << "  Placement new completed\n";
    }
    
    // b) Custom memory allocator
    cout << "\nb) Custom Memory Allocator Pattern:\n";
    {
        class MemoryPool {
        private:
            static const int POOL_SIZE = 100;
            char pool[POOL_SIZE];
            size_t offset;
            
        public:
            MemoryPool() : offset(0) {}
            
            void* allocate(size_t size) {
                if(offset + size > POOL_SIZE) {
                    throw bad_alloc();
                }
                void* ptr = pool + offset;
                offset += size;
                return ptr;
            }
            
            void reset() {
                offset = 0;
            }
        };
        
        MemoryPool pool;
        int* num = (int*)pool.allocate(sizeof(int));
        *num = 42;
        cout << "  Allocated from pool: " << *num << endl;
    }
    
    // c) Alignment
    cout << "\nc) Memory Alignment:\n";
    {
        struct alignas(16) AlignedStruct {
            int a;
            double b;
            char c;
        };
        
        AlignedStruct obj;
        cout << "  Alignment of struct: " << alignof(AlignedStruct) << " bytes\n";
        cout << "  Size of struct: " << sizeof(AlignedStruct) << " bytes\n";
    }
}

// ============================================
// 7. BEST PRACTICES DEMONSTRATION
// ============================================
void bestPractices() {
    cout << "\n=== 7. BEST PRACTICES ===\n";
    
    // 1. Prefer stack allocation when possible
    cout << "\n1. Prefer stack over heap:\n";
    {
        Resource local(1, "Local stack object");  // Good
        // vs Resource* heap = new Resource(...);  // Avoid if possible
    }
    
    // 2. Use smart pointers for heap allocation
    cout << "\n2. Use smart pointers:\n";
    {
        unique_ptr<Resource> resource = make_unique<Resource>(2, "Smart managed");
        resource->display();
    }
    
    // 3. Use containers instead of raw arrays
    cout << "\n3. Use containers:\n";
    {
        vector<int> numbers = {1, 2, 3, 4, 5};  // Good
        // vs int* arr = new int[5];             // Avoid
        
        for(auto num : numbers) {
            cout << "  " << num;
        }
        cout << endl;
    }
    
    // 4. Avoid manual memory management
    cout << "\n4. Avoid manual new/delete:\n";
    {
        // BAD
        // Resource* r = new Resource();
        // ... use r ...
        // delete r;
        
        // GOOD
        auto r = make_unique<Resource>(3, "Auto-managed");
        r->display();
    }
    
    // 5. Check allocation failures
    cout << "\n5. Handle allocation failures:\n";
    try {
        // Large allocation that might fail
        char* huge = new char[1000000000000LL];  // Might throw bad_alloc
        delete[] huge;
    } catch(const bad_alloc& e) {
        cout << "  Allocation failed: " << e.what() << endl;
    }
}

int main() {
    cout << "=== C++ MEMORY MANAGEMENT GUIDE ===\n";
    
    // Run all demonstrations
    stackVsHeap();
    memoryIssues();
    smartPointers();
    rawPointerOperations();
    memoryPatterns();
    advancedTopics();
    bestPractices();
    
    cout << "\n=== MEMORY MANAGEMENT SUMMARY ===\n";
    cout << "1. Use stack for small, short-lived objects\n";
    cout << "2. Use smart pointers (unique_ptr, shared_ptr) for heap objects\n";
    cout << "3. Follow RAII principle\n";
    cout << "4. Avoid raw new/delete when possible\n";
    cout << "5. Use containers (vector, array) instead of raw arrays\n";
    cout << "6. Always match new with delete and new[] with delete[]\n";
    cout << "7. Set pointers to nullptr after deletion\n";
    cout << "8. Be aware of ownership semantics\n";
    
    // Memory leak detection hint
    #ifdef _DEBUG
    cout << "\nDebug Tip: Use Valgrind or AddressSanitizer to detect leaks\n";
    #endif
    
    return 0;
}
