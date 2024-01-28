/*
* Created by boil on 2023/12/28.
*/

#ifndef RENDU_UNIT_H
#define RENDU_UNIT_H

// Unit plays the role of a simplest type in case we couldn't
// use void directly.
//
// User shouldn't use this directly.
struct Unit {
  constexpr bool operator==(const Unit&) const { return true; }
  constexpr bool operator!=(const Unit&) const { return false; }
};


#endif//RENDU_UNIT_H
