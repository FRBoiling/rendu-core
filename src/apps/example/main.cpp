#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <typeindex>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <system_error>
#include <condition_variable>

//=============== 平台抽象层 ===============//
namespace Platform
{
    class Socket
    {
    public:
        virtual ~Socket() = default;
        virtual void bind(uint16_t port) = 0;
        virtual void listen() = 0;
        virtual std::unique_ptr<Socket> accept() = 0;
        virtual size_t read(void* buffer, size_t size) = 0;
        virtual size_t write(const void* buffer, size_t size) = 0;
        virtual void close() = 0;
        virtual int native_handle() const = 0;
    };

    std::unique_ptr<Socket> create_socket();
} // namespace Platform

//=============== ECS核心 ===============//
class Entity
{
    std::unordered_map<std::type_index, std::shared_ptr<void>> components;

public:
    template <typename T, typename... Args>
    T& add_component(Args&&... args)
    {
        auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
        components[typeid(T)] = ptr;
        return *ptr;
    }

    template <typename T>
    T* get_component() const
    {
        auto it = components.find(typeid(T));
        return it != components.end() ? static_cast<T*>(it->second.get()) : nullptr;
    }
};

class World;


//=============== 修改System基类 ===============//
class System
{
public:
    explicit System(World& world) : world_ref(world)
    {
    }

    virtual void update(double delta_time) = 0;
    virtual ~System() = default;

protected:
    World& world_ref; // 所有系统都能访问World
};

class World
{
public:
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<System>> systems;
    std::mutex entities_mutex;

    void add_entity(std::unique_ptr<Entity> entity)
    {
        std::lock_guard<std::mutex> lock(entities_mutex);
        entities.emplace_back(std::move(entity));
    }

    template <typename T, typename... Args>
    void add_system(Args&&... args) {
        systems.emplace_back(
            std::make_unique<T>(*this, std::forward<Args>(args)...)
        );
    }

    void update(double delta_time)
    {
        for (auto& system : systems)
        {
            system->update(delta_time);
        }
    }
};

//=============== 网络组件 ===============//
struct NetworkComponent
{
    std::unique_ptr<Platform::Socket> socket;
    std::vector<uint8_t> read_buffer;
    std::vector<uint8_t> write_buffer;
    std::atomic<bool> active{true};

    explicit NetworkComponent(std::unique_ptr<Platform::Socket> s)
        : socket(std::move(s))
    {
    }
};

//=============== 线程安全队列 ===============//
template <typename T>
class ConcurrentQueue
{
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cv;

public:
    void push(T&& item)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(item));
        }
        cv.notify_one();
    }

    bool try_pop(T& item)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (cv.wait_for(lock, std::chrono::milliseconds(10),
                        [this] { return !queue.empty(); }))
        {
            item = std::move(queue.front());
            queue.pop();
            return true;
        }
        return false;
    }
};

//=============== 网络系统 ===============//
class NetworkSystem : public System
{
    std::unique_ptr<Platform::Socket> listener;
    ConcurrentQueue<std::unique_ptr<Platform::Socket>> new_connections;
    std::atomic<bool> running{true};
    std::thread accept_thread;
    const uint16_t port;

public:
    NetworkSystem(World& world, uint16_t port)
        : System(world),
          port(port),
          listener(Platform::create_socket())
    {
        // 初始化顺序很重要

        listener->bind(port);
        listener->listen();

        accept_thread = std::thread([this]
        {
            while (running)
            {
                try
                {
                    auto client = listener->accept();
                    new_connections.push(std::move(client));
                }
                catch (...)
                {
                    if (running)
                    {
                        using namespace std::chrono_literals;
                        std::this_thread::sleep_for(10ms);
                    }
                }
            }
        });
    }

    ~NetworkSystem()
    {
        running = false;
        listener->close();
        if (accept_thread.joinable()) accept_thread.join();
    }

    void update(double) override
    {
        process_connections();
        process_io();
    }

private:
    void process_connections()
    {
        std::unique_ptr<Platform::Socket> client;
        while (new_connections.try_pop(client))
        {
            auto entity = std::make_unique<Entity>();
            entity->add_component<NetworkComponent>(std::move(client));
            world_ref.add_entity(std::move(entity));
        }
    }

    void process_io()
    {
        // 实际项目需实现非阻塞IO多路复用
        // 此处为简化实现（仅示例）
        static std::vector<Entity*> network_entities;
        network_entities.clear();

        {
            std::lock_guard<std::mutex> lock(world_ref.entities_mutex);
            for (auto& entity : world_ref.entities)
            {
                if (auto* nc = entity->get_component<NetworkComponent>())
                {
                    network_entities.push_back(entity.get());
                }
            }
        }

        for (auto entity : network_entities)
        {
            auto* nc = entity->get_component<NetworkComponent>();
            if (!nc->active) continue;

            try
            {
                uint8_t buffer[4096];
                size_t bytes = nc->socket->read(buffer, sizeof(buffer));
                if (bytes > 0)
                {
                    nc->read_buffer.insert(nc->read_buffer.end(), buffer, buffer + bytes);
                    process_packet(*nc);
                }
            }
            catch (const std::system_error& e)
            {
                nc->active = false;
            }
        }
    }

    void process_packet(NetworkComponent& nc)
    {
        // 示例：简单echo逻辑
        nc.write_buffer = nc.read_buffer;
        nc.read_buffer.clear();

        try
        {
            nc.socket->write(nc.write_buffer.data(), nc.write_buffer.size());
        }
        catch (const std::system_error& e)
        {
            nc.active = false;
        }
    }
};

//=============== 日志系统 ===============//
class Logger
{
public:
    enum Level { Debug, Info, Warning, Error };

    static void log(Level level, const std::string& message)
    {
        static std::mutex log_mutex;
        std::lock_guard<std::mutex> lock(log_mutex);

        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        std::cout << std::put_time(std::localtime(&t), "%F %T")
            << " [" << level_to_string(level) << "] "
            << message << std::endl;
    }

private:
    static const char* level_to_string(Level level)
    {
        switch (level)
        {
        case Debug: return "DEBUG";
        case Info: return "INFO";
        case Warning: return "WARN";
        case Error: return "ERROR";
        default: return "UNKNOWN";
        }
    }
};

//=============== 主程序 ===============//
int main()
{
    try
    {
        World world;
        world.add_system<NetworkSystem>(8080);

        auto last_time = std::chrono::steady_clock::now();
        while (true)
        {
            auto current_time = std::chrono::steady_clock::now();
            double delta_time = std::chrono::duration<double>(
                current_time - last_time).count();
            last_time = current_time;

            world.update(delta_time);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    catch (const std::exception& e)
    {
        Logger::log(Logger::Error, e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
