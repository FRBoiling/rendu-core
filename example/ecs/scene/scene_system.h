/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_SCENE_SYSTEM_H
#define RENDU_SCENE_SYSTEM_H

#include "base/system.h"
#include "scene/scene.h"

class SceneSystem final : public System {
private:
    std::unique_ptr<scene> m_current_scene{nullptr};

public:
    void run(entt::registry &registry) override;

    void change_scene(entt::registry &registry, std::unique_ptr<scene> new_scene);
};

#endif //RENDU_SCENE_SYSTEM_H
