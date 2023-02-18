/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_SPARSE_SET_ITERATOR_H_
#define RENDU_CORE_ECS_SPARSE_SET_ITERATOR_H_

#include "base/iterator.h"

namespace rendu {

/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */

namespace internal {

template<typename Container>
struct sparse_set_iterator final {
  using value_type = typename Container::value_type;
  using pointer = typename Container::const_pointer;
  using reference = typename Container::const_reference;
  using difference_type = typename Container::difference_type;
  using iterator_category = std::random_access_iterator_tag;

  constexpr sparse_set_iterator() noexcept
      : packed{},
        offset{} {}

  constexpr sparse_set_iterator(const Container &ref, const difference_type idx) noexcept
      : packed{std::addressof(ref)},
        offset{idx} {}

  constexpr sparse_set_iterator &operator++() noexcept {
    return --offset, *this;
  }

  constexpr sparse_set_iterator operator++(int) noexcept {
    sparse_set_iterator orig = *this;
    return ++(*this), orig;
  }

  constexpr sparse_set_iterator &operator--() noexcept {
    return ++offset, *this;
  }

  constexpr sparse_set_iterator operator--(int) noexcept {
    sparse_set_iterator orig = *this;
    return operator--(), orig;
  }

  constexpr sparse_set_iterator &operator+=(const difference_type value) noexcept {
    offset -= value;
    return *this;
  }

  constexpr sparse_set_iterator operator+(const difference_type value) const noexcept {
    sparse_set_iterator copy = *this;
    return (copy += value);
  }

  constexpr sparse_set_iterator &operator-=(const difference_type value) noexcept {
    return (*this += -value);
  }

  constexpr sparse_set_iterator operator-(const difference_type value) const noexcept {
    return (*this + -value);
  }

  [[nodiscard]] constexpr reference operator[](const difference_type value) const noexcept {
    return packed->data()[index() - value];
  }

  [[nodiscard]] constexpr pointer operator->() const noexcept {
    return packed->data() + index();
  }

  [[nodiscard]] constexpr reference operator*() const noexcept {
    return *operator->();
  }

  [[nodiscard]] constexpr pointer data() const noexcept {
    return packed ? packed->data() : nullptr;
  }

  [[nodiscard]] constexpr difference_type index() const noexcept {
    return offset - 1;
  }

 private:
  const Container *packed;
  difference_type offset;
};

template<typename Container>
[[nodiscard]] constexpr std::ptrdiff_t operator-(const sparse_set_iterator<Container> &lhs, const sparse_set_iterator<Container> &rhs) noexcept {
  return rhs.index() - lhs.index();
}

template<typename Container>
[[nodiscard]] constexpr bool operator==(const sparse_set_iterator<Container> &lhs, const sparse_set_iterator<Container> &rhs) noexcept {
  return lhs.index() == rhs.index();
}

template<typename Container>
[[nodiscard]] constexpr bool operator!=(const sparse_set_iterator<Container> &lhs, const sparse_set_iterator<Container> &rhs) noexcept {
  return !(lhs == rhs);
}

template<typename Container>
[[nodiscard]] constexpr bool operator<(const sparse_set_iterator<Container> &lhs, const sparse_set_iterator<Container> &rhs) noexcept {
  return lhs.index() > rhs.index();
}

template<typename Container>
[[nodiscard]] constexpr bool operator>(const sparse_set_iterator<Container> &lhs, const sparse_set_iterator<Container> &rhs) noexcept {
  return lhs.index() < rhs.index();
}

template<typename Container>
[[nodiscard]] constexpr bool operator<=(const sparse_set_iterator<Container> &lhs, const sparse_set_iterator<Container> &rhs) noexcept {
  return !(lhs > rhs);
}

template<typename Container>
[[nodiscard]] constexpr bool operator>=(const sparse_set_iterator<Container> &lhs, const sparse_set_iterator<Container> &rhs) noexcept {
  return !(lhs < rhs);
}

} // namespace internal


} // namespace rendu

#endif //RENDU_CORE_ECS_SPARSE_SET_ITERATOR_H_
