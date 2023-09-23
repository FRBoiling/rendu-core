/*
* Created by boil on 2022/12/31.
*/

#include <exception>
#include "log/logger.h"
#include "api.h"

using namespace rendu;

int main(int argc, char **argv) {
  Api::Start(argc,argv);
  while (true) {
    try {
      Api::Update();
      Api::LateUpdate();
    }
    catch (const std::exception &ex) {
      RD_CRITICAL("{}", ex.what());
    }
  }
  return 0;
}
