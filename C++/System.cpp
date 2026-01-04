// ==============================
//  C++ System Framework
// ==============================
// Author: Zen
// Version: 1.2
// ==============================

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <future>
#include <type_traits>
#include <optional>
#include <variant>
#include <any>
#include <algorithm>
#include <ranges>
#include <concepts>
#include <coroutine>
#include <source_location>
#include <format>

// Forward declarations
namespace SystemFramework {
    template<typename T>
    concept Serializable = requires(T t, std::ostream& os) {
        { t.serialize(os) } -> std::same_as<void>;
    };

    class IComponent;
    class SystemManager;
    class EventDispatcher;
    class ThreadPool;
}

// ==============================
// Utility Macros and Types
// ==============================
#define NO_COPY(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete

#define NO_MOVE(ClassName) \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete

#define SINGLETON(ClassName) \
    NO_COPY(ClassName); \
    NO_MOVE(ClassName); \
    static ClassName& instance() { \
        static ClassName instance; \
        return instance; \
    }

namespace SystemFramework::Types {
    using UUID = std::string;
    
    template<typename T>
    using Ref = std::shared_ptr<T>;
    
    template<typename T>
    using WeakRef = std::weak_ptr<T>;
    
    template<typename T>
    using UniqueRef = std::unique_ptr<T>;
    
    template<typename T>
    using Optional = std::optional<T>;
    
    template<typename... Ts>
    using Variant = std::variant<Ts...>;
    
    using Any = std::any;
    
    UUID generateUUID() {
        static std::atomic<uint64_t> counter{0};
        return std::format("UUID-{}-{}", 
            std::chrono::steady_clock::now().time_since_epoch().count(),
            counter.fetch_add(1, std::memory_order_relaxed));
    }
}

// ==============================
// Advanced Memory Management
// ==============================
namespace SystemFramework::Memory {
    template<typename T, size_t PoolSize = 1024>
    class ObjectPool {
    private:
        struct Block {
            alignas(T) std::byte storage[sizeof(T)];
            bool allocated{false};
        };
        
        std::array<Block, PoolSize> pool;
        std::stack<size_t> freeList;
        std::mutex mutex;
        
    public:
        ObjectPool() {
            for (size_t i = 0; i < PoolSize; ++i) {
                freeList.push(i);
            }
        }
        
        template<typename... Args>
        Ref<T> acquire(Args&&... args) {
            std::lock_guard lock(mutex);
            
            if (freeList.empty()) {
                return std::make_shared<T>(std::forward<Args>(args)...);
            }
            
            size_t index = freeList.top();
            freeList.pop();
            
            Block& block = pool[index];
            new(&block.storage) T(std::forward<Args>(args)...);
            block.allocated = true;
            
            return Ref<T>(reinterpret_cast<T*>(&block.storage),
                [this, index](T* ptr) {
                    std::lock_guard lock2(mutex);
                    ptr->~T();
                    pool[index].allocated = false;
                    freeList.push(index);
                });
        }
    };
    
    template<typename T>
    class Allocator {
    public:
        using value_type = T;
        
        Allocator() = default;
        
        template<typename U>
        Allocator(const Allocator<U>&) noexcept {}
        
        T* allocate(size_t n) {
            if (n > std::numeric_limits<size_t>::max() / sizeof(T)) {
                throw std::bad_alloc();
            }
            
            if (auto p = static_cast<T*>(std::malloc(n * sizeof(T)))) {
                return p;
            }
            
            throw std::bad_alloc();
        }
        
        void deallocate(T* p, size_t) noexcept {
            std::free(p);
        }
    };
}

// ==============================
// Event System with Type Safety
// ==============================
namespace SystemFramework::Events {
    class IEvent {
    public:
        virtual ~IEvent() = default;
        virtual UUID getType() const = 0;
        virtual std::string toString() const = 0;
    };
    
    template<typename EventType>
    class Event : public IEvent {
    private:
        static inline const UUID typeID = Types::generateUUID();
        
    public:
        UUID getType() const override {
            return typeID;
        }
        
        std::string toString() const override {
            return std::format("Event[{}]", typeid(EventType).name());
        }
        
        virtual void dispatch() = 0;
    };
    
    class EventDispatcher {
    private:
        using EventHandler = std::function<void(const Ref<IEvent>&)>;
        std::unordered_map<UUID, std::vector<EventHandler>> listeners;
        std::mutex mutex;
        ThreadPool& threadPool;
        
    public:
        EventDispatcher(ThreadPool& pool) : threadPool(pool) {}
        
        template<typename EventType>
        void subscribe(std::function<void(const Ref<EventType>&)> handler) {
            std::lock_guard lock(mutex);
            listeners[EventType::typeID].emplace_back(
                [handler = std::move(handler)](const Ref<IEvent>& event) {
                    handler(std::static_pointer_cast<EventType>(event));
                }
            );
        }
        
        template<typename EventType, typename... Args>
        void emit(Args&&... args) {
            auto event = std::make_shared<EventType>(std::forward<Args>(args)...);
            
            std::lock_guard lock(mutex);
            if (auto it = listeners.find(EventType::typeID); it != listeners.end()) {
                for (auto& handler : it->second) {
                    threadPool.enqueue([handler, event] {
                        handler(event);
                    });
                }
            }
        }
        
        template<typename EventType, typename... Args>
        void emitSync(Args&&... args) {
            auto event = std::make_shared<EventType>(std::forward<Args>(args)...);
            
            std::lock_guard lock(mutex);
            if (auto it = listeners.find(EventType::typeID); it != listeners.end()) {
                for (auto& handler : it->second) {
                    handler(event);
                }
            }
        }
    };
}

// ==============================
// Component-Based Architecture
// ==============================
namespace SystemFramework::ECS {
    class IComponent {
    public:
        virtual ~IComponent() = default;
        virtual UUID getType() const = 0;
        virtual Ref<IComponent> clone() const = 0;
        virtual void update(double deltaTime) = 0;
        virtual void serialize(std::ostream& os) const = 0;
        virtual void deserialize(std::istream& is) = 0;
    };
    
    template<typename T>
    class Component : public IComponent {
    private:
        static inline const UUID typeID = Types::generateUUID();
        
    public:
        UUID getType() const override {
            return typeID;
        }
        
        Ref<IComponent> clone() const override {
            return std::make_shared<T>(static_cast<const T&>(*this));
        }
        
        void update(double) override {}
        void serialize(std::ostream&) const override {}
        void deserialize(std::istream&) override {}
    };
    
    class Entity {
    private:
        UUID id;
        std::unordered_map<UUID, Ref<IComponent>> components;
        std::string tag;
        
    public:
        explicit Entity(std::string entityTag = "") 
            : id(Types::generateUUID()), tag(std::move(entityTag)) {}
        
        UUID getId() const { return id; }
        const std::string& getTag() const { return tag; }
        
        template<typename T, typename... Args>
        Ref<T> addComponent(Args&&... args) {
            static_assert(std::is_base_of_v<IComponent, T>, 
                "T must inherit from IComponent");
            
            auto component = std::make_shared<T>(std::forward<Args>(args)...);
            components[component->getType()] = component;
            return component;
        }
        
        template<typename T>
        Optional<Ref<T>> getComponent() const {
            if (auto it = components.find(T::typeID); it != components.end()) {
                return std::static_pointer_cast<T>(it->second);
            }
            return std::nullopt;
        }
        
        template<typename T>
        bool hasComponent() const {
            return components.find(T::typeID) != components.end();
        }
        
        template<typename T>
        void removeComponent() {
            components.erase(T::typeID);
        }
        
        void update(double deltaTime) {
            for (auto& [_, component] : components) {
                component->update(deltaTime);
            }
        }
    };
    
    class System {
    public:
        virtual ~System() = default;
        virtual void initialize() = 0;
        virtual void update(double deltaTime) = 0;
        virtual void shutdown() = 0;
    };
    
    class SystemManager {
    private:
        std::vector<UniqueRef<System>> systems;
        std::unordered_map<UUID, Ref<Entity>> entities;
        Events::EventDispatcher& eventDispatcher;
        
    public:
        explicit SystemManager(Events::EventDispatcher& dispatcher) 
            : eventDispatcher(dispatcher) {}
        
        template<typename T, typename... Args>
        T& registerSystem(Args&&... args) {
            static_assert(std::is_base_of_v<System, T>, 
                "T must inherit from System");
            
            auto system = std::make_unique<T>(std::forward<Args>(args)...);
            T& ref = *system;
            systems.push_back(std::move(system));
            return ref;
        }
        
        Ref<Entity> createEntity(const std::string& tag = "") {
            auto entity = std::make_shared<Entity>(tag);
            entities[entity->getId()] = entity;
            return entity;
        }
        
        void update(double deltaTime) {
            for (auto& system : systems) {
                system->update(deltaTime);
            }
            
            for (auto& [_, entity] : entities) {
                entity->update(deltaTime);
            }
        }
    };
}

// ==============================
// Concurrent Task System
// ==============================
namespace SystemFramework::Concurrency {
    class ThreadPool {
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<bool> stop{false};
        
    public:
        explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency()) {
            workers.reserve(numThreads);
            for (size_t i = 0; i < numThreads; ++i) {
                workers.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;
                        
                        {
                            std::unique_lock lock(queueMutex);
                            condition.wait(lock, [this] {
                                return stop || !tasks.empty();
                            });
                            
                            if (stop && tasks.empty()) {
                                return;
                            }
                            
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        
                        task();
                    }
                });
            }
        }
        
        ~ThreadPool() {
            stop = true;
            condition.notify_all();
            for (auto& worker : workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }
        
        template<typename F, typename... Args>
        auto enqueue(F&& f, Args&&... args) 
            -> std::future<std::invoke_result_t<F, Args...>> {
            
            using ReturnType = std::invoke_result_t<F, Args...>;
            
            auto task = std::make_shared<std::packaged_task<ReturnType()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );
            
            std::future<ReturnType> result = task->get_future();
            
            {
                std::lock_guard lock(queueMutex);
                if (stop) {
                    throw std::runtime_error("ThreadPool is stopped");
                }
                tasks.emplace([task] { (*task)(); });
            }
            
            condition.notify_one();
            return result;
        }
    };
    
    template<typename T>
    class ConcurrentQueue {
    private:
        mutable std::mutex mutex;
        std::queue<T> queue;
        std::condition_variable condition;
        
    public:
        void push(T value) {
            std::lock_guard lock(mutex);
            queue.push(std::move(value));
            condition.notify_one();
        }
        
        Optional<T> tryPop() {
            std::lock_guard lock(mutex);
            if (queue.empty()) {
                return std::nullopt;
            }
            T value = std::move(queue.front());
            queue.pop();
            return value;
        }
        
        T waitAndPop() {
            std::unique_lock lock(mutex);
            condition.wait(lock, [this] { return !queue.empty(); });
            T value = std::move(queue.front());
            queue.pop();
            return value;
        }
        
        bool empty() const {
            std::lock_guard lock(mutex);
            return queue.empty();
        }
    };
}

// ==============================
// Async Coroutine Support (C++20)
// ==============================
namespace SystemFramework::Async {#include <iostream>

 using namespace std:

    int main(){
     cout << "Hello World";
 return 0;
}
    template<typename T = void>
    class Task {
    public:
        struct promise_type {
            T value;
            std::exception_ptr exception;
            std::coroutine_handle<> continuation;
            
            Task get_return_object() {
                return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }
            
            std::suspend_always initial_suspend() { return {}; }
            
            auto final_suspend() noexcept {
                struct awaiter {
                    bool await_ready() noexcept { return false; }
                    void await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                        if (h.promise().continuation) {
                            h.promise().continuation.resume();
                        }
                    }
                    void await_resume() noexcept {}
                };
                return awaiter{};
            }
            
            void unhandled_exception() {
                exception = std::current_exception();
            }
            
            template<typename U>
            void return_value(U&& u) {
                value = std::forward<U>(u);
            }
            
            void return_void() {}
        };
        
        explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
        
        ~Task() {
            if (handle) handle.destroy();
        }
        
        Task(Task&& other) noexcept : handle(other.handle) {
            other.handle = nullptr;
        }
        
        bool await_ready() const { return false; }
        
        void await_suspend(std::coroutine_handle<> continuation) {
            handle.promise().continuation = continuation;
            handle.resume();
        }
        
        T await_resume() {
            if (handle.promise().exception) {
                std::rethrow_exception(handle.promise().exception);
            }
            if constexpr (!std::is_void_v<T>) {
                return std::move(handle.promise().value);
            }
        }
        
    private:
        std::coroutine_handle<promise_type> handle;
    };
    
    template<typename T>
    class AsyncValue {
    private:
        std::mutex mutex;
        std::condition_variable condition;
        Optional<T> value;
        std::exception_ptr exception;
        bool ready{false};
        
    public:
        void setValue(T val) {
            std::lock_guard lock(mutex);
            value = std::move(val);
            ready = true;
            condition.notify_all();
        }
        
        void setException(std::exception_ptr exc) {
            std::lock_guard lock(mutex);
            exception = exc;
            ready = true;
            condition.notify_all();
        }
        
        T get() {
            std::unique_lock lock(mutex);
            condition.wait(lock, [this] { return ready; });
            
            if (exception) {
                std::rethrow_exception(exception);
            }
            
            return std::move(*value);
        }
    };
}

// ==============================
// Advanced Logging System
// ==============================
namespace SystemFramework::Logging {
    enum class LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
    
    class Logger {
    private:
        std::string name;
        LogLevel level{LogLevel::INFO};
        mutable std::mutex mutex;
        
        static constexpr const char* levelToString(LogLevel lvl) {
            switch (lvl) {
                case LogLevel::TRACE: return "TRACE";
                case LogLevel::DEBUG: return "DEBUG";
                case LogLevel::INFO:  return "INFO";
                case LogLevel::WARN:  return "WARN";
                case LogLevel::ERROR: return "ERROR";
                case LogLevel::FATAL: return "FATAL";
                default: return "UNKNOWN";
            }
        }
        
    public:
        explicit Logger(std::string loggerName) : name(std::move(loggerName)) {}
        
        void setLevel(LogLevel newLevel) {
            level = newLevel;
        }
        
        template<typename... Args>
        void log(LogLevel lvl, std::source_location loc, 
                 std::format_string<Args...> fmt, Args&&... args) const {
            if (lvl < level) return;
            
            auto now = std::chrono::system_clock::now();
            auto msg = std::format(fmt, std::forward<Args>(args)...);
            
            std::lock_guard lock(mutex);
            std::cout << std::format("[{}] [{}] [{}:{}] {}: {}\n",
                std::chrono::floor<std::chrono::milliseconds>(now),
                levelToString(lvl),
                loc.file_name(), loc.line(),
                name, msg);
        }
        
        template<typename... Args>
        void trace(std::format_string<Args...> fmt, Args&&... args) const {
            log(LogLevel::TRACE, std::source_location::current(), 
                fmt, std::forward<Args>(args)...);
        }
        
        template<typename... Args>
        void info(std::format_string<Args...> fmt, Args&&... args) const {
            log(LogLevel::INFO, std::source_location::current(), 
                fmt, std::forward<Args>(args)...);
        }
        
        template<typename... Args>
        void error(std::format_string<Args...> fmt, Args&&... args) const {
            log(LogLevel::ERROR, std::source_location::current(), 
                fmt, std::forward<Args>(args)...);
        }
    };
    
    class LogManager {
    private:
        std::unordered_map<std::string, Ref<Logger>> loggers;
        std::mutex mutex;
        
        SINGLETON(LogManager);
        
    public:
        Ref<Logger> getLogger(const std::string& name) {
            std::lock_guard lock(mutex);
            if (auto it = loggers.find(name); it != loggers.end()) {
                return it->second;
            }
            
            auto logger = std::make_shared<Logger>(name);
            loggers[name] = logger;
            return logger;
        }
    };
}

// ==============================
// Configuration Management
// ==============================
namespace SystemFramework::Config {
    class ConfigValue {
    private:
        Variant<int, double, bool, std::string, std::vector<ConfigValue>> value;
        
    public:
        template<typename T>
        ConfigValue(T val) : value(std::move(val)) {}
        
        template<typename T>
        Optional<T> get() const {
            if (const T* ptr = std::get_if<T>(&value)) {
                return *ptr;
            }
            return std::nullopt;
        }
        
        template<typename T>
        T getOr(T defaultValue) const {
            return get<T>().value_or(defaultValue);
        }
    };
    
    class Configuration {
    private:
        std::unordered_map<std::string, ConfigValue> settings;
        std::mutex mutex;
        
    public:
        template<typename T>
        void set(const std::string& key, T value) {
            std::lock_guard lock(mutex);
            settings[key] = ConfigValue(std::move(value));
        }
        
        template<typename T>
        Optional<T> get(const std::string& key) const {
            std::lock_guard lock(mutex);
            if (auto it = settings.find(key); it != settings.end()) {
                return it->second.get<T>();
            }
            return std::nullopt;
        }
        
        template<typename T>
        T getOr(const std::string& key, T defaultValue) const {
            return get<T>(key).value_or(defaultValue);
        }
    };
}

// ==============================
// Example Usage Components
// ==============================
namespace SystemFramework::Examples {
    // Example Component
    class TransformComponent : public ECS::Component<TransformComponent> {
    public:
        float x{0}, y{0}, z{0};
        float rotation{0};
        float scale{1.0f};
        
        void update(double deltaTime) override {
            // Update logic here
            rotation += 90.0f * deltaTime;
        }
        
        void serialize(std::ostream& os) const override {
            os << x << " " << y << " " << z << " " << rotation << " " << scale;
        }
        
        void deserialize(std::istream& is) override {
            is >> x >> y >> z >> rotation >> scale;
        }
    };
    
    class PhysicsSystem : public ECS::System {
    private:
        Logging::Logger logger;
        
    public:
        PhysicsSystem() : logger("PhysicsSystem") {}
        
        void initialize() override {
            logger.info("Physics system initialized");
        }
        
        void update(double deltaTime) override {
            // Physics update logic
            logger.trace("Physics update: {}s", deltaTime);
        }
        
        void shutdown() override {
            logger.info("Physics system shutdown");
        }
    };
    
    // Example Event
    class CollisionEvent : public Events::Event<CollisionEvent> {
    public:
        UUID entityA;
        UUID entityB;
        float impactForce;
        
        CollisionEvent(UUID a, UUID b, float force) 
            : entityA(std::move(a)), entityB(std::move(b)), impactForce(force) {}
        
        void dispatch() override {
            // Event-specific dispatch logic
        }
        
        std::string toString() const override {
            return std::format("CollisionEvent: {} collided with {} (force: {})", 
                entityA, entityB, impactForce);
        }
    };
}

// ==============================
// Main Application Framework
// ==============================
class AdvancedSystemApplication {
private:
    SystemFramework::Concurrency::ThreadPool threadPool;
    SystemFramework::Events::EventDispatcher eventDispatcher;
    SystemFramework::ECS::SystemManager systemManager;
    SystemFramework::Logging::Ref<SystemFramework::Logging::Logger> logger;
    SystemFramework::Config::Configuration config;
    
    std::atomic<bool> running{false};
    std::chrono::steady_clock::time_point lastUpdate;
    
public:
    AdvancedSystemApplication() 
        : eventDispatcher(threadPool),
          systemManager(eventDispatcher),
          logger(SystemFramework::Logging::LogManager::instance().getLogger("Application")) {
        
        initialize();
    }
    
    void initialize() {
        logger->info("Initializing Advanced System Application");
        
        // Load configuration
        config.set("maxFPS", 60);
        config.set("windowTitle", "Advanced C++ System");
        
        // Register systems
        auto& physicsSystem = systemManager.registerSystem<SystemFramework::Examples::PhysicsSystem>();
        
        // Subscribe to events
        eventDispatcher.subscribe<SystemFramework::Examples::CollisionEvent>(
            [this](const auto& event) {
                logger->info("Collision detected: {}", event->toString());
            }
        );
        
        // Create example entities
        auto entity = systemManager.createEntity("Player");
        entity->addComponent<SystemFramework::Examples::TransformComponent>();
        
        lastUpdate = std::chrono::steady_clock::now();
    }
    
    void run() {
        running = true;
        logger->info("Starting main loop");
        
        const double targetFrameTime = 1.0 / config.getOr("maxFPS", 60);
        
        while (running) {
            auto currentTime = std::chrono::steady_clock::now();
            double deltaTime = std::chrono::duration<double>(currentTime - lastUpdate).count();
            
            // Fixed timestep
            if (deltaTime >= targetFrameTime) {
                update(deltaTime);
                lastUpdate = currentTime;
            }
            
            // Yield to prevent CPU spinning
            std::this_thread::yield();
        }
        
        shutdown();
    }
    
    void update(double deltaTime) {
        // Update all systems
        systemManager.update(deltaTime);
        
        // Emit example event
        static int frameCount = 0;
        if (frameCount++ % 100 == 0) {
            eventDispatcher.emit<SystemFramework::Examples::CollisionEvent>(
                "Entity1", "Entity2", 100.0f
            );
        }
    }
    
    void shutdown() {
        logger->info("Shutting down application");
        running = false;
    }
};

// ==============================
// Entry Point
// ==============================
int main() {
    try {
        std::cout << "=========================================\n";
        std::cout << "C++ System Framework\n";
        std::cout << "=========================================\n\n";
        
        AdvancedSystemApplication app;
        app.run();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
