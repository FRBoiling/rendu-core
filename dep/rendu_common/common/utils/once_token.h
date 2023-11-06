/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_ONCE_TOKEN_H
#define RENDU_ONCE_TOKEN_H

#include <functional>

#include "common/define.h"

RD_NAMESPACE_BEGIN

  class OnceToken {
  public:
    using task = std::function<void(void)>;

    template<typename FUNC>
    OnceToken(const FUNC &onConstructed, task onDestructed = nullptr) {
      onConstructed();
      _onDestructed = std::move(onDestructed);
    }

    OnceToken(std::nullptr_t, task onDestructed = nullptr) {
      _onDestructed = std::move(onDestructed);
    }

    ~OnceToken() {
      if (_onDestructed) {
        _onDestructed();
      }
    }

  private:
    OnceToken() = delete;

    OnceToken(const OnceToken &) = delete;

    OnceToken(OnceToken &&) = delete;

    OnceToken &operator=(const OnceToken &) = delete;

    OnceToken &operator=(OnceToken &&) = delete;

  private:
    task _onDestructed;
  };

RD_NAMESPACE_END

#endif //RENDU_ONCE_TOKEN_H
