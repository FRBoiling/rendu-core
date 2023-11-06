/*
* Created by boil on 2023/11/8.
*/

#ifndef RENDU_PIPE_H
#define RENDU_PIPE_H

#include "sockets/sock_ops.h"

RD_NAMESPACE_BEGIN

  class Pipe {
  public:
    Pipe();
    ~Pipe();
  public:
    int Write(const void *buf, int n);

    int Read(void *buf, int n);

    int ReadFD() const { return _pipe_fd[0]; }

    int WriteFD() const { return _pipe_fd[1]; }

    void ReOpen();

    void ClearFD();

  private:
    int _pipe_fd[2] = {-1, -1};
  };

RD_NAMESPACE_END

#endif //RENDU_PIPE_H
