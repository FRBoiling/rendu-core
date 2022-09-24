/*
* Created by boil on 2022/9/25.
*/

#ifndef RENDU_ITERATOR_H_
#define RENDU_ITERATOR_H_

#include <iterator>
#include "iterator_pair.h"
#include "enum_utils.h"

namespace rendu{
  template<typename Enum>
  class Iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Enum;
    using pointer = Enum *;
    using reference = Enum &;
    using difference_type = std::ptrdiff_t;

    Iterator() : _index(EnumUtils::Count<Enum>()) {}

    explicit Iterator(size_t index) : _index(index) {}

    bool operator==(const Iterator &other) const { return other._index == _index; }

    bool operator!=(const Iterator &other) const { return !operator==(other); }

    difference_type operator-(Iterator const &other) const { return _index - other._index; }

    bool operator<(const Iterator &other) const { return _index < other._index; }

    bool operator<=(const Iterator &other) const { return _index <= other._index; }

    bool operator>(const Iterator &other) const { return _index > other._index; }

    bool operator>=(const Iterator &other) const { return _index >= other._index; }

    value_type operator[](difference_type d) const { return FromIndex<Enum>(_index + d); }

    value_type operator*() const { return operator[](0); }

    Iterator &operator+=(difference_type d) {
      _index += d;
      return *this;
    }

    Iterator &operator++() { return operator+=(1); }

    Iterator operator++(int) {
      Iterator i = *this;
      operator++();
      return i;
    }

    Iterator operator+(difference_type d) const {
      Iterator i = *this;
      i += d;
      return i;
    }

    Iterator &operator-=(difference_type d) {
      _index -= d;
      return *this;
    }

    Iterator &operator--() { return operator-=(1); }

    Iterator operator--(int) {
      Iterator i = *this;
      operator--();
      return i;
    }

    Iterator operator-(difference_type d) const {
      Iterator i = *this;
      i -= d;
      return i;
    }

  private:
    difference_type _index;
  };

  template<typename Enum>
  static Iterator<Enum> Begin() { return Iterator<Enum>(0); }

  template<typename Enum>
  static Iterator<Enum> End() { return Iterator<Enum>(); }

  template<typename Enum>
  static IteratorPair<Iterator<Enum>> Iterate() { return {Begin<Enum>(), End<Enum>()}; }

} //namespace rendu
#endif //RENDU_ITERATOR_H_
