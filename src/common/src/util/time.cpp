#include <common/util/time.h>
#include <common/define.h>
#include <thread>
#include <ctime>
#include <sstream>
#include <iomanip>

BEGIN_NAMESPACE_COMMON
namespace time {

int64_t now_ms() {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

int64_t now_us() {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

std::string format_time(int64_t timestamp_ms, const char* format) {
    std::time_t tt = static_cast<std::time_t>(timestamp_ms / 1000);
    std::tm* tm = std::localtime(&tt);

    std::ostringstream oss;
    oss << std::put_time(tm, format);
    return oss.str();
}

int64_t parse_time(const std::string& time_str, const char* format) {
    std::tm tm = {};
    std::istringstream iss(time_str);
    iss >> std::get_time(&tm, format);

    if (iss.fail()) {
        return 0;
    }

    std::time_t tt = std::mktime(&tm);
    return static_cast<int64_t>(tt) * 1000;
}

void sleep_ms(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

Stopwatch::Stopwatch()
    : start_(std::chrono::steady_clock::now()) {}

void Stopwatch::reset() {
    start_ = std::chrono::steady_clock::now();
}

int64_t Stopwatch::elapsed_ms() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = now - start_;
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

int64_t Stopwatch::elapsed_us() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = now - start_;
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

} // namespace time
END_NAMESPACE_COMMON
