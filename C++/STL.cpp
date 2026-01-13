#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <set>
#include <queue>
#include <stack>

using namespace std;

int main() {
    cout << "=== STL (Standard Template Library) Basic Examples ===\n\n";
    
    // 1. VECTOR (Dynamic Array)
    cout << "1. VECTOR Examples:\n";
    vector<int> numbers = {5, 2, 8, 1, 9, 3};
    
    cout << "   Original vector: ";
    for(int num : numbers) {
        cout << num << " ";
    }
    cout << endl;
    
    // Add elements
    numbers.push_back(7);
    numbers.push_back(4);
    
    cout << "   After push_back: ";
    for(int num : numbers) {
        cout << num << " ";
    }
    cout << endl;
    
    // Sort vector
    sort(numbers.begin(), numbers.end());
    
    cout << "   Sorted vector: ";
    for(int num : numbers) {
        cout << num << " ";
    }
    cout << endl;
    
    // Access elements
    cout << "   First element: " << numbers.front() << endl;
    cout << "   Last element: " << numbers.back() << endl;
    cout << "   Element at index 3: " << numbers[3] << endl;
    
    // 2. LIST (Doubly Linked List)
    cout << "\n2. LIST Examples:\n";
    list<string> names = {"Alice", "Bob", "Charlie"};
    
    names.push_front("Zara");
    names.push_back("David");
    
    cout << "   List contents: ";
    for(const auto& name : names) {
        cout << name << " ";
    }
    cout << endl;
    
    // 3. MAP (Key-Value Pairs)
    cout << "\n3. MAP Examples:\n";
    map<string, int> ageMap;
    
    ageMap["Alice"] = 25;
    ageMap["Bob"] = 30;
    ageMap["Charlie"] = 35;
    ageMap["David"] = 28;
    
    cout << "   Age Map:\n";
    for(const auto& pair : ageMap) {
        cout << "   " << pair.first << ": " << pair.second << " years old\n";
    }
    
    // Find in map
    string searchName = "Bob";
    auto it = ageMap.find(searchName);
    if(it != ageMap.end()) {
        cout << "   Found " << searchName << " with age " << it->second << endl;
    }
    
    // 4. SET (Unique Sorted Elements)
    cout << "\n4. SET Examples:\n";
    set<int> uniqueNumbers = {5, 2, 5, 8, 2, 1, 9, 1, 3};
    
    cout << "   Unique numbers (automatically sorted): ";
    for(int num : uniqueNumbers) {
        cout << num << " ";
    }
    cout << endl;
    
    // Check if element exists
    if(uniqueNumbers.find(5) != uniqueNumbers.end()) {
        cout << "   Number 5 exists in the set\n";
    }
    
    // 5. QUEUE (FIFO - First In First Out)
    cout << "\n5. QUEUE Examples:\n";
    queue<string> ticketQueue;
    
    ticketQueue.push("Person A");
    ticketQueue.push("Person B");
    ticketQueue.push("Person C");
    
    cout << "   Queue front: " << ticketQueue.front() << endl;
    cout << "   Queue back: " << ticketQueue.back() << endl;
    cout << "   Queue size: " << ticketQueue.size() << endl;
    
    // 6. STACK (LIFO - Last In First Out)
    cout << "\n6. STACK Examples:\n";
    stack<string> books;
    
    books.push("C++ Programming");
    books.push("Data Structures");
    books.push("Algorithms");
    
    cout << "   Top book: " << books.top() << endl;
    books.pop();
    cout << "   After pop, top book: " << books.top() << endl;
    
    // 7. ALGORITHMS
    cout << "\n7. ALGORITHM Examples:\n";
    
    vector<int> nums = {10, 20, 30, 40, 50};
    
    // Find element
    auto findIt = find(nums.begin(), nums.end(), 30);
    if(findIt != nums.end()) {
        cout << "   Found 30 at position: " << distance(nums.begin(), findIt) << endl;
    }
    
    // Count elements
    vector<int> scores = {85, 90, 85, 78, 85, 92};
    int count85 = count(scores.begin(), scores.end(), 85);
    cout << "   Number 85 appears " << count85 << " times\n";
    
    // Reverse vector
    reverse(nums.begin(), nums.end());
    cout << "   Reversed vector: ";
    for(int num : nums) {
        cout << num << " ";
    }
    cout << endl;
    
    // Min and max
    cout << "   Min element: " << *min_element(scores.begin(), scores.end()) << endl;
    cout << "   Max element: " << *max_element(scores.begin(), scores.end()) << endl;
    
    // 8. PAIR
    cout << "\n8. PAIR Examples:\n";
    pair<string, double> student = {"John", 85.5};
    cout << "   Student: " << student.first << ", Grade: " << student.second << endl;
    
    // Pair in vector
    vector<pair<string, int>> inventory = {
        {"Apples", 50},
        {"Oranges", 30},
        {"Bananas", 40}
    };
    
    cout << "   Inventory:\n";
    for(const auto& item : inventory) {
        cout << "   " << item.first << ": " << item.second << " units\n";
    }
    
    cout << "\n=== End of STL Examples ===\n";
    
    return 0;
}
