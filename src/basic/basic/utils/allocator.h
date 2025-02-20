/*
* Created by boil on 2024/7/9.
*/

#ifndef RENDU_BASIC_BASIC_TYPE_ALLOCATOR_H_
#define RENDU_BASIC_BASIC_TYPE_ALLOCATOR_H_

#include "type/type_include.h"
#include "std_ex.h"
#include "struct.h"

RD_NAMESPACE_BEGIN
/**
 * 仅用于生成Object类型的类
 */
class Allocator : public Object {
public:
  /**
     * 生成一个 Object 对象
     * @tparam T
     * @return
     */
  template<typename T, enable_if_t<std::is_base_of<Object, T>::value, int> = 0>
  static T *safeMallocObject() {
    return safeMalloc<T>();
  }

  /**
     * 生成一个 CStruct 的对象
     * @tparam T
     * @return
     */
  template<typename T,
           enable_if_t<std::is_base_of<Struct, T>::value, int> = 0>
  static T *safeMallocCStruct() {
    return safeMalloc<T>();
  }

  /**
     * 生成带参数的普通指针
     * @tparam T
     * @tparam Args
     * @param args
     * @return
     */
  template<typename T, typename... Args,
           enable_if_t<std::is_base_of<Object, T>::value, int> = 0>
  static T *safeMallocTemplateObject(Args... args) {
    T *ptr = nullptr;
    while (!ptr) {
      ptr = new T(std::forward<Args>(args)...);
    }
    return ptr;
  }


  /**
     * 生成unique智能指针信息
     * @tparam T
     * @return
     */
  template<typename T,
           enable_if_t<std::is_base_of<Object, T>::value, int> = 0>
  static std::unique_ptr<T> makeUniqueObject() {
    return make_unique<T>();
  }

private:
  /**
     * 生成T类型的对象
     * @tparam T
     * @return
     */
  template<class T>
  static T *safeMalloc() {
    T *ptr = nullptr;
    while (!ptr) {
      ptr = new (std::nothrow) T();
    }
    return ptr;
  }
};


#define RD_SAFE_MALLOC_OBJECT(Type) \
  Allocator::safeMallocObject<Type>();

#define RD_MAKE_UNIQUE_OBJECT(Type) \
  Allocator::makeUniqueObject<Type>();

RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_TYPE_ALLOCATOR_H_
