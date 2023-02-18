/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_STORAGE_ITERATOR_H_
#define RENDU_CORE_ECS_STORAGE_ITERATOR_H_

#include "base/iterator.h"
#include "basic_sparse_set.h"

namespace rendu {

namespace internal {

template<typename Container, typename Size>
class storage_iterator final {
  friend storage_iterator<const Container, Size>;

  using container_type = std::remove_const_t<Container>;
  using alloc_traits = std::allocator_traits<typename container_type::allocator_type>;

  using iterator_traits = std::iterator_traits<std::conditional_t<
      std::is_const_v<Container>,
      typename alloc_traits::template rebind_traits<typename std::pointer_traits<typename container_type::value_type>::element_type>::const_pointer,
      typename alloc_traits::template rebind_traits<typename std::pointer_traits<typename container_type::value_type>::element_type>::pointer>>;

 public:
  using value_type = typename iterator_traits::value_type;
  using pointer = typename iterator_traits::pointer;
  using reference = typename iterator_traits::reference;
  using difference_type = typename iterator_traits::difference_type;
  using iterator_category = std::random_access_iterator_tag;

  constexpr storage_iterator() noexcept = default;

  constexpr storage_iterator(Container *ref, const difference_type idx) noexcept
      : payload{ref},
        offset{idx} {}

  template<bool Const = std::is_const_v<Container>, typename = std::enable_if_t<Const>>
  constexpr
  storage_iterator(const storage_iterator<std::remove_const_t<Container>, Size
  > &other) noexcept
      : storage_iterator{other.payload, other.offset} {}

  constexpr storage_iterator &operator++() noexcept {
    return --offset, *this;
  }

  constexpr storage_iterator operator++(int) noexcept {
    storage_iterator orig = *this;
    return ++(*this), orig;
  }

  constexpr storage_iterator &operator--() noexcept {
    return ++offset, *this;
  }

  constexpr storage_iterator operator--(int) noexcept {
    storage_iterator orig = *this;
    return operator--(), orig;
  }

  constexpr storage_iterator &operator+=(const difference_type value) noexcept {
    offset -= value;
    return *this;
  }

  constexpr storage_iterator operator+(const difference_type value) const noexcept {
    storage_iterator copy = *this;
    return (copy += value);
  }

  constexpr storage_iterator &operator-=(const difference_type value) noexcept {
    return (*this += -value);
  }

  constexpr storage_iterator operator-(const difference_type value) const noexcept {
    return (*this + -value);
  }

  [[nodiscard]] constexpr reference operator[](const difference_type value) const noexcept {
    const auto pos = index() - value;
    return (*payload)[pos / Size::value][fast_mod(pos, Size::value)];
  }

  [[nodiscard]] constexpr pointer operator->() const noexcept {
    const auto pos = index();
    return (*payload)[pos / Size::value] + fast_mod(pos, Size::value);
  }

  [[nodiscard]] constexpr reference operator*() const noexcept {
    return *operator->();
  }

  [[nodiscard]] constexpr difference_type index() const noexcept {
    return offset - 1;
  }

 private:
  Container *payload;
  difference_type offset;
};

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr std::ptrdiff_t operator-(const storage_iterator<Lhs...> &lhs,
                                                 const storage_iterator<Rhs...> &rhs) noexcept {
  return rhs.index() - lhs.index();
}

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator==(const storage_iterator<Lhs...> &lhs,
                                        const storage_iterator<Rhs...> &rhs) noexcept {
  return lhs.index() == rhs.index();
}

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator!=(const storage_iterator<Lhs...> &lhs,
                                        const storage_iterator<Rhs...> &rhs) noexcept {
  return !(lhs == rhs);
}

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator<(const storage_iterator<Lhs...> &lhs,
                                       const storage_iterator<Rhs...> &rhs) noexcept {
  return lhs.index() > rhs.index();
}

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator>(const storage_iterator<Lhs...> &lhs,
                                       const storage_iterator<Rhs...> &rhs) noexcept {
  return lhs.index() < rhs.index();
}

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator<=(const storage_iterator<Lhs...> &lhs,
                                        const storage_iterator<Rhs...> &rhs) noexcept {
  return !(lhs > rhs);
}

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator>=(const storage_iterator<Lhs...> &lhs,
                                        const storage_iterator<Rhs...> &rhs) noexcept {
  return !(lhs < rhs);
}

} //namespace internal

} //namespace rendu


#endif //RENDU_CORE_ECS_STORAGE_ITERATOR_H_
