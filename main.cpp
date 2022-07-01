#include <iostream>
#include <string>

float calc_final(float base, float ratio) {
  return base * (1.0 + ratio);
}

void print_message(const char *name, const char *date, const char *event, int cur_year, float cur_count) {
  std::cout << "【" << cur_year << date << "】" << name << event << "（月工资为：" << cur_count << "）\n";
}

const float ratio = 0.1;
const float jump_ratio = 0.15;
const int base_year = 2016;
const int now_year = 2022;
const char *name = "小花";

int main(int, char **) {
  float base_count = 18000;
  int year = 6;
  float cur_count = base_count;
  int cur_year = base_year;

  std::cout << "按照不跳涨幅" << ratio * 100 << "%,跳槽涨幅" << jump_ratio * 100 << "%算!\n";
  print_message(name, "年10月", "离开了糖果", cur_year, cur_count);

  for (int i = 0; i < year; ++i) {
    cur_year += 1;
    if (i % 2 == 0 && i > 0) {
      print_message(name, "年初", "跳槽了", cur_year, cur_count);
      cur_count = calc_final(cur_count, jump_ratio);
    } else {
      if (i > 0) {
        print_message(name, "年初", "努力搬砖ing", cur_year, cur_count);
      }
    }
    cur_count = calc_final(cur_count, ratio);
    print_message(name, "年底", "涨薪了", cur_year, cur_count);
    std::cout << "---------------------------------------------------------------" << std::endl;
  }

  std::cout << "Hello, 雷森破!\n";
}
