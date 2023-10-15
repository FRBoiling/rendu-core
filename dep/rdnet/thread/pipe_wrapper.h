/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_PIPE_WRAPPER_H
#define RENDU_PIPE_WRAPPER_H

#include "define.h"

RD_NAMESPACE_BEGIN

  class PipeWrapper {
  public:
    PipeWrapper();

    ~PipeWrapper();

    int Read(void *buf, int n);

    int Write(const void *buf, int n);

    int ReadFD() const { return _pipe_fd[0]; }

    int WriteFD() const { return _pipe_fd[1]; }

    void reOpen();

  private:
    void clearFD();

  private:
    int _pipe_fd[2] = {-1, -1};
  };

RD_NAMESPACE_END

#endif //RENDU_PIPE_WRAPPER_H
