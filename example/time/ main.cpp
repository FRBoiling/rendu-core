#include <iostream>
#include <chrono>
#include <iomanip>
#include "date/tz.h"

int main() {
  // 获取当前本地时间点
  auto now_time = std::chrono::system_clock::now();

  // 获取当前系统的时区信息
  auto cur_tz = date::current_zone();
  std::string cur_time_zone = cur_tz->name();


  // 将本地时间转换为带有时区信息的时间点
  auto cur_zt = date::make_zoned(cur_time_zone, now_time);
  auto system_t = cur_zt.get_sys_time();


  // 设置目标时区
  std::string target_time_zone2 = "America/New_York";
  // 将时间点转换为目标时区的时间
  auto target_zt2 = date::make_zoned(target_time_zone2, now_time);

//  std::string target_time_zone3 =  "Europe/London";
  std::string target_time_zone3 =  "UTC";
  // 将时间点转换为目标时区的时间
  auto target_zt3 = date::make_zoned(target_time_zone3, now_time);

  // 将时间点转换为 time_t 类型
  std::time_t utc_time_t = std::chrono::system_clock::to_time_t(now_time);
  std::tm* utc_time = std::gmtime(&utc_time_t);
  // 输出目标时区的时间
  std::cout << "UTC时间：" << std::put_time(utc_time, "%Y-%m-%d %H:%M:%S") << std::endl;
  std::cout << "目标时区(" << cur_time_zone << "）的时间：" << cur_zt << std::endl;
  std::cout << "目标时区(" << target_time_zone2 << "）的时间：" << target_zt2 << std::endl;
  std::cout << "目标时区(" << target_time_zone3 << "）的时间：" << target_zt3 << std::endl;

  return 0;
}
