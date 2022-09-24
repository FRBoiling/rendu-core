/*
* Created by boil on 2022/9/25.
*/

#ifndef RENDU_ITERATOR_PAIR_UTILS_H_
#define RENDU_ITERATOR_PAIR_UTILS_H_

#include "iterator_pair.h"

namespace rendu{
  template<typename iterator>
  constexpr IteratorPair<iterator> MakeIteratorPair(iterator first, iterator second) {
    return {first, second};
  }

  template<typename iterator>
  constexpr IteratorPair<iterator> MakeIteratorPair(std::pair<iterator, iterator> iterators) {
    return iterators;
  }

  template<class M>
  inline auto MapEqualRange(M &map, typename M::key_type const &key) -> IteratorPair<decltype(map.begin())> {
    return {map.equal_range(key)};
  }
}


#endif //RENDU_ITERATOR_PAIR_UTILS_H_
