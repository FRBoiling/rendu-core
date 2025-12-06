#include "common/ecs/world.h"
#include "common/ecs/application.h"
#include <stdexcept>
#include <memory>

#include "common/logging/log.h"

BEGIN_NAMESPACE_COMMON
    using namespace Ecs;

    World::World(Application* application)
        : application_(application),
          thread_pool_(std::make_unique<Threading::ThreadPoolAdapter>(Threading::WorkStealingThreadPoolType::Basic,
                                                                      std::thread::hardware_concurrency())),
          system_manager_(std::make_unique<SystemManager>(*thread_pool_))
    {
    }

    World::~World()
    {
        shutdown_systems();
        cleanup_systems();
    }

    void World::set_application(Application* application)
    {
        application_ = application;
    }

    Entity World::create_entity()
    {
        return registry_.create();
    }

    void World::destroy_entity(Entity entity)
    {
        registry_.destroy(entity);
    }

    bool World::is_valid(Entity entity) const
    {
        return registry_.valid(entity);
    }

END_NAMESPACE_COMMON
