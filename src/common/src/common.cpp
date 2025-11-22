//
// Created by FRee2 on 2025/10/18.
//

#include "common.h"

BEGIN_NAMESPACE_COMMON
    char const* localeNames[TOTAL_LOCALES] =
    {
        "enUS",
        "koKR",
        "frFR",
        "deDE",
        "zhCN",
        "zhTW",
        "esES",
        "esMX",
        "ruRU",
        "none",
        "ptBR",
        "itIT"
    };

    LocaleConstant GetLocaleByName(std::string_view name)
    {
        for (uint32 i = 0; i < TOTAL_LOCALES; ++i)
            if (name == localeNames[i])
                return LocaleConstant(i);

        return TOTAL_LOCALES;
    }

    CascLocaleBit RCLocaleToCascLocaleBit[TOTAL_LOCALES] =
    {
        CascLocaleBit::enUS,
        CascLocaleBit::koKR,
        CascLocaleBit::frFR,
        CascLocaleBit::deDE,
        CascLocaleBit::zhCN,
        CascLocaleBit::zhTW,
        CascLocaleBit::esES,
        CascLocaleBit::esMX,
        CascLocaleBit::ruRU,
        CascLocaleBit::None,
        CascLocaleBit::ptBR,
        CascLocaleBit::itIT
    };

END_NAMESPACE_COMMON
