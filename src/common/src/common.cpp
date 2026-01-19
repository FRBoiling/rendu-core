//
// Created by FRee2 on 2025/10/18.
//

#include "common.h"

BEGIN_NAMESPACE_COMMON

// ============================================================================
// 地区名称数组
// ============================================================================

char const* localeNames[TOTAL_LOCALES] =
{
    "enUS",  // 英语（美国）
    "koKR",  // 韩语（韩国）
    "frFR",  // 法语（法国）
    "deDE",  // 德语（德国）
    "zhCN",  // 中文（简体）
    "zhTW",  // 中文（繁体）
    "esES",  // 西班牙语（西班牙）
    "esMX",  // 西班牙语（墨西哥）
    "ruRU",  // 俄语（俄罗斯）
    "none",  // 未指定
    "ptBR",  // 葡萄牙语（巴西）
    "itIT"   // 意大利语（意大利）
};

// ============================================================================
// 地区名称到地区常量的转换函数
// ============================================================================

LocaleConstant GetLocaleByName(std::string_view name)
{
    for (uint32 i = 0; i < TOTAL_LOCALES; ++i)
    {
        if (name == localeNames[i])
        {
            return LocaleConstant(i);
        }
    }

    return TOTAL_LOCALES;  // 未找到
}

// ============================================================================
// WoW 地区到 CASC 地区位的映射表
// ============================================================================

CascLocaleBit WowLocaleToCascLocaleBit[TOTAL_LOCALES] =
{
    CascLocaleBit::enUS,  // enUS
    CascLocaleBit::koKR,  // koKR
    CascLocaleBit::frFR,  // frFR
    CascLocaleBit::deDE,  // deDE
    CascLocaleBit::zhCN,  // zhCN
    CascLocaleBit::zhTW,  // zhTW
    CascLocaleBit::esES,  // esES
    CascLocaleBit::esMX,  // esMX
    CascLocaleBit::ruRU,  // ruRU
    CascLocaleBit::None,  // none
    CascLocaleBit::ptBR,  // ptBR
    CascLocaleBit::itIT   // itIT
};

END_NAMESPACE_COMMON
