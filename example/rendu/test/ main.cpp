#include "log.h"
#include "base/utils/convert.h"

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
  RD_LOGGER_ADD_CHANNEL(new ConsoleChannel());
  RD_LOGGER_INIT("async_example", LogLevel::LL_TRACE);
  ttt();
}