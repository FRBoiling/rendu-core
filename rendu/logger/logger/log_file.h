/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_LOG_FILE_H
#define RENDU_LOG_FILE_H

#include "common/base/non_copyable.h"
#include "common/thread/mutex_lock.h"
#include "file/append_file.h"

RD_NAMESPACE_BEGIN

  class LogFile : NonCopyable {
  public:
    LogFile(const string &basename,
            off_t rollSize,
            bool threadSafe = true,
            int flushInterval = 3,
            int checkEveryN = 1024);

    ~LogFile();

    void append(const char *logline, int len);

    void flush();

    bool rollFile();

  private:
    void append_unlocked(const char *logline, int len);

    static string getLogFileName(const string &basename, time_t *now);

    const string basename_;
    const off_t rollSize_;
    const int flushInterval_;
    const int checkEveryN_;

    int count_;

    std::unique_ptr<MutexLock> mutex_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    std::unique_ptr<AppendFile> file_;

    const static int kRollPerSeconds_ = 60 * 60 * 24;
  };

RD_NAMESPACE_END

#endif //RENDU_LOG_FILE_H
