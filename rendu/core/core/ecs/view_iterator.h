/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_VIEW_ITERATOR_H_
#define RENDU_CORE_ECS_VIEW_ITERATOR_H_

#include <array>
#include "base/iterator.h"
#include "component_traits.h"
#include "entity_traits.h"

namespace rendu {

namespace internal {

template<typename... Args, typename Type, std::size_t N>
auto filter_as_tuple(const std::array<const Type *, N> &filter) noexcept {
  return std::apply([](const auto *...curr) {
    return std::make_tuple(static_cast<Args *>(const_cast<constness_as_t<Type, Args> *>(curr))...);
  }, filter);
}

template<typename Type, std::size_t N>
[[nodiscard]] auto none_of(const std::array<const Type *, N> &filter,
                           const typename Type::entity_type entity) noexcept {
  return std::apply([entity](const auto *...curr) {
    return (!curr->contains(entity) && ...);
  }, filter);
}

template<typename Type, std::size_t Get, std::size_t Exclude>
class view_iterator final {
  using iterator_type = typename Type::const_iterator;

  [[nodiscard]] bool valid() const noexcept {
    return ((Get != 0u) || (*it != tombstone))
        && std::apply([entity = *it](const auto *...curr) { return (curr->contains(entity) && ...); }, pools)
        && none_of(filter, *it);
  }

 public:
  using value_type = typename iterator_type::value_type;
  using pointer = typename iterator_type::pointer;
  using reference = typename iterator_type::reference;
  using difference_type = typename iterator_type::difference_type;
  using iterator_category = std::forward_iterator_tag;

  constexpr view_iterator() noexcept
      : it{},
        last{},
        pools{},
        filter{} {}

  view_iterator(iterator_type curr,
                iterator_type to,
                std::array<const Type *, Get> value,
                std::array<const Type *, Exclude> excl) noexcept
      : it{curr},
        last{to},
        pools{value},
        filter{excl} {
    while (it != last && !valid()) {
      ++it;
    }
  }

  view_iterator &operator++() noexcept {
    while (++it != last && !valid()) {}
    return *this;
  }

  view_iterator operator++(int) noexcept {
    view_iterator orig = *this;
    return ++(*this), orig;
  }

  [[nodiscard]] pointer operator->() const noexcept {
    return &*it;
  }

  [[nodiscard]] reference operator*() const noexcept {
    return *operator->();
  }

  template<typename LhsType, auto... LhsArgs, typename RhsType, auto... RhsArgs>
  friend constexpr bool operator==(const view_iterator<LhsType, LhsArgs...> &,
                                   const view_iterator<RhsType, RhsArgs...> &) noexcept;

 private:
  iterator_type it;
  iterator_type last;
  std::array<const Type *, Get> pools;
  std::array<const Type *, Exclude> filter;
};

template<typename LhsType, auto... LhsArgs, typename RhsType, auto... RhsArgs>
[[nodiscard]] constexpr bool operator==(const view_iterator<LhsType, LhsArgs...> &lhs,
                                        const view_iterator<RhsType, RhsArgs...> &rhs) noexcept {
  return lhs.it == rhs.it;
}

template<typename LhsType, auto... LhsArgs, typename RhsType, auto... RhsArgs>
[[nodiscard]] constexpr bool operator!=(const view_iterator<LhsType, LhsArgs...> &lhs,
                                        const view_iterator<RhsType, RhsArgs...> &rhs) noexcept {
  return !(lhs == rhs);
}

} //namespace internal

} //namespace rendu

#endif //RENDU_CORE_ECS_VIEW_ITERATOR_H_
