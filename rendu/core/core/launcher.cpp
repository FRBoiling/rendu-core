/*
* Created by boil on 2023/2/16.
*/

#include "launcher.h"
#include "base/host.h"
#include "io/cmd_io_plugin.h"

int rendu::Launcher::Run() {

  cmd_io_plugin::config config = {
      .win_title = "rendu core",
      .cap_fps = 60
  };

  cmd_io_plugin io(config);
  host()
      .add_plugin((plugin &) io)
      .run();

  return 0;
}
