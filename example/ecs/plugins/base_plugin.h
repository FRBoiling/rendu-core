/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_BASE_PLUGIN_H
#define RENDU_BASE_PLUGIN_H

#include "base/plugin.h"
#include <string>
#include "sdl_plugin.h"
#include "imgui_plugin.h"
#include "input/manager.h"

class BasePlugin final : public Plugin {
public:
    struct config {
        std::string win_title;
        uint32_t cap_fps;
    };

private:
    sdl_plugin m_sdl;
    imgui_plugin m_imgui;
    std::shared_ptr<input::manager> m_input_manager;
//    std::shared_ptr<messaging::bus> m_message_bus;
//
//    std::shared_ptr<sys::debug_system> m_debug_system;

public:
    explicit BasePlugin(config &config)
            : m_sdl(config.win_title, config.cap_fps)
            , m_input_manager(std::make_shared<input::manager>())
//            , m_message_bus(std::make_shared<messaging::bus>())
//            , m_debug_system(std::make_shared<debug_system>())
            {}

    void mount(game_loop &game_loop) override;
};

#endif //RENDU_BASE_PLUGIN_H
