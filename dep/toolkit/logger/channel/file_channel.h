/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_FILE_CHANNEL_H
#define RENDU_FILE_CHANNEL_H

#include "a_log_channel.h"
#include "path/path.h"
#include <fstream>
#include <set>

RD_NAMESPACE_BEGIN

  /**
 * 自动清理的日志文件通道
 * 默认最多保存30天的日志
 */
  class FileChannel : public ALogChannel {
  public:
    FileChannel(const std::string &name = "FileChannel", const std::string &dir = PathHelper::exeDir() + "log/", LogLevel::Level level = LogLevel::LTrace);
    ~FileChannel() override = default;
  public:
  public:
    bool SetPath(const std::string &path);

    const std::string &GetPath() const;
    /**
     * 写日志时才会触发新建日志文件或者删除老的日志文件
     * @param logger
     * @param stream
     */
    void Write(const Logger &logger, const LogContext::Ptr &ctx) override;

    /**
     * 设置日志最大保存天数
     * @param max_day 天数
     */
    void setMaxDay(size_t max_day);

    /**
     * 设置日志切片文件最大大小
     * @param max_size 单位MB
     */
    void setFileMaxSize(size_t max_size);

    /**
     * 设置日志切片文件最大个数
     * @param max_count 个数
     */
    void setFileMaxCount(size_t max_count);


  private:
    bool open();

    void close();

    size_t size();

    /**
     * 删除日志切片文件，条件为超过最大保存天数与最大切片个数
     */
    void clean();

    /**
     * 检查当前日志切片文件大小，如果超过限制，则创建新的日志切片文件
     */
    void checkSize(time_t second);

    /**
     * 创建并切换到下一个日志切片文件
     */
    void changeFile(time_t second);

  private:
    std::string _path;

    std::ofstream _fstream;

    bool _can_write = false;
    //默认最多保存30天的日志文件
    size_t _log_max_day = 30;
    //每个日志切片文件最大默认128MB
    size_t _log_max_size = 128;
    //最多默认保持30个日志切片文件
    size_t _log_max_count = 30;
    //当前日志切片文件索引
    size_t _index = 0;
    int64_t _last_day = -1;
    time_t _last_check_time = 0;
    std::string _dir;
    std::set<std::string> _log_file_map;
  };

RD_NAMESPACE_END

#endif //RENDU_FILE_CHANNEL_H
