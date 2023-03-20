/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_IO_CMD_IO_PLUGIN_H_
#define RENDU_CORE_IO_CMD_IO_PLUGIN_H_

#include <string>
#include "base/plugin.h"

namespace rendu {

class cmd_io_plugin final : public plugin {
 public:
  struct config {
    std::string win_title;
    int cap_fps;
  };
 public:
  cmd_io_plugin(config &config)
  {}
  void mount(host &game_loop) override;

};
}

#endif //RENDU_CORE_IO_CMD_IO_PLUGIN_H_
