/*
* Created by boil on 2023/4/9.
*/

#include "scene_menu_main.h"

void scene_menu_main::on_enter(entt::registry &registry) {
    auto entity = registry.create();
// 为entity添加组件
    m_entities.push_back(entity);
}

void scene_menu_main::on_update(entt::registry &registry) {

}

void scene_menu_main::on_exit(entt::registry &registry) {

}
