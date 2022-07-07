// This file is part of the rendu-core Project. See AUTHORS file for Copyright information
// Created by Administrator on 2022/7/7.
//

#ifndef RENDU_SRC_COMMON_UTILS_OPTIONAL_H_
#define RENDU_SRC_COMMON_UTILS_OPTIONAL_H_

#include <optional>

//! Optional helper class to wrap optional values within.
template <class T>
using Optional = std::optional<T>;

#endif//RENDU_SRC_COMMON_UTILS_OPTIONAL_H_
