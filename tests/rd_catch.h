// This file is part of the rendu Project. See AUTHORS file for Copyright information
// Created by Boil on 2022/7/7.
//

#ifndef RENDU_RD_CATCH_H
#define RENDU_RD_CATCH_H

#include "Optional.h"
#include <iostream>
#include <typeinfo>

template <typename T>
std::ostream& operator<<(std::ostream& os, Optional<T> const& value)
{
  os << "Opt";
  if (value)
    os << " { " << *value << " }";
  else
    os << " (<empty>)";
  return os;
}

inline std::ostream& operator<<(std::ostream& os, std::nullopt_t)
{
  os << "<empty>";
  return os;
}

#include "catch2/catch.hpp"

#endif

#endif//RENDU_RD_CATCH_H
