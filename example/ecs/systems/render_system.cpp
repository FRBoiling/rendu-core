/*
* Created by boil on 2023/4/9.
*/
#include <SDL.h>
#include "render_system.h"

void RenderSystem::run(entt::registry &registry) {
    SDL_Renderer *renderer = registry.ctx().get<SDL_Renderer *>();

    SDL_RenderClear(renderer);
    // ...
    SDL_RenderPresent(renderer);
}
