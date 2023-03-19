/*
* Created by boil on 2023/2/16.
*/

#include "launcher.h"
#include "framework/main_loop.h"
#include "base/base_plugin.h"

int rendu::Launcher::Run() {

  base_plugin::config config = {
      .win_title = "Warmonger Dynasty",
      .cap_fps = 60
  };

  base_plugin base(config);
  main_loop()
      .add_plugin((plugin &) base)
      .run();

  return 0;
}
