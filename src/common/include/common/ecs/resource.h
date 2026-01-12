//
// Created by boil on 2026/1/16.
// ECS 资源管理系统
//

#ifndef RENDU_ECS_RESOURCE_H
#define RENDU_ECS_RESOURCE_H

#include "common/define.h"
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <functional>
#include <stdexcept>
#include <mutex>

BEGIN_NAMESPACE_ECS

template <typename T>
using ResourceLoader = std::function<std::shared_ptr<T>()>;

class RC_COMMON_API ResourceCache {
public:
    ResourceCache() = default;
    ~ResourceCache() = default;

    template <typename T>
    void emplace(const std::string& id, std::shared_ptr<T> resource) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cache[typeid(T)][id] = std::static_pointer_cast<void>(resource);
    }

    template <typename T>
    std::shared_ptr<T> get(const std::string& id) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_cache.find(typeid(T));
        if (it == m_cache.end()) return nullptr;
        auto cit = it->second.find(id);
        if (cit == it->second.end()) return nullptr;
        return std::static_pointer_cast<T>(cit->second);
    }

    template <typename T>
    bool contains(const std::string& id) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_cache.find(typeid(T));
        if (it == m_cache.end()) return false;
        return it->second.find(id) != it->second.end();
    }

private:
    mutable std::mutex m_mutex;
    std::unordered_map<std::type_index, std::unordered_map<std::string, std::shared_ptr<void>>> m_cache;
};

class RC_COMMON_API Locator {
public:
    template <typename T>
    static void set(std::shared_ptr<T> resource) {
        std::lock_guard<std::mutex> lock(instance().m_mutex);
        instance().m_resources[typeid(T)] = std::static_pointer_cast<void>(resource);
    }

    template <typename T>
    static T& get() {
        std::lock_guard<std::mutex> lock(instance().m_mutex);
        auto it = instance().m_resources.find(typeid(T));
        if (it == instance().m_resources.end())
            throw std::runtime_error(std::string("Resource not found: ") + typeid(T).name());
        return *std::static_pointer_cast<T>(it->second);
    }

    template <typename T>
    static bool contains() {
        std::lock_guard<std::mutex> lock(instance().m_mutex);
        return instance().m_resources.find(typeid(T)) != instance().m_resources.end();
    }

private:
    Locator() = default;
    ~Locator() = default;
    Locator(const Locator&) = delete;
    Locator& operator=(const Locator&) = delete;

    static Locator& instance() {
        static Locator instance;
        return instance;
    }

    std::mutex m_mutex;
    std::unordered_map<std::type_index, std::shared_ptr<void>> m_resources;
};

END_NAMESPACE_ECS

#endif
