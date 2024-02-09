#include "base/utils/convert.h"
#include "log.hpp"

using namespace rendu;
using namespace rendu::log;

class A{
public:
  A(){
    RD_TRACE("A+ {}",Convert::ToString(this));
  }
  ~A(){
    RD_TRACE("A- {}",Convert::ToString(this));
  }

};

void ttt(){
  A a;
}



int main(){
  RD_LOGGER_CLEAN();
  RD_LOGGER_SET(new Logger());
  RD_LOGGER_INIT("async_example", LogLevel::LL_TRACE);

  byte data[3];
  data[0] = 0x11;
  RD_DEBUG("data: {}",sizeof(data));
  ttt();
}