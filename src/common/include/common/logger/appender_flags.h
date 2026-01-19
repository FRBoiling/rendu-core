//
// Created by 沸腾 on 2026/1/12.
//

#ifndef RENDU_APPENDER_FLAGS_H
#define RENDU_APPENDER_FLAGS_H

#include "common/define.h"

BEGIN_NAMESPACE_COMMON
    enum class AppenderFlags : uint8
    {
        APPENDER_FLAGS_NONE = 0x00,
        APPENDER_FLAGS_PREFIX_TIMESTAMP = 0x01,
        APPENDER_FLAGS_PREFIX_LOGLEVEL = 0x02,
        APPENDER_FLAGS_PREFIX_LOGFILTERTYPE = 0x04,
        APPENDER_FLAGS_USE_TIMESTAMP = 0x08, // only used by FileAppender
        APPENDER_FLAGS_MAKE_FILE_BACKUP = 0x10, // only used by FileAppender
        APPENDER_FLAGS_ASYNC = 0x20, // 异步写入模式（基于 Asio）
        APPENDER_FLAGS_ASYNC_BATCH_SIZE_MASK = 0xC0 // 批量大小掩码（高2位用于存储批量大小）
    };

    // 位运算符重载，支持 enum class 的位操作
    inline AppenderFlags operator|(AppenderFlags a, AppenderFlags b)
    {
        return static_cast<AppenderFlags>(static_cast<uint8>(a) | static_cast<uint8>(b));
    }

    inline AppenderFlags operator&(AppenderFlags a, AppenderFlags b)
    {
        return static_cast<AppenderFlags>(static_cast<uint8>(a) & static_cast<uint8>(b));
    }

    inline AppenderFlags operator^(AppenderFlags a, AppenderFlags b)
    {
        return static_cast<AppenderFlags>(static_cast<uint8>(a) ^ static_cast<uint8>(b));
    }

    inline AppenderFlags operator~(AppenderFlags a)
    {
        return static_cast<AppenderFlags>(~static_cast<uint8>(a));
    }

    inline AppenderFlags& operator|=(AppenderFlags& a, AppenderFlags b)
    {
        a = a | b;
        return a;
    }

    inline AppenderFlags& operator&=(AppenderFlags& a, AppenderFlags b)
    {
        a = a & b;
        return a;
    }

    inline AppenderFlags& operator^=(AppenderFlags& a, AppenderFlags b)
    {
        a = a ^ b;
        return a;
    }

END_NAMESPACE_COMMON
#endif //RENDU_APPENDER_FLAGS_H
