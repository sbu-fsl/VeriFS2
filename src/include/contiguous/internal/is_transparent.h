#pragma once

/**
    @file is_transparent.h
    @brief Check whether a class has is_transparent typedef using SFINAE.
*/

namespace contiguous
{
namespace internal
{
	/*
	template<class T, class K, class U = void>
	struct has_is_transparent : public std::integral_constant<bool,false>
	{};

	template <class T, class K>
	struct has_is_transparent<T, K, typename T::is_transparent>
	    : public std::integral_constant<bool,true>
	{
	};
	*/

	// from libc++, license MIT
	template <class Tp1, class Tp2 = void>
	struct check_is_transparent
	{
	private:
		struct two
		{
			char lx;
			char lxx;
		};
		template <class Up>
		static two test(...);
		template <class Up>
		static char test(typename Up::is_transparent* = 0);

	public:
		static const bool value = sizeof(test<Tp1>(0)) == 1;
	};
}
}
