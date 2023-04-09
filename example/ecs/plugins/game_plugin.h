/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_GAME_PLUGIN_H
#define RENDU_GAME_PLUGIN_H

#include "base/plugin.h"
#include "base/game_loop.h"
#include "scene/scene_system.h"

class game_plugin final :public Plugin {
private:
    std::shared_ptr<SceneSystem> m_scene_manager{
            std::make_shared<SceneSystem>()
    };

public:
    void mount(game_loop &game_loop) override;
};


#endif //RENDU_GAME_PLUGIN_H
