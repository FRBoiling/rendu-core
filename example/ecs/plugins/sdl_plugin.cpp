/*
* Created by boil on 2023/4/9.
*/

#include "sdl_plugin.h"
#include "base/game_loop.h"
#include "event_system.h"
#include "render_system.h"

sdl_plugin::sdl_plugin(std::string &win_title, uint32_t cap_fps)
        : m_win_title(win_title)
        , m_cap_fps(cap_fps)
        , m_renderer(nullptr)
        , m_window(nullptr)
        {}

void sdl_plugin::mount(game_loop &game_loop) {
    game_loop
            .add_setup_callback([&](entt::registry &registry) {
                // 初始化SDL环境，
                // SDL_INIT_EVERYTHING 全部环境
                // SDL_INIT_TIMER   时间
                // SDL_INIT_AUDIO   音频
                // SDL_INIT_VIDEO   视频
                // SDL_INIT_JOYSTICK    手柄
                // SDL_INIT_HAPTIC      触摸传感器
                // SDL_INIT_GAMECONTROLLER 游戏控制器
                // SDL_INIT_EVENTS  事件触发
                // SDL_INIT_SENSOR  其他传感器
                // SDL_INIT_NOPARACHUTE  其他
                if (SDL_Init(SDL_INIT_VIDEO) < 0)
                    return 1;

                // 使用SDL初始化一个窗口
                // ps: 现在linux操作系统大多数还是使用的x11架构，是一个server(窗口管理器:如 Xfce,gnome,kde 等)多个client(窗口)的结构。
                // 现在技术圈也在尝试架构更加合理的 wayland 架构。
                // 窗口名字
                m_window = SDL_CreateWindow("SDL_RenderClear",
                        // 窗口位置，x居中            y居中
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        // 窗口大小, w    h
                                          512, 512,
                        // 窗口标志，指明 opengl (SDL_WINDOW_OPENGL) vulkan(SDL_WINDOW_VULKAN) 窗口，一般默认0.
                                          0);

                // 为窗口生成渲染工具
                m_renderer = SDL_CreateRenderer(m_window, -1, 0);

                registry.ctx().emplace<SDL_Renderer * >(m_renderer);
                registry.ctx().emplace<SDL_Window * >(m_window);

                return 0;
            })
            .add_teardown_callback([&](entt::registry &registry) {
                SDL_Renderer *renderer = registry.ctx().get<SDL_Renderer *>();
                SDL_Window *win = registry.ctx().get<SDL_Window *>();
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(win);
                // 释放 SDL 资源
                SDL_Quit();
            })
            .add_system(std::make_shared<EventSystem>())
            .add_system_last(std::make_shared<RenderSystem>())
            ;
}