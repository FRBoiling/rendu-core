//#include "task_test.h"
//#include "task_test1.h"
//#include "task_test2.h"
#include <iostream>

using namespace std;

class Data {
public:
  byte data[8];
public:
  Data() : data{} {
    cout << "Data 构造" << endl;
  }

//  Data(const Data &m) {
//    cout << "Data 拷贝构造" << endl;
////    this->data = m.Data;
//  }

  ~Data() {
    cout << "Data 析构" << endl;
  }
};

class Man {
public:
  Man() {
    cout << "Man 构造" << endl;
    data = new Data();
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


Man get() {
  Man m;
  for (int i = 0; i < 8; ++i) {
    m.data->data[i] = byte{i};
  }
  cout << "----" << endl;
  return m;
}

static int32_t ToInt32(const byte *data, int start_index) {
  auto length = sizeof(data);
  if (length < sizeof(int32_t)) {
    // 处理长度不足的情况
    // 可以抛出异常或返回一个默认值，具体根据需求而定
    return 0;
  }

  int32_t result;
  std::memcpy(&result, data, sizeof(int32_t));
  return result;
}

namespace Array {

    template<typename T>
    void Copy(const T *src, size_t src_index, T *dest, size_t dest_index, size_t length) {
      // 检查索引和长度是否合法，如果不合法，抛出异常
      if (src_index + length > sizeof(src) || dest_index + length > sizeof(dest)) {
        throw std::out_of_range("Array index out of range");
      }
      // 使用 std::copy 算法复制数组的一部分
      std::memcpy(dest + dest_index, src + src_index, sizeof(T) * length);
    }

    template<typename T>
    void Copy(const T *src, T *dest) {
      if (sizeof(src) < sizeof(dest)) {
        // 长度是否合法，如果不合法，抛出异常
        // 目标内存过小 TODO：BOIL
        return;
      }
      std::memcpy(dest, src, sizeof(T));
    }
}

#include <limits>

int main() {
  const int a1[5] = {1, 2, 3, 4, 5};
  int a2[6] = {};


  auto size1 = sizeof(a1);
  auto size2 = sizeof(a2);
  Array::Copy(a1, 1, a2, 0, 4);

  auto size = sizeof(size_t);

  Man m = get();;
  //cout << "before m=" << &m << "n=" << &n << endl;
//  printf("m.data is %d\n", m.data->data);

  auto size_n = ToInt32(reinterpret_cast<const byte *>(m.data), 1);

  return 0;
//  task_test_fuc();
//  task_test1();
// task_test2::task_test_fuc();
}



