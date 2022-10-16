/*
* Created by boil on 2022/9/6.
*/

#ifndef RENDU_SINGLETON_H_
#define RENDU_SINGLETON_H_


template<typename T>
class Singleton {
public:
  static T &get_inst() {
    static T instance;
    return instance;
  }

  Singleton(T &&) = delete;

  Singleton(const T &) = delete;

  void operator=(const T &) = delete;

protected:
  Singleton() = default;

  virtual ~Singleton() = default;
};

#endif //RENDU_SINGLETON_H_
