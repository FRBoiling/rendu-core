/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BUFFER_RAW_H
#define RENDU_BUFFER_RAW_H

#include "a_buffer.h"
#include "utils/resource_pool.h"

RD_NAMESPACE_BEGIN

//指针式缓存对象，
  class BufferRaw : public ABuffer {
  public:
    using Ptr = std::shared_ptr<BufferRaw>;

    static Ptr Create();

  protected:
    friend class ResourcePool_l<BufferRaw>;

    explicit BufferRaw(size_t capacity = 0);

    explicit BufferRaw(const char *data, size_t size = 0);

  public:
    ~BufferRaw() override;

  public:
    //在写入数据时请确保内存是否越界
    char *Data() const override {
      return _data;
    }

    //有效数据大小
    size_t Size() const override {
      return _size;
    }

    size_t GetCapacity() const override {
      return _capacity;
    }

  public:
    //设置有效数据大小
    virtual void SetSize(size_t size);

    //分配内存大小
    void SetCapacity(size_t capacity);

    //赋值数据
    void Assign(const char *data, size_t size = 0);

  private:
    size_t _size = 0;
    size_t _capacity = 0;
    char *_data = nullptr;
    //对象个数统计
    ObjectStatistic<BufferRaw> _statistic;
  };


RD_NAMESPACE_END

#endif //RENDU_BUFFER_RAW_H
