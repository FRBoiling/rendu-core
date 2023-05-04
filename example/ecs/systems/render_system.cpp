/*
* Created by boil on 2023/4/11.
*/

#include <SDL.h>
#include "render_system.h"

void RenderSystem::run(entt::registry &registry) {
    auto renderer = registry.ctx().get<SDL_Renderer *>();

    // 在渲染工具上设置颜色            r   g    b    a
    SDL_SetRenderDrawColor(renderer, 64, 255, 128, 255);

    // 根据配置, 重置渲染
    SDL_RenderClear(renderer);

//    auto shaders = registry.view<shader>();
//    auto cameras = registry.view<camera>();
//    auto sprites = registry.view<transform, sprite>().use<sprite>();

//    for (auto camera_entity: cameras) {
//        auto &camera = cameras.get<camera>(camera_entity);
//
//        SDL_Rect viewport = {
//                .x = int(std::floor(camera.viewport.x)),
//                .y = int(std::floor(camera.viewport.y)),
//                .w = int(std::floor(camera.viewport.w)),
//                .h = int(std::floor(camera.viewport.h))
//        };
//        SDL_RenderSetViewport(renderer, &viewport);
//
//        for (auto sprite_entity: sprites) {
//            auto &transform = sprites.get<components::transform>(sprite_entity);
//            auto &sprite = sprites.get<components::sprite>(sprite_entity);
//
//            auto screen_pos = camera.world_to_screen(transform.position);
//            auto cam_scale = camera.scale();
//
//            SDL_FRect dest = {
//                    .x = screen_pos.x,
//                    .y = screen_pos.y,
//                    .w = sprite.source.w * transform.scale.x * cam_scale.x,
//                    .h = sprite.source.h * transform.scale.y * cam_scale.y
//            };
//
//            if (shaders.contains(sprite_entity)) {
//                auto &shader = shaders.get<components::shader>(sprite_entity);
//                // 用着色器渲染texture
//            } else {
//                SDL_RenderCopyF(renderer, sprite.texture, &sprite.source, &dest);
//            }
//        }
//    }

//    SDL_RenderSetViewport(renderer, nullptr);

//    ImGui_ImplSDLRenderer_NewFrame();
//    ImGui_ImplSDL2_NewFrame();
//    ImGui::NewFrame();
//
//    // 执行"ui_command_buffer"中的命令（后面会有更多介绍）
//
//    ImGui::Render();
//    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());


    // 将渲染的内容显示在屏幕上
    SDL_RenderPresent(renderer);
}