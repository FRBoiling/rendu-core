/*
* Created by boil on 2023/4/9.
*/

#include <SDL.h>
#include "event_system.h"

void EventSystem::run(entt::registry &registry) {
    auto &cf = registry.ctx().get<ControlFlow &>();

    SDL_Event evt;

    while (SDL_PollEvent(&evt) != 0) {
        switch (evt.type) {
            case SDL_QUIT:
                cf = ControlFlow::exit;
                break;

                // ...
        }
    }
}
