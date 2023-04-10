/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_INGAME_H
#define RENDU_INGAME_H

#include "scene.h"
#include "imgui_plugin.h"

class ingame final : public scene {
private:
    std::vector<entt::entity> m_entities;

public:
    void on_enter(entt::registry &registry) override;

    void on_update(entt::registry &registry) override;

    void on_exit(entt::registry &registry) override;
};


#endif //RENDU_INGAME_H
