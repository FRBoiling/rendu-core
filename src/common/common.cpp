// This file is part of the rendu-core Project. See AUTHORS file for Copyright information
// Created by Boil on 2022/7/6.
//

#include "common.h"
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

LocaleConstant GetLocaleByName(std::string const& name)
{
  for (uint32 i = 0; i < TOTAL_LOCALES; ++i)
    if (name == localeNames[i])
      return LocaleConstant(i);

  return TOTAL_LOCALES;
}

CascLocaleBit WowLocaleToCascLocaleBit[TOTAL_LOCALES] =
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