/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_BUFFER_SOCKET_H
#define RENDU_BUFFER_SOCKET_H

#include "a_buffer.h"
#include "adapter/socket/socket_adapter.h"

RD_NAMESPACE_BEGIN

  class BufferSocket : public ABuffer {
  public:
    using Ptr = std::shared_ptr<BufferSocket>;

    explicit BufferSocket(ABuffer::Ptr ptr, struct sockaddr *addr = nullptr, int addr_len = 0);

    ~BufferSocket() override = default;

  public:
    char *Data() const override;

    size_t Size() const override;

  public:
    [[nodiscard]] const struct sockaddr *GetSockAddr() const;

    [[nodiscard]] socklen_t GetSockLen() const;

  private:
    int _addr_len = 0;
    struct sockaddr_storage _addr;
    ABuffer::Ptr _buffer;
  };

RD_NAMESPACE_END

#endif //RENDU_BUFFER_SOCKET_H
