#pragma once

/**
    @file adl_swap.h
    @brief Namespace-scope ADL swap() for noexcept operator.
*/

namespace contiguous
{
namespace internal
{
	// Used in noexcept specifiers that depend on ADL-enabled swap.
	namespace adl
	{
		using std::swap; // enable adl for swap

		template <class T>
		void swap_with_adl(T& a, T& b) noexcept(noexcept(swap(a, b)))
		{
			swap(a, b);
		}
	}
}
}