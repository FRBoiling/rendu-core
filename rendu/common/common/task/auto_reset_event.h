/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_COMMON_AUTO_RESET_EVENT_H
#define RENDU_COMMON_AUTO_RESET_EVENT_H

#include "common/define.h"
# include <mutex>
# include <condition_variable>

COMMON_NAMESPACE_BEGIN

    class auto_reset_event {
    public:

      auto_reset_event(bool initiallySet = false);

      ~auto_reset_event();

      void set();

      void wait();

    private:

      std::mutex m_mutex;
      std::condition_variable m_cv;
      bool m_isSet;

    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_AUTO_RESET_EVENT_H
