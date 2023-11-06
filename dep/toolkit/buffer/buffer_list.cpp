/*
* Created by boil on 2023/10/26.
*/

#include "buffer_list.h"
#include "buffer_send_to.h"
#include "buffer_send_msg.h"
#include "utils/object_statistic.h"

RD_NAMESPACE_BEGIN

  StatisticImp(BufferList)

  BufferList::Ptr BufferList::Create(List<std::pair<ABuffer::Ptr, bool> > list, SendResult cb, bool is_udp) {
#if defined(_WIN32)
    if (is_udp) {
          // sendto/send 方案，待优化
          return std::make_shared<BufferSendTo>(std::move(list), std::move(cb), is_udp);
      }
      // WSASend方案
      return std::make_shared<BufferSendMsg>(std::move(list), std::move(cb));
#elif defined(__linux__) || defined(__linux)
    if (is_udp) {
          // sendmmsg方案
          return std::make_shared<BufferSendMMsg>(std::move(list), std::move(cb));
      }
      // sendmsg方案
      return std::make_shared<BufferSendMsg>(std::move(list), std::move(cb));
#else
    if (is_udp) {
      // sendto/send 方案, 可优化？
      return std::make_shared<BufferSendTo>(std::move(list), std::move(cb), is_udp);
    }
    // sendmsg方案
    return std::make_shared<BufferSendMsg>(std::move(list), std::move(cb));
#endif
  }

RD_NAMESPACE_END