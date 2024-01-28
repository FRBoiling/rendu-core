/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_DETAIL_UNWRAP_REFERENCE_HPP
#define RENDU_ASYNC_DETAIL_UNWRAP_REFERENCE_HPP

#include <functional>

ASYNC_NAMESPACE_BEGIN
{
	namespace detail
	{
		template<typename T>
		struct unwrap_reference
		{
			using type = T;
		};

		template<typename T>
		struct unwrap_reference<std::reference_wrapper<T>>
		{
			using type = T;
		};

		template<typename T>
		using unwrap_reference_t = typename unwrap_reference<T>::type;
	}
}

#endif
