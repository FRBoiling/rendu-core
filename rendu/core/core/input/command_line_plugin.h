/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_CORE_PLUGIN_COMMAND_LINE_PLUGIN_H_
#define RENDU_CORE_CORE_PLUGIN_COMMAND_LINE_PLUGIN_H_

#include <string>
#include "framework/plugin.h"

namespace rendu {

class command_line_plugin final : public plugin {
 public:
  command_line_plugin(std::string basic_string_1, int i);
  void mount(main_loop &game_loop) override;

};
}

#endif //RENDU_CORE_CORE_PLUGIN_COMMAND_LINE_PLUGIN_H_
