/*
* Created by boil on 2023/11/3.
*/

#ifndef RENDU_BASE_SINGLETON_H
#define RENDU_BASE_SINGLETON_H

#include "non_copyable.h"
#include "number/int_helper.h"

RD_NAMESPACE_BEGIN

  namespace detail {
    template<typename T>
    struct has_no_destroy {
      template<typename C>
      static char test(decltype(&C::no_destroy));

      template<typename C>
      static std::int32_t test(...);

      const static bool value = sizeof(test<T>(0)) == 1;
    };
  }  // namespace detail

  template<typename T>
  class Singleton : NonCopyable {
  public:
    Singleton() = delete;

    ~Singleton() = delete;

    static T &instance() {
      pthread_once(&ponce_, &Singleton::init);
      assert(value_ != NULL);
      return *value_;
    }

  private:
    static void init() {
      value_ = new T();
      if (!detail::has_no_destroy<T>::value) {
        ::atexit(destroy);
      }
    }

    static void destroy() {
      typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
      T_must_be_complete_type dummy;
      (void) dummy;

      delete value_;
      value_ = NULL;
    }

  private:
    static pthread_once_t ponce_;
    static T *value_;
  };

  template<typename T>
  pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

  template<typename T>
  T *Singleton<T>::value_ = NULL;


#define INSTANCE_IMP(class_name, ...) \
class_name &class_name::Instance() { \
    static std::shared_ptr<class_name> s_instance(new class_name(__VA_ARGS__)); \
    static class_name &s_instance_ref = *s_instance; \
    return s_instance_ref; \
}

RD_NAMESPACE_END

#endif //RENDU_BASE_SINGLETON_H
