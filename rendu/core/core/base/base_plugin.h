/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_PLUGIN_BASE_PLUGIN_H_
#define RENDU_CORE_PLUGIN_BASE_PLUGIN_H_

#include "define.h"
#include "framework/plugin.h"
#include "input/command_line_plugin.h"
#include "net/net_plugin.h"
#include "base/base_system.h"

namespace rendu {

class base_plugin final : public plugin {
 public:
  struct config {
    std::string win_title;
    int cap_fps;
  };

 private:
  command_line_plugin m_cmd_line;
  std::shared_ptr<base_system> m_base_system;

 public:
  explicit base_plugin(config &config)
      : m_cmd_line(config.win_title, config.cap_fps),
        m_base_system(std::make_shared<base_system>()) {}

  void mount(main_loop &main_loop) override;
};

}

#endif //RENDU_CORE_PLUGIN_BASE_PLUGIN_H_
