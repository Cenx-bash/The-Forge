// ======================================
// BASIC TASK MANAGEMENT SYSTEM
// ======================================
// A simple console-based task manager

#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <iomanip>

using namespace std;

// ======================================
// DATA STRUCTURES
// ======================================

enum class TaskPriority {
    LOW,
    MEDIUM,
    HIGH
};

struct Task {
    int id;
    string description;
    bool isCompleted;
    TaskPriority priority;
    string dueDate; // Format: YYYY-MM-DD
};

// ======================================
// FUNCTION PROTOTYPES
// ======================================

void displayMenu();
void addTask(vector<Task>& tasks);
void viewTasks(const vector<Task>& tasks);
void markTaskCompleted(vector<Task>& tasks);
void deleteTask(vector<Task>& tasks);
void searchTask(const vector<Task>& tasks);
void displayTask(const Task& task);
string priorityToString(TaskPriority priority);
void sortTasks(vector<Task>& tasks);
void saveTasksToFile(const vector<Task>& tasks);
void loadTasksFromFile(vector<Task>& tasks);

// ======================================
// GLOBAL VARIABLES
// ======================================

int nextTaskId = 1;

// ======================================
// MAIN FUNCTION
// ======================================

int main() {
    vector<Task> tasks;
    int choice;
    
    cout << "===================================\n";
    cout << "      TASK MANAGEMENT SYSTEM\n";
    cout << "===================================\n";
    
    // Load existing tasks from file
    loadTasksFromFile(tasks);
    
    do {
        displayMenu();
        cout << "Enter your choice (1-8): ";
        cin >> choice;
        cin.ignore(); // Clear newline character
        
        switch(choice) {
            case 1:
                addTask(tasks);
                break;
            case 2:
                viewTasks(tasks);
                break;
            case 3:
                markTaskCompleted(tasks);
                break;
            case 4:
                deleteTask(tasks);
                break;
            case 5:
                searchTask(tasks);
                break;
            case 6:
                sortTasks(tasks);
                cout << "Tasks sorted!\n";
                break;
            case 7:
                saveTasksToFile(tasks);
                cout << "Tasks saved to file!\n";
                break;
            case 8:
                saveTasksToFile(tasks);
                cout << "Goodbye! Tasks have been saved.\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
        
        cout << endl;
        
    } while(choice != 8);
    
    return 0;
}

// ======================================
// FUNCTION DEFINITIONS
// ======================================

void displayMenu() {
    cout << "\n=== MAIN MENU ===\n";
    cout << "1. Add New Task\n";
    cout << "2. View All Tasks\n";
    cout << "3. Mark Task as Completed\n";
    cout << "4. Delete Task\n";
    cout << "5. Search Tasks\n";
    cout << "6. Sort Tasks\n";
    cout << "7. Save Tasks to File\n";
    cout << "8. Exit\n";
    cout << "=================\n";
}

void addTask(vector<Task>& tasks) {
    Task newTask;
    int priorityChoice;
    
    cout << "\n=== ADD NEW TASK ===\n";
    
    newTask.id = nextTaskId++;
    
    cout << "Enter task description: ";
    getline(cin, newTask.description);
    
    cout << "Select priority (1-Low, 2-Medium, 3-High): ";
    cin >> priorityChoice;
    cin.ignore();
    
    switch(priorityChoice) {
        case 1:
            newTask.priority = TaskPriority::LOW;
            break;
        case 2:
            newTask.priority = TaskPriority::MEDIUM;
            break;
        case 3:
            newTask.priority = TaskPriority::HIGH;
            break;
        default:
            newTask.priority = TaskPriority::MEDIUM;
            cout << "Invalid choice. Defaulting to Medium priority.\n";
    }
    
    cout << "Enter due date (YYYY-MM-DD): ";
    getline(cin, newTask.dueDate);
    
    newTask.isCompleted = false;
    
    tasks.push_back(newTask);
    
    cout << "Task added successfully! (ID: " << newTask.id << ")\n";
}

void viewTasks(const vector<Task>& tasks) {
    if(tasks.empty()) {
        cout << "\nNo tasks found!\n";
        return;
    }
    
    cout << "\n=== ALL TASKS ===\n";
    cout << "-----------------------------------------------------------------\n";
    cout << left << setw(5) << "ID" 
         << setw(30) << "Description" 
         << setw(12) << "Priority"
         << setw(15) << "Due Date"
         << setw(10) << "Status" << endl;
    cout << "-----------------------------------------------------------------\n";
    
    for(const auto& task : tasks) {
        cout << left << setw(5) << task.id
             << setw(30) << (task.description.length() > 28 ? 
                             task.description.substr(0, 27) + "..." : task.description)
             << setw(12) << priorityToString(task.priority)
             << setw(15) << task.dueDate
             << setw(10) << (task.isCompleted ? "Done" : "Pending")
             << endl;
    }
    cout << "-----------------------------------------------------------------\n";
    cout << "Total tasks: " << tasks.size() << endl;
}

void markTaskCompleted(vector<Task>& tasks) {
    int taskId;
    bool found = false;
    
    if(tasks.empty()) {
        cout << "\nNo tasks to mark as completed!\n";
        return;
    }
    
    cout << "\n=== MARK TASK COMPLETED ===\n";
    cout << "Enter task ID to mark as completed: ";
    cin >> taskId;
    cin.ignore();
    
    for(auto& task : tasks) {
        if(task.id == taskId) {
            if(task.isCompleted) {
                cout << "Task is already marked as completed!\n";
            } else {
                task.isCompleted = true;
                cout << "Task marked as completed!\n";
            }
            found = true;
            break;
        }
    }
    
    if(!found) {
        cout << "Task with ID " << taskId << " not found!\n";
    }
}

void deleteTask(vector<Task>& tasks) {
    int taskId;
    bool found = false;
    
    if(tasks.empty()) {
        cout << "\nNo tasks to delete!\n";
        return;
    }
    
    cout << "\n=== DELETE TASK ===\n";
    cout << "Enter task ID to delete: ";
    cin >> taskId;
    cin.ignore();
    
    auto it = remove_if(tasks.begin(), tasks.end(),
        [taskId](const Task& task) {
            return task.id == taskId;
        });
    
    if(it != tasks.end()) {
        tasks.erase(it, tasks.end());
        cout << "Task deleted successfully!\n";
    } else {
        cout << "Task with ID " << taskId << " not found!\n";
    }
}

void searchTask(const vector<Task>& tasks) {
    string searchTerm;
    bool found = false;
    
    if(tasks.empty()) {
        cout << "\nNo tasks to search!\n";
        return;
    }
    
    cout << "\n=== SEARCH TASKS ===\n";
    cout << "Enter search term (description): ";
    getline(cin, searchTerm);
    
    // Convert search term to lowercase for case-insensitive search
    transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::tolower);
    
    cout << "\nSearch Results:\n";
    cout << "-----------------------------------------------------------------\n";
    cout << left << setw(5) << "ID" 
         << setw(30) << "Description" 
         << setw(12) << "Priority"
         << setw(15) << "Due Date"
         << setw(10) << "Status" << endl;
    cout << "-----------------------------------------------------------------\n";
    
    for(const auto& task : tasks) {
        string descLower = task.description;
        transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);
        
        if(descLower.find(searchTerm) != string::npos) {
            cout << left << setw(5) << task.id
                 << setw(30) << (task.description.length() > 28 ? 
                                 task.description.substr(0, 27) + "..." : task.description)
                 << setw(12) << priorityToString(task.priority)
                 << setw(15) << task.dueDate
                 << setw(10) << (task.isCompleted ? "Done" : "Pending")
                 << endl;
            found = true;
        }
    }
    
    if(!found) {
        cout << "No tasks found with the search term: " << searchTerm << endl;
    }
}

void displayTask(const Task& task) {
    cout << "\n=== TASK DETAILS ===\n";
    cout << "ID: " << task.id << endl;
    cout << "Description: " << task.description << endl;
    cout << "Priority: " << priorityToString(task.priority) << endl;
    cout << "Due Date: " << task.dueDate << endl;
    cout << "Status: " << (task.isCompleted ? "Completed" : "Pending") << endl;
}

string priorityToString(TaskPriority priority) {
    switch(priority) {
        case TaskPriority::LOW: return "Low";
        case TaskPriority::MEDIUM: return "Medium";
        case TaskPriority::HIGH: return "High";
        default: return "Unknown";
    }
}

void sortTasks(vector<Task>& tasks) {
    int sortChoice;
    
    cout << "\n=== SORT TASKS ===\n";
    cout << "1. Sort by ID\n";
    cout << "2. Sort by Priority\n";
    cout << "3. Sort by Due Date\n";
    cout << "4. Sort by Status\n";
    cout << "Enter your choice: ";
    cin >> sortChoice;
    cin.ignore();
    
    switch(sortChoice) {
        case 1: // Sort by ID
            sort(tasks.begin(), tasks.end(),
                [](const Task& a, const Task& b) {
                    return a.id < b.id;
                });
            break;
            
        case 2: // Sort by Priority (High to Low)
            sort(tasks.begin(), tasks.end(),
                [](const Task& a, const Task& b) {
                    return a.priority > b.priority;
                });
            break;
            
        case 3: // Sort by Due Date
            sort(tasks.begin(), tasks.end(),
                [](const Task& a, const Task& b) {
                    return a.dueDate < b.dueDate;
                });
            break;
            
        case 4: // Sort by Status (Pending first, then Completed)
            sort(tasks.begin(), tasks.end(),
                [](const Task& a, const Task& b) {
                    if(a.isCompleted != b.isCompleted) {
                        return !a.isCompleted; // Pending tasks first
                    }
                    return a.id < b.id; // Then by ID
                });
            break;
            
        default:
            cout << "Invalid choice. Sorting by ID.\n";
            sort(tasks.begin(), tasks.end(),
                [](const Task& a, const Task& b) {
                    return a.id < b.id;
                });
    }
}

void saveTasksToFile(const vector<Task>& tasks) {
    ofstream outFile("tasks.txt");
    
    if(!outFile) {
        cout << "Error: Cannot create file!\n";
        return;
    }
    
    // Save nextTaskId first
    outFile << nextTaskId << endl;
    
    // Save each task
    for(const auto& task : tasks) {
        outFile << task.id << endl
                << task.description << endl
                << static_cast<int>(task.priority) << endl
                << task.dueDate << endl
                << task.isCompleted << endl;
    }
    
    outFile.close();
}

void loadTasksFromFile(vector<Task>& tasks) {
    ifstream inFile("tasks.txt");
    
    if(!inFile) {
        cout << "No existing tasks found. Starting fresh.\n";
        return;
    }
    
    // Clear existing tasks
    tasks.clear();
    
    // Read nextTaskId
    inFile >> nextTaskId;
    inFile.ignore(); // Skip newline
    
    // Read tasks
    Task task;
    int priorityInt;
    
    while(inFile >> task.id) {
        inFile.ignore(); // Skip newline
        getline(inFile, task.description);
        inFile >> priorityInt;
        task.priority = static_cast<TaskPriority>(priorityInt);
        inFile.ignore(); // Skip newline
        getline(inFile, task.dueDate);
        inFile >> task.isCompleted;
        inFile.ignore(); // Skip newline
        
        tasks.push_back(task);
    }
    
    inFile.close();
    cout << "Loaded " << tasks.size() << " tasks from file.\n";
}
