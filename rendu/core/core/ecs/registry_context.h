/*
* Created by boil on 2023/2/18.
*/

#ifndef RENDU_CORE_ECS_REGISTRY_CONTEXT_H_
#define RENDU_CORE_ECS_REGISTRY_CONTEXT_H_

#include "container/dense_map.h"

namespace rendu {

namespace internal {

template<typename Allocator>
class registry_context {
  using alloc_traits = std::allocator_traits<Allocator>;
  using allocator_type = typename alloc_traits::template rebind_alloc<std::pair<const id_type, basic_any<0u>>>;

 public:
  explicit registry_context(const allocator_type &allocator)
      : ctx{allocator} {}

  template<typename Type, typename... Args>
  Type &emplace_as(const id_type id, Args &&...args) {
    return any_cast<Type &>(ctx.try_emplace(id,
                                            std::in_place_type<Type>,
                                            std::forward<Args>(args)...).first->second);
  }

  template<typename Type, typename... Args>
  Type &emplace(Args &&...args) {
    return emplace_as<Type>(type_id<Type>().hash(), std::forward<Args>(args)...);
  }

  template<typename Type>
  Type &insert_or_assign(const id_type id, Type &&value) {
    return any_cast<std::remove_cv_t<std::remove_reference_t<Type >> &
    >(ctx.insert_or_assign(id, std::forward<Type>(value)).first->second);
  }

  template<typename Type>
  Type &insert_or_assign(Type &&value) {
    return insert_or_assign(type_id<Type>().hash(), std::forward<Type>(value));
  }

  template<typename Type>
  bool erase(const id_type id = type_id<Type>().hash()) {
    const auto it = ctx.find(id);
    return it != ctx.end() && it->second.type() == type_id<Type>() ? (ctx.erase(it), true) : false;
  }

  template<typename Type>
  [[nodiscard]] const Type &get(const id_type id = type_id<Type>().hash()) const {
    return any_cast<const Type &>(ctx.at(id));
  }

  template<typename Type>
  [[nodiscard]] Type &get(const id_type id = type_id<Type>().hash()) {
    return any_cast<Type &>(ctx.at(id));
  }

  template<typename Type>
  [[nodiscard]] const Type *find(const id_type id = type_id<Type>().hash()) const {
    const auto it = ctx.find(id);
    return it != ctx.cend() ? any_cast<const Type>(&it->second) : nullptr;
  }

  template<typename Type>
  [[nodiscard]] Type *find(const id_type id = type_id<Type>().hash()) {
    const auto it = ctx.find(id);
    return it != ctx.end() ? any_cast<Type>(&it->second) : nullptr;
  }

  template<typename Type>
  [[nodiscard]] bool contains(const id_type id = type_id<Type>().hash()) const {
    const auto it = ctx.find(id);
    return it != ctx.cend() && it->second.type() == type_id<Type>();
  }

 private:
  dense_map<id_type, basic_any<0u>, std::identity, std::equal_to<id_type>, allocator_type> ctx;
};

} // namespace internal

} // namespace rendu

#endif //RENDU_CORE_ECS_REGISTRY_CONTEXT_H_
