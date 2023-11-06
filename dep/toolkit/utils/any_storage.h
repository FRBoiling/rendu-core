/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_ANY_STORAGE_H
#define RENDU_ANY_STORAGE_H

#include "any.h"
#include <unordered_map>

RD_NAMESPACE_BEGIN

// 用于保存一些外加属性
  class AnyStorage : public std::unordered_map<std::string, Any> {
  public:
    AnyStorage() = default;

    ~AnyStorage() = default;

    using Ptr = std::shared_ptr<AnyStorage>;
  };

RD_NAMESPACE_END

#endif //RENDU_ANY_STORAGE_H
