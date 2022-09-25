/*
* Created by boil on 2022/9/12.
*/
#include <sfmt.h>
#include <random>
#include <define.h>
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"

void test_random() {

}

void example_random() {
  sfmt_t state;
//  std::random_device dev;
//  if (dev.entropy() > 0) {
//    std::array<uint32, SFMT_N32> seed{};
//    std::generate(seed.begin(), seed.end(), std::ref(dev));
//
//    sfmt_init_by_array(&state, seed.data(), seed.size());
//  } else
  sfmt_init_gen_rand(&state, uint32(time(nullptr)));

  spdlog::stopwatch sw;
//  std::this_thread::sleep_for(std::chrono::milliseconds(123));
  for (int i = 0; i < 100; ++i) {
    auto a = sfmt_genrand_uint32(&state);
    spdlog::info("--->{}", a);
  }
  spdlog::info("Stopwatch: {} seconds", sw);
}


int main(int argc, char **argv) {
  test_sfmt_random();
}