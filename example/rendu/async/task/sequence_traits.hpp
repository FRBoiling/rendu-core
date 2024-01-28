///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef RENDU_ASYNC_SEQUENCE_TRAITS_HPP
#define RENDU_ASYNC_SEQUENCE_TRAITS_HPP

#include <type_traits>

ASYNC_NAMESPACE_BEGIN
{
	template<typename SEQUENCE>
	struct sequence_traits
	{
		using value_type = SEQUENCE;
		using difference_type = std::make_signed_t<SEQUENCE>;
		using size_type = std::make_unsigned_t<SEQUENCE>;

		static constexpr value_type initial_sequence = static_cast<value_type>(-1);

		static constexpr difference_type difference(value_type a, value_type b)
		{
			return static_cast<difference_type>(a - b);
		}

		static constexpr bool precedes(value_type a, value_type b)
		{
			return difference(a, b) < 0;
		}
	};
}

#endif
