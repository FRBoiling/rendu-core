/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_SDL_PLUGIN_H
#define RENDU_SDL_PLUGIN_H

#include "base/plugin.h"
#include <string>

class sdl_plugin final : public Plugin {
public:
    sdl_plugin(std::string& win_title, uint32_t cap_fps);

public:
    void mount(game_loop &game_loop) override;
};


#endif //RENDU_SDL_PLUGIN_H
