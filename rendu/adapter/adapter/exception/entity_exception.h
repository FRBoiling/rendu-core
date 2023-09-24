/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_ENTITY_EXCEPTION_H
#define RENDU_ENTITY_EXCEPTION_H

#include "define.h"

RD_NAMESPACE_BEGIN

    class EntityException: public std::exception {
    public:
      explicit EntityException(const std::string &msg);
      ~EntityException() override;
    public:
      [[nodiscard]] const char *what() const noexcept override;
    };

RD_NAMESPACE_END

#endif //RENDU_ENTITY_EXCEPTION_H
