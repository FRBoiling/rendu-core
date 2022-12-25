/*
* Created by boil on 2022/12/20.
*/
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class A {
public:
  A(int _a, int _b) {
    a = _a;
    b = _b;
  };
  int a;
  int b;
};

int main(int argc, char *argv[]) {
  vector<A> test;
  for (int i = 0; i < 10; i++) {
    A a(i, 10 - i);
    test.emplace_back(a);
  }
  sort(test.begin(), test.end(), [](A x, A y) { return x.b < y.b; });
  for (int i = 0; i < 10; i++) {
    cout << test[i].a << endl;
  }
  return 1;
}