/*
* Created by boil on 2022/12/31.
*/

#include "app.h"
#include "log.h"
#include <exception>

using namespace rendu;
using namespace rendu::log;

int main(int argc, char **argv) {
  App::Start(argc, argv);
  while (true) {
    try {
      App::Update();
      App::LateUpdate();
    }
    catch (const std::exception &ex) {
      RD_CRITICAL("{}", ex.what());
    }
  }
  return 0;
}
