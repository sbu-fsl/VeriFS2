#pragma once

/**
    @file wrapped_pair_compare.h
    @brief Compare for any combination of value_type / impl_value_type /
   key_type.
*/

namespace contiguous
{
namespace internal
{
	namespace wrapped_pair_compare_detail
	{
		/* Fetch key_type or T that may be comparable to key_type. */
		template <class Compare,
		          class key_type,
		          class value_type,
		          class impl_value_type,
		          class T>
		struct fetch_key
		{
			/* Fetch T that may be comparable to key_type,
			   fetch it as long as Compare::is_transparent */
			template <
			    class Dummy,
			    class = std::enable_if_t<
			        internal::check_is_transparent<Compare, Dummy>::value>>
			const T& fetch(const T& x) const
			{
				return x;
			}
		};

		/* Fetch the regular key_type */
		template <class Compare,
		          class key_type,
		          class value_type,
		          class impl_value_type>
		struct fetch_key<Compare,
		                 key_type,
		                 value_type,
		                 impl_value_type,
		                 std::remove_const_t<key_type>> // Don't care if
		                                                // key_type is
		                                                // const_qualified.
		{
			template <class U>
			const key_type& fetch(const key_type& x) const
			{
				return x;
			}
		};
		/* Fetch the key_type from the value_type */
		template <class Compare,
		          class key_type,
		          class value_type,
		          class impl_value_type>
		struct fetch_key<Compare,
		                 key_type,
		                 value_type,
		                 impl_value_type,
		                 value_type>
		{
			template <class U>
			const key_type& fetch(const value_type& x) const
			{
				return x.first;
			}
		};

		/* Fetch the key_type from the impl_value_type */
		template <class Compare,
		          class key_type,
		          class value_type,
		          class impl_value_type>
		struct fetch_key<Compare,
		                 key_type,
		                 value_type,
		                 impl_value_type,
		                 impl_value_type>
		{
			template <class U>
			const key_type& fetch(const impl_value_type& x) const
			{
				return x.get().first;
			}
		};
	}

	//! Compares various combinations of @c wrapped_pair<K,M>,
	//! @c pair<K,M>, and @c Key.
	template <class Key, class Mapped, class Compare>
	class wrapped_pair_compare
	{
	protected:
		using key_type = Key;
		using impl_value_type = wrapped_pair<Key, Mapped>;
		using value_type = std::pair<Key, Mapped>;

		Compare comp;

	public:
		using result_type = bool;
		using first_argument_type = value_type;
		using second_argument_type = value_type;

		wrapped_pair_compare(Compare c) : comp(c)
		{
		}

		//! Template compare - fetch the key from x and y, and compare them.
		template <class Left, class Right>
		bool operator()(const Left& x, const Right& y) const
		{
			return comp(fetch_key<Left>().template fetch<Left>(x),
			            fetch_key<Right>().template fetch<Right>(y));
		}

		//! Return a copy of the key comparator.
		Compare key_comp() const
		{
			return comp;
		}

		void swap(wrapped_pair_compare& other) noexcept(
		    noexcept(adl::swap_with_adl(comp, other.comp)))
		{
			adl::swap_with_adl(comp, other.comp);
		}

	private:
		/* Fetch key_type or T that may be comparable to key_type. */
		template <class T>
		using fetch_key = wrapped_pair_compare_detail::
		    fetch_key<Compare, key_type, value_type, impl_value_type, T>;
	};

	//! swap
	template <class Key, class Mapped, class Compare>
	void swap(wrapped_pair_compare<Key, Mapped, Compare>& x,
	          wrapped_pair_compare<Key, Mapped, Compare>&
	              y) noexcept(noexcept(x.swap(y)))
	{
		x.swap(y);
	}
}
}