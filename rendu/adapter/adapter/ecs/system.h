/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_SYSTEM_H
#define RENDU_SYSTEM_H

#include "entity_pool.h"
#include "fwd/adapter_fwd.h"

namespace rendu {

    enum class SystemType {
        Default,
        Init,
        LateInit,
        Update,
        LateUpdate,
    };

    class BaseSystem {
    public:
        BaseSystem() : m_Type(SystemType::Default) {};
        ~BaseSystem() = default;

    public:
        SystemType GetSystemType() { return m_Type; }

        virtual void Run(Entity &entity) {
            RD_INFO("{} Run", ToString());
        };

        virtual std::string ToString() { return typeid(this).name(); }

    protected:
        SystemType m_Type;
    };

    class BaseInitSystem : public BaseSystem {
    public:
        BaseInitSystem() {
            m_Type = SystemType::Init;
        };
    public:
        std::string ToString() override { return typeid(this).name(); }
    };

    class BaseUpdateSystem : public BaseSystem {
    public:
        BaseUpdateSystem() {
            m_Type = SystemType::Update;
        };
    public:
        std::string ToString() override { return typeid(this).name(); }
    };

}

#endif //RENDU_COMPONENT_SYSTEM_H
