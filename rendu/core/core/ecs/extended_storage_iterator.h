/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_EXTENDED_STORAGE_ITERATOR_H_
#define RENDU_CORE_ECS_EXTENDED_STORAGE_ITERATOR_H_

#include "storage_iterator.h"

namespace rendu {
namespace internal {

template<typename It, typename... Other>
class extended_storage_iterator final  {
  template<typename Iter, typename... Args>
  friend class extended_storage_iterator;

 public:
  using iterator_type = It;
  using difference_type = std::ptrdiff_t;
  using value_type = decltype(std::tuple_cat(std::make_tuple(*std::declval<It>()), std::forward_as_tuple(*std::declval<Other>()...)));
  using pointer = input_iterator_pointer<value_type>;
  using reference = value_type;
  using iterator_category = std::input_iterator_tag;

  constexpr extended_storage_iterator()
      : it{} {}

  constexpr extended_storage_iterator(It base, Other... other)
      : it{base, other...} {}

  template<typename... Args, typename = std::enable_if_t<(!std::is_same_v<Other, Args> && ...) && (std::is_constructible_v<Other, Args> && ...)>>
  constexpr extended_storage_iterator(const extended_storage_iterator<It, Args...> &other)
      : it{other.it} {}

  constexpr extended_storage_iterator &operator++() noexcept {
    return ++std::get<It>(it), (++std::get<Other>(it), ...), *this;
  }

  constexpr extended_storage_iterator operator++(int) noexcept {
    extended_storage_iterator orig = *this;
    return ++(*this), orig;
  }

  [[nodiscard]] constexpr pointer operator->() const noexcept {
    return operator*();
  }

  [[nodiscard]] constexpr reference operator*() const noexcept {
    return {*std::get<It>(it), *std::get<Other>(it)...};
  }

  [[nodiscard]] constexpr iterator_type base() const noexcept {
    return std::get<It>(it);
  }

  template<typename... Lhs, typename... Rhs>
  friend constexpr bool operator==(const extended_storage_iterator<Lhs...> &, const extended_storage_iterator<Rhs...> &) noexcept;

 private:
  std::tuple<It, Other...> it;
};

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator==(const extended_storage_iterator<Lhs...> &lhs, const extended_storage_iterator<Rhs...> &rhs) noexcept {
  return std::get<0>(lhs.it) == std::get<0>(rhs.it);
}

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator!=(const extended_storage_iterator<Lhs...> &lhs, const extended_storage_iterator<Rhs...> &rhs) noexcept {
  return !(lhs == rhs);
}

} // namespace internal

}//namespace rendu

#endif //RENDU_CORE_ECS_EXTENDED_STORAGE_ITERATOR_H_
