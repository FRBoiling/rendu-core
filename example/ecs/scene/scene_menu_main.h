/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_SCENE_MENU_MAIN_H
#define RENDU_SCENE_MENU_MAIN_H

#include "scene.h"

class scene_menu_main final : public scene {
private:
    std::vector<entt::entity> m_entities;

public:
    void on_enter(entt::registry &registry) override;

    void on_update(entt::registry &registry) override;

    void on_exit(entt::registry &registry) override;
};


#endif //RENDU_SCENE_MENU_MAIN_H
