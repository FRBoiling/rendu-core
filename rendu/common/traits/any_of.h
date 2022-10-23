#ifndef RENDU_TRAITS_ANYOF_H_
#define RENDU_TRAITS_ANYOF_H_

#include <type_traits>
#include <concepts>

namespace rendu::traits
{
	template <typename T, typename... Types>
	struct IsAnyOf : std::disjunction<std::conjunction<std::is_same<T, Types>, std::is_same<Types, T>>...> {};

	template <typename T, typename... Types>
	concept AnyOf = (std::same_as<T, Types> || ...);
}

#endif // RENDU_TRAITS_ANYOF_H_