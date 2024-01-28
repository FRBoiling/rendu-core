///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef RENDU_ASYNC_INLINE_SCHEDULER_HPP
#define RENDU_ASYNC_INLINE_SCHEDULER_HPP

#include <coroutine>

ASYNC_NAMESPACE_BEGIN
{
	class inline_scheduler
	{
	public:

		inline_scheduler() noexcept = default;

		std::suspend_never schedule() const noexcept
		{
			return {};
		}
	};
}

#endif
