/*
* Created by boil on 2022/12/31.
*/

#include "loader_include.h"

using namespace rendu;

int main(int argc, char **argv) {
  Host host;
  host.Start();

  while (host.isRunning) {
    try {
      host.Update();
      host.LateUpdate();
    }
    catch (const Exception &ex) {
      Log::Error(ex);
    }
  }
  return 0;
}
