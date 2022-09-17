/*
* Created by boil on 2022/8/20.
*/

#ifndef RENDU_SFMT_RAND_H_
#define RENDU_SFMT_RAND_H_

#include "define.h"
#include <SFMT.h>
#include <new>

/*
 * C++ Wrapper for sfmt
 */
class SFMTRand {
public:
  SFMTRand();

  uint32 RandomUInt32(); // Output random bits
  void *operator new(size_t size, std::nothrow_t const &);

  void operator delete(void *ptr, std::nothrow_t const &);

  void *operator new(size_t size);

  void operator delete(void *ptr);

  void *operator new[](size_t size, std::nothrow_t const &);

  void operator delete[](void *ptr, std::nothrow_t const &);

  void *operator new[](size_t size);

  void operator delete[](void *ptr);

private:
  sfmt_t _state;
};

#endif // RENDU_SFMT_RAND_H_
