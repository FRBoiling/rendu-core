///*
//* Created by boil on 2023/10/26.
//*/
//
//#include "file_channel.h"
//
//RD_NAMESPACE_BEGIN
//
//  FileChannel::FileChannel(const string &name, const string &path, LogLevel::Level level) : ALogChannel(name, level), _path(path) {}
//
//  FileChannel::~FileChannel() {
//    close();
//  }
//
//  void FileChannel::Write(const Logger &logger, const std::shared_ptr<LogContext> &ctx) {
//    if (_level > ctx->_level) {
//      return;
//    }
//    if (!_fstream.is_open()) {
//      open();
//    }
//    //打印至文件
//    Format(logger, _fstream, ctx);
//  }
//
//  bool FileChannel::SetPath(const string &path) {
//    _path = path;
//    return open();
//  }
//
//  const string &FileChannel::GetPath() const {
//    return _path;
//  }
//
//  bool FileChannel::open() {
//    // Ensure a path was set
//    if (_path.empty()) {
//      throw runtime_error("Log file path must be set");
//    }
//    // Open the file stream
//    _fstream.close();
//#if !defined(_WIN32)
//    //创建文件夹
//    File::create_path(_path.data(), S_IRWXO | S_IRWXG | S_IRWXU);
//#else
//    File::create_path(_path.data(),0);
//#endif
//    _fstream.open(_path.data(), ios::out | ios::app);
//    if (!_fstream.is_open()) {
//      return false;
//    }
//    //打开文件成功
//    return true;
//  }
//
//  void FileChannel::close() {
//    _fstream.close();
//  }
//
//  size_t FileChannel::size() {
//    return (_fstream << std::flush).tellp();
//  }
//
/////////////////////FileChannel///////////////////
//
//  static const auto s_second_per_day = 24 * 60 * 60;
//
////根据GMT UNIX时间戳生产日志文件名
//  static string getLogFilePath(const string &dir, time_t second, int32_t index) {
//    auto tm = getLocalTime(second);
//    char buf[64];
//    snprintf(buf, sizeof(buf), "%d-%02d-%02d_%02d.log", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, index);
//    return dir + buf;
//  }
//
////根据日志文件名返回GMT UNIX时间戳
//  static time_t getLogFileTime(const string &full_path) {
//    auto name = getFileName(full_path.data());
//    struct tm tm{0};
//    if (!strptime(name, "%Y-%m-%d", &tm)) {
//      return 0;
//    }
//    //此函数会把本地时间转换成GMT时间戳
//    return mktime(&tm);
//  }
//
////获取1970年以来的第几天
//  static uint64_t getDay(time_t second) {
//    return (second + getGMTOff()) / s_second_per_day;
//  }
//
//  FileChannel::FileChannel(const string &name, const string &dir, LogLevel level) : FileChannel(name, "", level) {
//    _dir = dir;
//    if (_dir.back() != '/') {
//      _dir.append("/");
//    }
//
//    //收集所有日志文件
//    File::scanDir(_dir, [this](const string &path, bool isDir) -> bool {
//      if (!isDir && end_with(path, ".log")) {
//        _log_file_map.emplace(path);
//      }
//      return true;
//    }, false);
//
//    //获取今天日志文件的最大index号
//    auto log_name_prefix = getTimeStr("%Y-%m-%d_");
//    for (auto it = _log_file_map.begin(); it != _log_file_map.end(); ++it) {
//      auto name = getFileName(it->data());
//      //筛选出今天所有的日志文件
//      if (start_with(name, log_name_prefix)) {
//        int tm_mday;  // day of the month - [1, 31]
//        int tm_mon;   // months since January - [0, 11]
//        int tm_year;  // years since 1900
//        uint32_t index;
//        //今天第几个文件
//        int count = sscanf(name, "%d-%02d-%02d_%d.log", &tm_year, &tm_mon, &tm_mday, &index);
//        if (count == 4) {
//          _index = index >= _index ? index : _index;
//        }
//      }
//    }
//  }
//
//  void FileChannel::Write(const Logger &logger, const LogContextPtr &ctx) {
//    //GMT UNIX时间戳
//    time_t second = ctx->_tv.tv_sec;
//    //这条日志所在第几天
//    auto day = getDay(second);
//    if ((int64_t) day != _last_day) {
//      if (_last_day != -1) {
//        //重置日志index
//        _index = 0;
//      }
//      //这条日志是新的一天，记录这一天
//      _last_day = day;
//      //获取日志当天对应的文件，每天可能有多个日志切片文件
//      changeFile(second);
//    } else {
//      //检查该天日志是否需要重新分片
//      checkSize(second);
//    }
//
//    //写日志
//    if (_can_write) {
//      FileChannel::write(logger, ctx);
//    }
//  }
//
//  void FileChannel::clean() {
//    //获取今天是第几天
//    auto today = getDay(time(nullptr));
//    //遍历所有日志文件，删除超过若干天前的过期日志文件
//    for (auto it = _log_file_map.begin(); it != _log_file_map.end();) {
//      auto day = getDay(getLogFileTime(it->data()));
//      if (today < day + _log_max_day) {
//        //这个日志文件距今尚未超过一定天数,后面的文件更新，所以停止遍历
//        break;
//      }
//      //这个文件距今超过了一定天数，则删除文件
//      File::delete_file(it->data());
//      //删除这条记录
//      it = _log_file_map.erase(it);
//    }
//
//    //按文件个数清理，限制最大文件切片个数
//    while (_log_file_map.size() > _log_max_count) {
//      auto it = _log_file_map.begin();
//      if (*it == path()) {
//        //当前文件，停止删除
//        break;
//      }
//      //删除文件
//      File::delete_file(it->data());
//      //删除这条记录
//      _log_file_map.erase(it);
//    }
//  }
//
//  void FileChannel::checkSize(time_t second) {
//    //每60秒检查一下文件大小，防止频繁flush日志文件
//    if (second - _last_check_time > 60) {
//      if (FileChannel::size() > _log_max_size * 1024 * 1024) {
//        changeFile(second);
//      }
//      _last_check_time = second;
//    }
//  }
//
//  void FileChannel::changeFile(time_t second) {
//    auto log_file = getLogFilePath(_dir, second, _index++);
//    //记录所有的日志文件，以便后续删除老的日志
//    _log_file_map.emplace(log_file);
//    //打开新的日志文件
//    _can_write = setPath(log_file);
//    if (!_can_write) {
//      ErrorL << "Failed to open log file: " << _path;
//    }
//    //尝试删除过期的日志文件
//    clean();
//  }
//
//  void FileChannel::setMaxDay(size_t max_day) {
//    _log_max_day = max_day > 1 ? max_day : 1;
//  }
//
//  void FileChannel::setFileMaxSize(size_t max_size) {
//    _log_max_size = max_size > 1 ? max_size : 1;
//  }
//
//  void FileChannel::setFileMaxCount(size_t max_count) {
//    _log_max_count = max_count > 1 ? max_count : 1;
//  }
//
//RD_NAMESPACE_END