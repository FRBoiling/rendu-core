#include "logger/log.h"

using namespace rendu;

int main() {
  LOG_TRACE <<"LOG_TRACE";
  LOG_DEBUG <<"LOG_DEBUG";
  LOG_WARN <<"LOG_WARN";
  LOG_ERROR <<"LOG_ERROR";
  LOG_FATAL <<"LOG_FATAL";

  LOG_SYSERR <<"LOG_SYSERR";
  LOG_SYSFATAL <<"LOG_SYSFATAL";
  return 0;
}