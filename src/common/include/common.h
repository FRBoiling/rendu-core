//
// Created by FRee2 on 2025/10/18.
//

// ============================================================================
// common.h - 公共头文件，包含常用枚举、常量和工具函数
// ============================================================================

#ifndef RENDU_COMMON_H
#define RENDU_COMMON_H

#include "common/define.h"
#include <array>
#include <string>

// 启用字符串字面量后缀
using namespace std::string_literals;
using namespace std::string_view_literals;

// 字符串化宏
#define STRINGIZE(a) #a

BEGIN_NAMESPACE_COMMON

// ============================================================================
// 时间常量定义（单位：秒）
// ============================================================================

enum TimeConstants
{
    MINUTE = 60,           // 1 分钟
    HOUR   = MINUTE * 60,  // 1 小时
    DAY    = HOUR * 24,    // 1 天
    WEEK   = DAY * 7,      // 1 周
    MONTH  = DAY * 30,     // 1 月（近似）
    YEAR   = MONTH * 12,   // 1 年
    IN_MILLISECONDS = 1000 // 毫秒转换因子
};

// ============================================================================
// 账户类型定义（安全级别）
// ============================================================================

enum AccountTypes
{
    SEC_PLAYER        = 0,  // 普通玩家
    SEC_MODERATOR     = 1,  // 版主
    SEC_GAMEMASTER    = 2,  // 游戏管理员
    SEC_ADMINISTRATOR = 3,  // 管理员
    SEC_CONSOLE       = 4   // 控制台（必须位于列表末尾）
};

// ============================================================================
// 地区语言常量定义
// ============================================================================

enum LocaleConstant : uint8
{
    LOCALE_enUS = 0,   // 英语（美国）
    LOCALE_koKR = 1,   // 韩语（韩国）
    LOCALE_frFR = 2,   // 法语（法国）
    LOCALE_deDE = 3,   // 德语（德国）
    LOCALE_zhCN = 4,   // 中文（简体）
    LOCALE_zhTW = 5,   // 中文（繁体）
    LOCALE_esES = 6,   // 西班牙语（西班牙）
    LOCALE_esMX = 7,   // 西班牙语（墨西哥）
    LOCALE_ruRU = 8,   // 俄语（俄罗斯）
    LOCALE_none = 9,   // 未指定
    LOCALE_ptBR = 10,  // 葡萄牙语（巴西）
    LOCALE_itIT = 11,  // 意大利语（意大利）

    TOTAL_LOCALES      // 地区总数
};

// 旧版地区总数（待迁移）
const uint8 OLD_TOTAL_LOCALES = 9; // TODO: 转换为简单系统

// 默认地区
#define DEFAULT_LOCALE LOCALE_enUS

// ============================================================================
// CASC 地区位定义（用于资源管理）
// ============================================================================

enum class CascLocaleBit : uint8
{
    None    = 0,   // 无
    enUS    = 1,   // 英语（美国）
    koKR    = 2,   // 韩语（韩国）
    Reserved = 3,  // 保留
    frFR    = 4,   // 法语（法国）
    deDE    = 5,   // 德语（德国）
    zhCN    = 6,   // 中文（简体）
    esES    = 7,   // 西班牙语（西班牙）
    zhTW    = 8,   // 中文（繁体）
    enGB    = 9,   // 英语（英国）
    enCN    = 10,  // 英语（中国）
    enTW    = 11,  // 英语（台湾）
    esMX    = 12,  // 西班牙语（墨西哥）
    ruRU    = 13,  // 俄语（俄罗斯）
    ptBR    = 14,  // 葡萄牙语（巴西）
    itIT    = 15,  // 意大利语（意大利）
    ptPT    = 16   // 葡萄牙语（葡萄牙）
};

// ============================================================================
// 地区名称数组
// ============================================================================

RC_COMMON_API extern char const* localeNames[TOTAL_LOCALES];

/**
 * @brief 根据名称获取地区常量
 * @param name 地区名称（如 "enUS"）
 * @return 地区常量，未找到则返回 TOTAL_LOCALES
 */
RC_COMMON_API LocaleConstant GetLocaleByName(std::string_view name);

// ============================================================================
// WoW 地区到 CASC 地区位的映射表
// ============================================================================

RC_COMMON_API extern CascLocaleBit WowLocaleToCascLocaleBit[TOTAL_LOCALES];

/**
 * @brief 检查地区是否有效
 * @param locale 地区常量
 * @return true 如果地区有效，否则 false
 */
constexpr inline bool IsValidLocale(LocaleConstant locale)
{
    return locale < TOTAL_LOCALES && locale != LOCALE_none;
}

// ============================================================================
// 多语言字符串结构（紧凑布局）
// ============================================================================

#pragma pack(push, 1)

struct LocalizedString
{
    /**
     * @brief 通过地区索引获取字符串
     * @param locale 地区常量
     * @return 对应语言的字符串指针
     */
    constexpr char const* operator[](LocaleConstant locale) const
    {
        return Str[locale];
    }

    // 各地区对应的字符串数组
    std::array<char const*, TOTAL_LOCALES> Str;
};

#pragma pack(pop)

// ============================================================================
// 数学常量
// ============================================================================

#ifndef M_PI
#define M_PI 3.14159265358979323846  // 圆周率
#endif

#ifndef M_PI_4
#define M_PI_4 0.785398163397448309616  // π/4（45度）
#endif

// ============================================================================
// 查询操作的最大长度限制（32KB）
// ============================================================================

#define MAX_QUERY_LEN (32 * 1024)

END_NAMESPACE_COMMON

#endif // RENDU_COMMON_H
