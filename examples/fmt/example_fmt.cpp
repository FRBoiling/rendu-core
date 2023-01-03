/*
* Created by boil on 2022/10/23.
*/

#ifndef RENDU_MAIN_H_
#define RENDU_MAIN_H_

#include "absl/algorithm/algorithm.h"
#include <algorithm>
#include <list>
#include <vector>
#include "absl/base/config.h"
#include <iostream>

int main(int argc, char **argv) {
    std::vector<int> v1{1, 2, 3};
    std::vector<int> v2 = v1;
    std::vector<int> v3 = {1, 2};
    std::vector<int> v4 = {1, 2, 4};
    bool is_equal = false;
    is_equal = absl::equal(v1.begin(), v1.end(), v2.begin(), v2.end());
    std::cout << 1 << ":" << is_equal << std::endl;
    is_equal = absl::equal(v1.begin(), v1.end(), v3.begin(), v3.end());
    std::cout << 2 << ":" << is_equal << std::endl;
    is_equal = absl::equal(v1.begin(), v1.end(), v4.begin(), v4.end());
    std::cout << 3 << ":" << is_equal << std::endl;
}

#endif //RENDU_MAIN_H_
