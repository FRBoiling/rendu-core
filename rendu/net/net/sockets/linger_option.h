/*
* Created by boil on 2024/2/20.
*/

#ifndef RENDU_NET_NET_SOCKETS_LINGER_OPTION_H_
#define RENDU_NET_NET_SOCKETS_LINGER_OPTION_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

class LingerOption {

private:
  bool m_enabled;
  int m_lingerTime;

public:
  LingerOption(bool enable, int seconds) :m_enabled(enable), m_lingerTime(seconds) {}


};

NET_NAMESPACE_END


#endif//RENDU_NET_NET_SOCKETS_LINGER_OPTION_H_
