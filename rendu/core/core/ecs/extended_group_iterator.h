/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_EXTENDED_GROUP_ITERATOR_H_
#define RENDU_CORE_ECS_EXTENDED_GROUP_ITERATOR_H_

#include "base/iterator.h"

namespace rendu {

namespace internal {

template<typename, typename, typename>
class extended_group_iterator;

template<typename It, typename... Owned, typename... Get>
class extended_group_iterator<It, owned_t<Owned...>, get_t<Get...>> {
  template<typename Type>
  auto index_to_element([[maybe_unused]] Type &cpool) const {
    if constexpr (Type::traits_type::page_size == 0u) {
      return std::make_tuple();
    } else {
      return std::forward_as_tuple(cpool.rbegin()[it.index()]);
    }
  }

 public:
  using iterator_type = It;
  using difference_type = std::ptrdiff_t;
  using value_type = decltype(std::tuple_cat(std::make_tuple(*std::declval<It>()),
                                             std::declval<Owned>().get_as_tuple({})...,
                                             std::declval<Get>().get_as_tuple({})...));
  using pointer = input_iterator_pointer<value_type>;
  using reference = value_type;
  using iterator_category = std::input_iterator_tag;

  constexpr extended_group_iterator()
      : it{},
        pools{} {}

  extended_group_iterator(It from, const std::tuple<Owned *..., Get *...> &cpools)
      : it{from},
        pools{cpools} {}

  extended_group_iterator &operator++() noexcept {
    return ++it, *this;
  }

  extended_group_iterator operator++(int) noexcept {
    extended_group_iterator orig = *this;
    return ++(*this), orig;
  }

  [[nodiscard]] reference operator*() const noexcept {
    return std::tuple_cat(std::make_tuple(*it),
                          index_to_element(*std::get<Owned *>(pools))...,
                          std::get<Get *>(pools)->get_as_tuple(*it)...);
  }

  [[nodiscard]] pointer operator->() const noexcept {
    return operator*();
  }

  [[nodiscard]] constexpr iterator_type base() const noexcept {
    return it;
  }

  template<typename... Lhs, typename... Rhs>
  friend constexpr bool operator==(const extended_group_iterator<Lhs...> &,
                                   const extended_group_iterator<Rhs...> &) noexcept;

 private:
  It it;
  std::tuple<Owned *..., Get *...> pools;
};

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator==(const extended_group_iterator<Lhs...> &lhs,
                                        const extended_group_iterator<Rhs...> &rhs) noexcept {
  return lhs.it == rhs.it;
}

template<typename... Lhs, typename... Rhs>
[[nodiscard]] constexpr bool operator!=(const extended_group_iterator<Lhs...> &lhs,
                                        const extended_group_iterator<Rhs...> &rhs) noexcept {
  return !(lhs == rhs);
}

} // namespace internal

} // namespace rendu
#endif //RENDU_CORE_ECS_EXTENDED_GROUP_ITERATOR_H_
