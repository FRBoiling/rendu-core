/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_SDL_PLUGIN_H
#define RENDU_SDL_PLUGIN_H

#include "base/plugin.h"
#include <string>
#include <SDL.h>

class sdl_plugin final : public Plugin {
public:
    sdl_plugin(std::string &win_title, uint32_t cap_fps);

public:
    void mount(game_loop &game_loop) override;

private:
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    std::string m_win_title;
    uint32_t m_cap_fps;
};


#endif //RENDU_SDL_PLUGIN_H
