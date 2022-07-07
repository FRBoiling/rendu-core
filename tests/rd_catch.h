// This file is part of the rendu Project. See AUTHORS file for Copyright information
// Created by Boil on 2022/7/7.
//

#ifndef RENDU_TESTS_RD_CATCH_H_
#define RENDU_TESTS_RD_CATCH_H_

#include "optional.h"
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

#endif//RENDU_TESTS_RD_CATCH_H_
