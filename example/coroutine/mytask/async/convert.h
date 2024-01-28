/*
* Created by boil on 2024/1/17.
*/

#ifndef RENDU_CONVERT_H
#define RENDU_CONVERT_H

#include <sstream>
#include <type_traits>

class Convert {
public:
  template<class T>
    requires std::is_pointer_v<T>
  static std::string ToString(T &&value) {
    std::ostringstream oss;
    oss << &value;
    return oss.str();
  }
};


#endif//RENDU_CONVERT_H
