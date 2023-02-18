/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_REGISTRY_STORAGE_ITERATOR_H_
#define RENDU_CORE_ECS_REGISTRY_STORAGE_ITERATOR_H_

#include "base/iterator.h"
#include "fwd.h"

namespace rendu {

namespace internal {

template<typename It>
class registry_storage_iterator final {
  template<typename Other>
  friend
  class registry_storage_iterator;
  using mapped_type = std::remove_reference_t<decltype(std::declval<It>()->second)>;

 public:
  using value_type = std::pair<id_type, constness_as_t<typename mapped_type::element_type, mapped_type> &>;
  using pointer = input_iterator_pointer<value_type>;
  using reference = value_type;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;

  constexpr registry_storage_iterator() noexcept
      : it{} {}

  constexpr registry_storage_iterator(It iter) noexcept
      : it{iter} {}

  template<typename Other, typename =
  std::enable_if_t<!std::is_same_v < It, Other> &&std::is_constructible_v<It, Other>>
  >
  constexpr registry_storage_iterator(const registry_storage_iterator<Other> &other) noexcept
      : registry_storage_iterator{other.it} {}

  constexpr registry_storage_iterator &operator++() noexcept {
    return ++it, *this;
  }

  constexpr registry_storage_iterator operator++(int) noexcept {
    registry_storage_iterator orig = *this;
    return ++(*this), orig;
  }

  constexpr registry_storage_iterator &operator--() noexcept {
    return --it, *this;
  }

  constexpr registry_storage_iterator operator--(int) noexcept {
    registry_storage_iterator orig = *this;
    return operator--(), orig;
  }

  constexpr registry_storage_iterator &operator+=(const difference_type value) noexcept {
    it += value;
    return *this;
  }

  constexpr registry_storage_iterator operator+(const difference_type value) const noexcept {
    registry_storage_iterator copy = *this;
    return (copy += value);
  }

  constexpr registry_storage_iterator &operator-=(const difference_type value) noexcept {
    return (*this += -value);
  }

  constexpr registry_storage_iterator operator-(const difference_type value) const noexcept {
    return (*this + -value);
  }

  [[nodiscard]] constexpr reference operator[](const difference_type value) const noexcept {
    return {it[value].first, *it[value].second};
  }

  [[nodiscard]] constexpr reference operator*() const noexcept {
    return {it->first, *it->second};
  }

  [[nodiscard]] constexpr pointer operator->() const noexcept {
    return operator*();
  }

  template<typename Lhs, typename Rhs>
  friend constexpr std::ptrdiff_t operator-(const registry_storage_iterator<Lhs> &,
                                            const registry_storage_iterator<Rhs> &) noexcept;

  template<typename Lhs, typename Rhs>
  friend constexpr bool operator==(const registry_storage_iterator<Lhs> &,
                                   const registry_storage_iterator<Rhs> &) noexcept;

  template<typename Lhs, typename Rhs>
  friend constexpr bool operator<(const registry_storage_iterator<Lhs> &,
                                  const registry_storage_iterator<Rhs> &) noexcept;

 private:
  It it;
};
template<typename Lhs, typename Rhs>
[[nodiscard]] constexpr std::ptrdiff_t operator-(const registry_storage_iterator<Lhs> &lhs,
                                                 const registry_storage_iterator<Rhs> &rhs) noexcept {
  return lhs.it - rhs.it;
}

template<typename Lhs, typename Rhs>
[[nodiscard]] constexpr bool operator==(const registry_storage_iterator<Lhs> &lhs,
                                        const registry_storage_iterator<Rhs> &rhs) noexcept {
  return lhs.it == rhs.it;
}

template<typename Lhs, typename Rhs>
[[nodiscard]] constexpr bool operator!=(const registry_storage_iterator<Lhs> &lhs,
                                        const registry_storage_iterator<Rhs> &rhs) noexcept {
  return !(lhs == rhs);
}

template<typename Lhs, typename Rhs>
[[nodiscard]] constexpr bool operator<(const registry_storage_iterator<Lhs> &lhs,
                                       const registry_storage_iterator<Rhs> &rhs) noexcept {
  return lhs.it < rhs.it;
}

template<typename Lhs, typename Rhs>
[[nodiscard]] constexpr bool operator>(const registry_storage_iterator<Lhs> &lhs,
                                       const registry_storage_iterator<Rhs> &rhs) noexcept {
  return rhs < lhs;
}

template<typename Lhs, typename Rhs>
[[nodiscard]] constexpr bool operator<=(const registry_storage_iterator<Lhs> &lhs,
                                        const registry_storage_iterator<Rhs> &rhs) noexcept {
  return !(lhs > rhs);
}

template<typename Lhs, typename Rhs>
[[nodiscard]] constexpr bool operator>=(const registry_storage_iterator<Lhs> &lhs,
                                        const registry_storage_iterator<Rhs> &rhs) noexcept {
  return !(lhs < rhs);
}
} // namespace internal
} // namespace rendu
#endif //RENDU_CORE_ECS_REGISTRY_STORAGE_ITERATOR_H_
