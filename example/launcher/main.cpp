/*
* Created by boil on 2023/2/4.
*/

#include <iostream>
#include <map>
#include <list>
#include <vector>

int main() {
  std::map<int, int> ttt_map;
  for (int i = 0; i < 10; ++i) {
    ttt_map[i] = i + 100;
  }
  for (auto it = ttt_map.begin(); it != ttt_map.end();) {
   it = ttt_map.erase(it);
  }

  std::vector<int> ttt_list;
  for (int i = 0; i < 10; ++i) {
    ttt_list.push_back(i);
  }
  for (auto it = ttt_list.begin(); it != ttt_list.end();) {
    it = ttt_list.erase(it);
  }

  return 0;
}
