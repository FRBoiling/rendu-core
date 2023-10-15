/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_A_BUFFER_H
#define RENDU_A_BUFFER_H

#include "define.h"
#include "utils/noncopyable.h"
#include "utils/object_statistic.h"

RD_NAMESPACE_BEGIN
//抽象缓存基类
  class ABuffer : public noncopyable {
  public:
    using Ptr = std::shared_ptr<ABuffer>;

    ABuffer() = default;

    virtual ~ABuffer() = default;

  public:
    //返回数据长度
    [[nodiscard]] virtual char *Data() const = 0;

    [[nodiscard]] virtual size_t Size() const = 0;

    [[nodiscard]] virtual string ToString() const;

    [[nodiscard]] virtual size_t GetCapacity() const;

  private:
    //对象个数统计
    ObjectStatistic<ABuffer> _statistic;
  };

RD_NAMESPACE_END

#endif //RENDU_A_BUFFER_H
