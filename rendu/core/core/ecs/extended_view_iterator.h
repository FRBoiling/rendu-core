/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_EXTENDED_VIEW_ITERATOR_H_
#define RENDU_CORE_ECS_EXTENDED_VIEW_ITERATOR_H_

#include "view_iterator.h"

namespace rendu {

namespace internal {

template<typename It, typename... Type>
struct extended_view_iterator final {
  using iterator_type = It;
  using difference_type = std::ptrdiff_t;
  using value_type = decltype(std::tuple_cat(std::make_tuple(*std::declval<It>()),
                                             std::declval<Type>().get_as_tuple({})...));
  using pointer = input_iterator_pointer<value_type>;
  using reference = value_type;
  using iterator_category = std::input_iterator_tag;

  constexpr extended_view_iterator()
      : it{},
        pools{} {}

  extended_view_iterator(It from, std::tuple<Type *...> value)
      : it{from},
        pools{value} {}

  extended_view_iterator &operator++() noexcept {
    return ++it, *this;
  }

  extended_view_iterator operator++(int) noexcept {
    extended_view_iterator orig = *this;
    return ++(*this), orig;
  }

  [[nodiscard]] reference operator*() const noexcept {
    return std::apply([entity = *it](auto *...curr) {
      return std::tuple_cat(std::make_tuple(entity), curr->get_as_tuple(entity)...);
    }, pools);
  }

  [[nodiscard]] pointer operator->() const noexcept {
    return operator*();
  }

  [[nodiscard]] constexpr iterator_type base() const noexcept {
    return it;
  }

  template<typename... Lhs, typename... Rhs>
  friend bool constexpr operator==(const extended_view_iterator<Lhs...> &,
                                   const extended_view_iterator<Rhs...> &) noexcept;

 private:
  It it;
  std::tuple<Type *...> pools;
};

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator==(const extended_view_iterator<Lhs...> &lhs,
                                        const extended_view_iterator<Rhs...> &rhs) noexcept {
  return lhs.it == rhs.it;
}

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator!=(const extended_view_iterator<Lhs...> &lhs,
                                        const extended_view_iterator<Rhs...> &rhs) noexcept {
  return !(lhs == rhs);
}

} // namespace internal

} // namespace rendu

#endif //RENDU_CORE_ECS_EXTENDED_VIEW_ITERATOR_H_
