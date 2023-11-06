/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BUFF_LIST_H
#define RENDU_BUFF_LIST_H

#include "utils/non_copyable.h"
#include "utils/list_exp.h"
#include "a_buffer.h"

RD_NAMESPACE_BEGIN

  class BufferList : public NonCopyable {
  public:
    using Ptr = std::shared_ptr<BufferList>;
    using SendResult = std::function<void(const ABuffer::Ptr &buffer, bool send_success)>;

    BufferList() = default;

    virtual ~BufferList() = default;

  public:
    static Ptr Create(List<std::pair<ABuffer::Ptr, bool>> list, SendResult cb, bool is_udp);

    virtual bool IsEmpty() = 0;

    virtual size_t Count() = 0;

    virtual ssize_t Send(int fd, int flags) = 0;

  private:
    //对象个数统计
    ObjectStatistic<BufferList> _statistic;
  };

RD_NAMESPACE_END


#endif //RENDU_BUFF_LIST_H
