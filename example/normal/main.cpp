//#include "task_test.h"
//#include "task_test1.h"
//#include "task_test2.h"
#include <iostream>

using namespace std;

class Data {
public:
  int data;
public:
  Data(int i) {

  }

  Data() {
    cout << "Data 构造" << endl;
    data = 0;
  }

  Data(const Data &m) {
    cout << "Data 拷贝构造" << endl;
    this->data = m.data;
  }

  ~Data() {
    cout << "Data 析构" << endl;
  }
};

class Man {
public:
  Man() {
    cout << "Man 构造" << endl;
    data = new Data(0);
  }

  Man(const Man &m) {
    cout << "Man 拷贝构造" << endl;
    this->data = m.data;
  }

  ~Man() {
    cout << "Man 析构" << endl;
    delete data;
  }

  Data *data;
};


Man& get() {
  Man m;
  m.data->data = 1111;
  cout << "----" << endl;
  return m;
}


int main() {
  Man m = get();;
  //cout << "before m=" << &m << "n=" << &n << endl;
  printf("m.data is %d\n", m.data->data);


  return 0;
//  task_test_fuc();
//  task_test1();
// task_test2::task_test_fuc();
}



