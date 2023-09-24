#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/hourly_file_sink.h>

int main(){
//  auto rotatingSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/logfile", 3600, 24); // 每小时切换一次日志文件
// 创建按小时滚动的日志文件，保留一个月的日志文件
  auto sink = std::make_shared<spdlog::sinks::hourly_file_sink_mt>("logs/logfile.txt", 0, 24 * 30); // 24 * 30 表示一个月的天数

// 创建日志记录器并将 sink 添加到记录器中
  auto logger = std::make_shared<spdlog::logger>("logger", sink);

// 设置默认日志记录器
  spdlog::set_default_logger(logger);

// 记录日志
  spdlog::info("This is a log message");
  spdlog::default_logger()->sinks().push_back(sink);
}