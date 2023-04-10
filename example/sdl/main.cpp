/*
* Created by boil on 2023/2/4.
*/

#include "SDL2/SDL.h"

int main(int argc, char* argv[])
{
    SDL_Window* window;
    SDL_Renderer* renderer;

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
    window = SDL_CreateWindow("SDL_RenderClear",
            // 窗口位置，x居中            y居中
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            // 窗口大小, w    h
                              512, 512,
            // 窗口标志，指明 opengl (SDL_WINDOW_OPENGL) vulkan(SDL_WINDOW_VULKAN) 窗口，一般默认0.
                              0);

    // 为窗口生成渲染工具
    renderer = SDL_CreateRenderer(window, -1, 0);

    // 在渲染工具上设置颜色            r   g    b    a
    SDL_SetRenderDrawColor(renderer, 64, 255, 128, 255);

    // 根据配置, 重置渲染
    SDL_RenderClear(renderer);

    // 将渲染的内容显示在屏幕上
    SDL_RenderPresent(renderer);

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }
//    // 延时
//    SDL_Delay(5000);

    // 释放 SDL 资源
    SDL_Quit();
    return 0;
}


