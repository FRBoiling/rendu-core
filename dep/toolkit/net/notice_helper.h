/*
* Created by boil on 2023/10/29.
*/

#ifndef RENDU_NOTICE_HELPER_H
#define RENDU_NOTICE_HELPER_H

#include "notice_center.h"

RD_NAMESPACE_BEGIN

  template<typename T>
  struct NoticeHelper;

  template<typename RET, typename... Args>
  struct NoticeHelper<RET(Args...)> {
  public:
    template<typename... ArgsType>
    static int emit(const std::string &event, ArgsType &&...args) {
      return emit(NoticeCenter::Instance(), event, std::forward<ArgsType>(args)...);
    }

    template<typename... ArgsType>
    static int emit(NoticeCenter &center, const std::string &event, ArgsType &&...args) {
      return center.emitEventSafe(event, std::forward<Args>(args)...);
    }
  };

#define NOTICE_EMIT(types, ...) NoticeHelper<void(types)>::emit(__VA_ARGS__)

RD_NAMESPACE_END
#endif //RENDU_NOTICE_HELPER_H
