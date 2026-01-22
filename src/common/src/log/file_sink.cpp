#include <common/log/file_sink.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <filesystem>
#include <algorithm>

BEGIN_NAMESPACE_COMMON
namespace log {

FileSink::FileSink(const std::string& filename, Level level)
    : Sink(level)
    , filename_(filename)
    , rotation_(Rotation::None)
    , max_size_(10 * 1024 * 1024) // 10MB
    , current_size_(0)
{
    file_.open(filename_, std::ios::out | std::ios::app);
    if (!file_.is_open()) {
        std::cerr << "Failed to open log file: " << filename_ << std::endl;
    }

    // 获取当前日期用于每日轮转
    last_date_ = get_current_date_string();
}

FileSink::~FileSink() {
    if (file_.is_open()) {
        file_.close();
    }
}

void FileSink::set_rotation(Rotation rotation, size_t max_size) {
    std::lock_guard<std::mutex> lock(mutex_);
    rotation_ = rotation;
    max_size_ = max_size;
}

void FileSink::write(const std::string& formatted) {
    // 检查是否需要轮转
    if (rotation_ == Rotation::Daily) {
        std::string current_date = get_current_date_string();
        if (current_date != last_date_) {
            rotate();
            last_date_ = current_date;
        }
    } else if (rotation_ == Rotation::Size) {
        if (current_size_ >= max_size_) {
            rotate();
            current_size_ = 0;
        }
    }

    if (file_.is_open()) {
        file_ << formatted << std::endl;
        file_.flush();
        current_size_ += formatted.length() + 1; // +1 for newline
    }
}

void FileSink::rotate() {
    if (!file_.is_open()) {
        return;
    }

    file_.close();

    // 查找可用的文件名
    int index = 1;
    std::string rotated_name;
    do {
        rotated_name = generate_filename(filename_, index);
        if (!std::filesystem::exists(rotated_name)) {
            break;
        }
        index++;
    } while (true);

    // 重命名当前文件
    if (std::filesystem::exists(filename_)) {
        std::filesystem::rename(filename_, rotated_name);
    }

    // 重新打开文件
    file_.open(filename_, std::ios::out | std::ios::app);
    if (!file_.is_open()) {
        std::cerr << "Failed to reopen log file after rotation: " << filename_ << std::endl;
    }
}

std::string FileSink::generate_filename(const std::string& base, int index) {
    std::filesystem::path path(base);
    std::string stem = path.stem().string();
    std::string extension = path.extension().string();

    return path.parent_path() / (stem + "." + std::to_string(index) + extension);
}

std::string FileSink::get_current_date_string() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << (tm.tm_year + 1900)
        << std::setw(2) << (tm.tm_mon + 1)
        << std::setw(2) << tm.tm_mday;
    return oss.str();
}

} // namespace log
END_NAMESPACE_COMMON
