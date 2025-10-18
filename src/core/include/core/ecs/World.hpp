// World.hpp
#pragma once

#include "Entity.hpp"
#include "System.hpp"

#include <atomic>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <algorithm>

namespace core
{
    class World
    {
    public:
        // 创建新实体（线程安全）
        Entity& createEntity()
        {
            const Entity::ID newId = nextId_++;
            auto entity = std::make_unique<Entity>(newId);

            std::lock_guard<std::mutex> lock(entitiesMutex_);
            auto [it, inserted] = entities_.insert(std::move(entity));
            if (!inserted)
            {
                throw std::runtime_error("Failed to insert entity");
            }
            return **it; // 返回容器中存储的实体引用
        }

        // 根据ID获取实体（线程安全）
        Entity* getEntity(Entity::ID id)
        {
            std::lock_guard<std::mutex> lock(entitiesMutex_);
            auto it = std::find_if(entities_.begin(), entities_.end(),
                                   [id](const auto& e) { return e->getID() == id; });
            return (it != entities_.end()) ? it->get() : nullptr;
        }

        // 销毁指定实体（线程安全）
        void destroyEntity(Entity::ID id)
        {
            std::lock_guard<std::mutex> lock(entitiesMutex_);
            const auto it = std::ranges::find_if(entities_,
                                                 [id](const auto& e) { return e->getID() == id; });

            if (it != entities_.end())
            {
                entities_.erase(it);
            }
        }

        // 添加系统
        template <typename T, typename... Args>
        T& addSystem(Args&&... args)
        {
            static_assert(std::is_base_of_v<System, T>,
                          "T must be a subclass of System");

            auto system = std::make_unique<T>(*this, std::forward<Args>(args)...);
            auto& ref = *system;

            std::lock_guard<std::mutex> lock(systemsMutex_);
            systems_[typeid(T)] = std::move(system);
            return ref;
        }

        // 获取系统
        template <typename T>
        T& getSystem()
        {
            std::lock_guard lock(systemsMutex_);
            auto it = systems_.find(typeid(T));
            if (it == systems_.end())
            {
                throw std::runtime_error("System not registered: " + std::string(typeid(T).name()));
            }
            return *static_cast<T*>(it->second.get());
        }

        template <typename T>
        const T& getSystem() const
        {
            std::lock_guard lock(systemsMutex_);
            auto it = systems_.find(typeid(T));
            if (it == systems_.end())
            {
                throw std::runtime_error("System not registered: " + std::string(typeid(T).name()));
            }
            return *static_cast<const T*>(it->second.get());
        }

        // 更新所有系统（主线程调用）
        void update(const double deltaTime)
        {
            std::vector<std::unique_ptr<System>> systemsCopy;
            {
                std::lock_guard<std::mutex> lock(systemsMutex_);
                systemsCopy.reserve(systems_.size());
                for (auto& [_, system] : systems_)
                {
                    systemsCopy.push_back(std::move(system));
                }
                systems_.clear();
            }

            for (auto& system : systemsCopy)
            {
                system->update(deltaTime);
            }

            {
                std::lock_guard<std::mutex> lock(systemsMutex_);
                for (auto& system : systemsCopy)
                {
                    systems_[typeid(*system)] = std::move(system);
                }
            }
        }

        // 获取包含指定组件的实体集合（线程安全）
        template <typename... Components>
        std::vector<Entity*> getEntitiesWith()
        {
            std::vector<Entity*> result;
            std::lock_guard<std::mutex> lock(entitiesMutex_);
            result.reserve(entities_.size());

            for (const auto& entity : entities_)
            {
                if ((entity->template hasComponent<Components>() && ...))
                {
                    result.push_back(entity.get());
                }
            }
            return result;
        }

    private:
        // 实体管理
        std::atomic<Entity::ID> nextId_{1};
        std::unordered_set<std::unique_ptr<Entity>> entities_;
        std::mutex entitiesMutex_;

        // 系统管理
        std::unordered_map<std::type_index, std::unique_ptr<System>> systems_;
        std::mutex systemsMutex_;
    };
}
