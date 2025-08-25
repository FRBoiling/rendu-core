// Entity.hpp
#pragma once
#include "Component.hpp"
#include <typeindex>
#include <unordered_map>
#include <cassert>

namespace core
{
    class Entity
    {
    public:
        using ID = uint64_t;

        explicit Entity(ID id) : id_(id)
        {
        }

        template <typename T, typename... Args>
        T& addComponent(Args&&... args)
        {
            static_assert(std::is_base_of_v<Component, T>,
                          "T must inherit from Component");

            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            auto& ref = *component;
            components_[typeid(T)] = std::move(component);
            return ref;
        }

        template <typename T>
        T* getComponent() const
        {
            const auto it = components_.find(typeid(T));
            return it != components_.end() ? static_cast<T*>(it->second.get()) : nullptr;
        }

        template <typename T>
        bool hasComponent() const
        {
            return components_.find(typeid(T)) != components_.end();
        }

        template <typename T>
        void removeComponent()
        {
            auto it = components_.find(typeid(T));
            if (it != components_.end())
            {
                components_.erase(it);
            }
        }

        ID getID() const { return id_; }

    private:
        ID id_;
        std::unordered_map<std::type_index, std::unique_ptr<Component>> components_;
    };
}
