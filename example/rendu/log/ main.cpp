
#include "log.h"
#include "log_example.h"
#include <thread>

class A {

public:
  static int n;
  static int init() {
    n = 0;
    return n;
  }
  static int test() {
    return n++;
  }
  static void test1() {
    RD_INFO("{}", 1);
  }
};

int A::n;

void threadFunction1() {
  auto a = A::test();
  RD_INFO("{}", a);
}

void threadFunction2() {
  auto a = A::test();
  RD_INFO("{}", a);
}

class A1 {
public:
  int n;
~A1(){
    RD_INFO("A1 - ");
}
public:
  int init() {
    n = 0;
    return n;
  }
  virtual int test() {
    RD_INFO("A! ");
    return n++;
  }

  virtual int test1() {
    return n++;
  }
};

class B : public A1 {
  ~B(){
    RD_INFO("B - ");
  }
public:
  int test() override {
    RD_INFO("B ");
    return n++;
  }
};

int main() {
  RD_LOGGER_CLEAN();
  RD_LOGGER_ADD_CHANNEL(new ConsoleChannel());
  RD_LOGGER_INIT("async_example", LogLevel::LL_DEBUG);

  A1* a1 = new B();
  a1->test();
  delete(a1);

//  delete(a1);
//
//  int arr[10];
//  std::string str = "abced";
//  RD_INFO("size of arr {}", sizeof(str))
//
//  RD_INFO("size() arr {}", str.size())
//  RD_INFO("length() arr {}", str.length())
//
//
//  RD_INFO("size of A1 {}", sizeof(A1))
//  RD_INFO("size of B {}", sizeof(B))
//
//  A *a = nullptr;
//  a->test1();

  //  log_example();
  //  A::init();
  //  std::thread t1(threadFunction1);
  //  std::thread t2(threadFunction2);
  //
  //  t1.join();
  //  t2.join();


  while (true) {
    using namespace std::chrono_literals;
    //    LOG_INFO << "loop ..";
    RD_INFO("loop ..");
    std::this_thread::sleep_for(3s);
  }
  return 0;
}