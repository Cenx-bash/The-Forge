// ============================================
// ADVANCED C++ EXAMPLE: DISTRIBUTED TASK SYSTEM
// ============================================

#include <iostream>
#include <memory>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>
#include <chrono>
#include <random>
#include <type_traits>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <coroutine>
#include <concepts>

// ============================================
// 1. ADVANCED TEMPLATES & CONCEPTS
// ============================================

// Concept for tasks that can be executed
template<typename T>
concept Executable = requires(T t) {
    { t() } -> std::same_as<void>;
};

// Variadic template with fold expressions
template<typename... Args>
auto sumAll(Args... args) {
    return (args + ...); // C++17 fold expression
}

// CRTP (Curiously Recurring Template Pattern)
template<typename Derived>
class BaseSingleton {
protected:
    BaseSingleton() = default;
public:
    BaseSingleton(const BaseSingleton&) = delete;
    BaseSingleton& operator=(const BaseSingleton&) = delete;
    
    static Derived& instance() {
        static Derived instance;
        return instance;
    }
};

// ============================================
// 2. SMART POINTERS & OWNERSHIP
// ============================================

class Resource {
    std::unique_ptr<int[]> data;
    size_t size;
public:
    Resource(size_t n) : data(std::make_unique<int[]>(n)), size(n) {
        std::cout << "Resource created with size: " << n << std::endl;
    }
    
    ~Resource() {
        std::cout << "Resource destroyed" << std::endl;
    }
    
    // Move semantics
    Resource(Resource&& other) noexcept 
        : data(std::move(other.data)), size(other.size) {
        other.size = 0;
    }
    
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            size = other.size;
            other.size = 0;
        }
        return *this;
    }
    
    // No copy
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
};

// ============================================
// 3. THREAD POOL WITH MODERN C++ FEATURES
// ============================================

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop{false};
    
public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency()) {
        for(size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                while(true) {
                    std::function<void()> task;
                    {
                        std::unique_lock lock(queue_mutex);
                        condition.wait(lock, [this] {
                            return stop || !tasks.empty();
                        });
                        
                        if(stop && tasks.empty()) return;
                        
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        
        using return_type = typename std::invoke_result<F, Args...>::type;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> result = task->get_future();
        {
            std::unique_lock lock(queue_mutex);
            if(stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return result;
    }
    
    ~ThreadPool() {
        stop = true;
        condition.notify_all();
        for(std::thread &worker: workers) {
            worker.join();
        }
    }
};

// ============================================
// 4. ADVANCED TYPE-SAFE UNION (VARIANT)
// ============================================

class TaskResult {
private:
    std::variant<int, double, std::string, std::exception_ptr> data;
    
public:
    template<typename T>
    TaskResult(T&& value) : data(std::forward<T>(value)) {}
    
    void visit(auto&& visitor) {
        std::visit(visitor, data);
    }
    
    bool hasException() const {
        return std::holds_alternative<std::exception_ptr>(data);
    }
    
    void rethrowIfException() {
        if(hasException()) {
            std::rethrow_exception(std::get<std::exception_ptr>(data));
        }
    }
};

// ============================================
// 5. COROUTINES (C++20)
// ============================================

#if __has_include(<coroutine>)
struct Generator {
    struct promise_type {
        int current_value;
        
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        
        std::suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }
        
        void return_void() {}
    };
    
    std::coroutine_handle<promise_type> coro;
    
    explicit Generator(std::coroutine_handle<promise_type> h) : coro(h) {}
    ~Generator() { if(coro) coro.destroy(); }
    
    int value() const { return coro.promise().current_value; }
    
    bool next() {
        if(!coro.done()) {
            coro.resume();
            return !coro.done();
        }
        return false;
    }
};

Generator fibonacci(int n) {
    int a = 0, b = 1;
    for(int i = 0; i < n; ++i) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}
#endif

// ============================================
// 6. EXECUTION POLICY & PARALLEL ALGORITHMS
// ============================================

template<typename T>
class ParallelProcessor {
private:
    std::vector<T> data;
    ThreadPool pool;
    
public:
    ParallelProcessor(std::vector<T>&& input) 
        : data(std::move(input)), pool(4) {}
    
    template<typename Transform>
    auto parallel_transform(Transform&& transform) {
        std::vector<std::future<T>> futures;
        std::vector<T> result(data.size());
        
        for(size_t i = 0; i < data.size(); ++i) {
            futures.push_back(pool.enqueue([&, i] {
                return transform(data[i]);
            }));
        }
        
        for(size_t i = 0; i < futures.size(); ++i) {
            result[i] = futures[i].get();
        }
        
        return result;
    }
    
    T parallel_reduce(auto&& binary_op) {
        if(data.empty()) return T{};
        
        size_t chunk_size = std::max(size_t(1), data.size() / pool.enqueue([]{}).wait_for(std::chrono::seconds(0)) ? 1 : 4);
        std::vector<std::future<T>> futures;
        
        for(size_t i = 0; i < data.size(); i += chunk_size) {
            size_t end = std::min(i + chunk_size, data.size());
            futures.push_back(pool.enqueue([=] {
                T result = data[i];
                for(size_t j = i + 1; j < end; ++j) {
                    result = binary_op(result, data[j]);
                }
                return result;
            }));
        }
        
        T final_result = futures[0].get();
        for(size_t i = 1; i < futures.size(); ++i) {
            final_result = binary_op(final_result, futures[i].get());
        }
        
        return final_result;
    }
};

// ============================================
// 7. TYPE TRAITS & SFINAE
// ============================================

template<typename T>
struct is_container : std::false_type {};

template<typename T>
struct is_container<std::vector<T>> : std::true_type {};

template<typename T>
struct is_container<std::list<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_container_v = is_container<T>::value;

template<typename T>
auto process(const T& value) {
    if constexpr (std::is_arithmetic_v<T>) {
        return value * 2;
    } else if constexpr (is_container_v<T>) {
        T result = value;
        for(auto& elem : result) {
            elem = process(elem);
        }
        return result;
    } else {
        return value;
    }
}

// ============================================
// 8. OBSERVER PATTERN WITH MODERN C++
// ============================================

template<typename... Args>
class Observable {
    using Observer = std::function<void(Args...)>;
    std::vector<Observer> observers;
    std::mutex observers_mutex;
    
public:
    void subscribe(Observer observer) {
        std::lock_guard lock(observers_mutex);
        observers.push_back(std::move(observer));
    }
    
    void notify(Args... args) {
        std::vector<Observer> observers_copy;
        {
            std::lock_guard lock(observers_mutex);
            observers_copy = observers;
        }
        
        for(auto& observer : observers_copy) {
            try {
                observer(args...);
            } catch(...) {
                // Log error, don't propagate
            }
        }
    }
};

// ============================================
// MAIN DEMONSTRATION
// ============================================

int main() {
    std::cout << "=== ADVANCED C++ DEMONSTRATION ===\n\n";
    
    // 1. Thread Pool usage
    std::cout << "1. Thread Pool Demonstration:\n";
    {
        ThreadPool pool(4);
        std::vector<std::future<int>> results;
        
        for(int i = 0; i < 8; ++i) {
            results.emplace_back(pool.enqueue([i] {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                return i * i;
            }));
        }
        
        for(auto& result : results) {
            std::cout << "Result: " << result.get() << std::endl;
        }
    }
    
    std::cout << "\n2. Advanced Templates:\n";
    {
        // Fold expression
        std::cout << "Sum: " << sumAll(1, 2.5, 3, 4.2) << std::endl;
        
        // Type traits
        std::vector<int> vec{1, 2, 3, 4};
        auto processed = process(vec);
        std::cout << "Processed vector: ";
        for(auto& v : processed) std::cout << v << " ";
        std::cout << std::endl;
    }
    
    std::cout << "\n3. Resource Management (RAII):\n";
    {
        Resource r1(100);
        Resource r2 = std::move(r1); // Move constructor
        // r1 is now empty
    }
    
    std::cout << "\n4. Variant Type-Safe Union:\n";
    {
        TaskResult result1(42);
        TaskResult result2(3.14);
        TaskResult result3(std::string("Hello"));
        
        auto printer = [](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                std::cout << "Integer: " << arg << std::endl;
            } else if constexpr (std::is_same_v<T, double>) {
                std::cout << "Double: " << arg << std::endl;
            } else if constexpr (std::is_same_v<T, std::string>) {
                std::cout << "String: " << arg << std::endl;
            }
        };
        
        result1.visit(printer);
        result2.visit(printer);
        result3.visit(printer);
    }
    
#if __has_include(<coroutine>)
    std::cout << "\n5. C++20 Coroutines:\n";
    {
        auto fib = fibonacci(10);
        std::cout << "Fibonacci: ";
        while(fib.next()) {
            std::cout << fib.value() << " ";
        }
        std::cout << std::endl;
    }
#endif
    
    std::cout << "\n6. Parallel Processing:\n";
    {
        std::vector<int> numbers(100);
        std::iota(numbers.begin(), numbers.end(), 1);
        
        ParallelProcessor processor(std::move(numbers));
        
        // Parallel transform
        auto squared = processor.parallel_transform([](int x) {
            return x * x;
        });
        
        // Parallel reduce
        auto sum = processor.parallel_reduce(std::plus<int>{});
        
        std::cout << "Sum of squares: " << sum << std::endl;
    }
    
    std::cout << "\n7. Observer Pattern:\n";
    {
        Observable<int, std::string> observable;
        
        observable.subscribe([](int id, const std::string& message) {
            std::cout << "Observer 1: [" << id << "] " << message << std::endl;
        });
        
        observable.subscribe([](int id, const std::string& message) {
            std::cout << "Observer 2: [" << id << "] " << message << std::endl;
        });
        
        observable.notify(1, "Task started");
        observable.notify(2, "Task completed");
    }
    
    std::cout << "\n=== DEMONSTRATION COMPLETE ===\n";
    
    return 0;
}
