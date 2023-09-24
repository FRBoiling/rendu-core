/*
* Created by boil on 2023/9/23.
*/

#ifndef RENDU_ARGUMENT_EXCEPTION_H
#define RENDU_ARGUMENT_EXCEPTION_H

#include "define.h"
#include  <exception>

RD_NAMESPACE_BEGIN

    class ArgumentException : public std::exception {
    public:
      explicit ArgumentException(const std::string &msg);
      ~ArgumentException() override;
    public:
      [[nodiscard]] const char *what() const noexcept override;


    };

RD_NAMESPACE_END

#endif //RENDU_ARGUMENT_EXCEPTION_H
