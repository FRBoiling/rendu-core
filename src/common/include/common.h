//
// Created by FRee2 on 2025/10/18.
//

#ifndef RENDU_COMMON_H
#define RENDU_COMMON_H

#include "common/define.h"
#include <array>
#include <string>

using namespace std::string_literals;
using namespace std::string_view_literals;

#define STRINGIZE(a) #a

BEGIN_NAMESPACE_COMMON
    enum TimeConstants
    {
        MINUTE = 60,
        HOUR = MINUTE * 60,
        DAY = HOUR * 24,
        WEEK = DAY * 7,
        MONTH = DAY * 30,
        YEAR = MONTH * 12,
        IN_MILLISECONDS = 1000
    };

    enum AccountTypes
    {
        SEC_PLAYER = 0,
        SEC_MODERATOR = 1,
        SEC_GAMEMASTER = 2,
        SEC_ADMINISTRATOR = 3,
        SEC_CONSOLE = 4 // must be always last in list, accounts must have less security level always also
    };

    enum LocaleConstant : uint8
    {
        LOCALE_enUS = 0,
        LOCALE_koKR = 1,
        LOCALE_frFR = 2,
        LOCALE_deDE = 3,
        LOCALE_zhCN = 4,
        LOCALE_zhTW = 5,
        LOCALE_esES = 6,
        LOCALE_esMX = 7,
        LOCALE_ruRU = 8,
        LOCALE_none = 9,
        LOCALE_ptBR = 10,
        LOCALE_itIT = 11,

        TOTAL_LOCALES
    };

    const uint8 OLD_TOTAL_LOCALES = 9; /// @todo convert in simple system
#define DEFAULT_LOCALE LOCALE_enUS

    enum class CascLocaleBit : uint8
    {
        None = 0,
        enUS = 1,
        koKR = 2,
        Reserved = 3,
        frFR = 4,
        deDE = 5,
        zhCN = 6,
        esES = 7,
        zhTW = 8,
        enGB = 9,
        enCN = 10,
        enTW = 11,
        esMX = 12,
        ruRU = 13,
        ptBR = 14,
        itIT = 15,
        ptPT = 16
    };

    RC_COMMON_API extern char const* localeNames[TOTAL_LOCALES];

    RC_COMMON_API LocaleConstant GetLocaleByName(std::string_view name);

    RC_COMMON_API extern CascLocaleBit WowLocaleToCascLocaleBit[TOTAL_LOCALES];

    constexpr inline bool IsValidLocale(LocaleConstant locale)
    {
        return locale < TOTAL_LOCALES && locale != LOCALE_none;
    }

#pragma pack(push, 1)

    struct LocalizedString
    {
        constexpr char const* operator[](LocaleConstant locale) const
        {
            return Str[locale];
        }

        std::array<char const*, TOTAL_LOCALES> Str;
    };

#pragma pack(pop)


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_4
#define M_PI_4 0.785398163397448309616
#endif

    // 定义查询操作的最大长度限制（32KB）
#define MAX_QUERY_LEN 32*1024

END_NAMESPACE_COMMON

#endif // RENDU_COMMON_H
