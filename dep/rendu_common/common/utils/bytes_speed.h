/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BYTES_SPEED_H
#define RENDU_BYTES_SPEED_H

#include "common/define.h"
#include "common/time/time_ticker.h"

RD_NAMESPACE_BEGIN

  class BytesSpeed {
  public:
    BytesSpeed() = default;

    ~BytesSpeed() = default;

    /**
     * 添加统计字节
     */
    BytesSpeed &operator+=(size_t bytes) {
      _bytes += bytes;
      if (_bytes > 1024 * 1024) {
        //数据大于1MB就计算一次网速
        computeSpeed();
      }
      return *this;
    }

    /**
     * 获取速度，单位bytes/s
     */
    int getSpeed() {
      if (_ticker.ElapsedTime() < 1000) {
        //获取频率小于1秒，那么返回上次计算结果
        return _speed;
      }
      return computeSpeed();
    }

  private:
    int computeSpeed() {
      auto elapsed = _ticker.ElapsedTime();
      if (!elapsed) {
        return _speed;
      }
      _speed = (int) (_bytes * 1000 / elapsed);
      _ticker.ResetTime();
      _bytes = 0;
      return _speed;
    }

  private:
    int _speed = 0;
    size_t _bytes = 0;
    Ticker _ticker;
  };

RD_NAMESPACE_END

#endif //RENDU_BYTES_SPEED_H
