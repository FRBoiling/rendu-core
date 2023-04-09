/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_IMGUI_PLUGIN_H
#define RENDU_IMGUI_PLUGIN_H

#include "base/plugin.h"
class imgui_plugin final : public Plugin {
public:
    void mount(game_loop &game_loop) override;

public:
    class ui_command_request {

    };
};


#endif //RENDU_IMGUI_PLUGIN_H
