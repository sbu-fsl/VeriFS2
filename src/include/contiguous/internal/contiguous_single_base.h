#pragma once

#include "contiguous_base.h"

/**
    @file contiguous_single_base.h
    @brief Common functionality of associative containers that support unique
   keys - @c contiguous_map and @c contiguous_set.
*/

namespace contiguous
{
/*!
    @brief Common functionality of associative containers that support unique
   keys - @c contiguous_map and @c contiguous_set.
*/
template <class Traits>
class contiguous_single_base : public contiguous_base<Traits>
{
private:
	using MyBase = contiguous_base<Traits>;

protected:
	using typename MyBase::impl_container_type;
	using typename MyBase::impl_value_type;

public:
	//! @name types
	/// @{

	using typename MyBase::key_type;
	using typename MyBase::value_type;
	using typename MyBase::allocator_type;

	using typename MyBase::key_compare;
	using typename MyBase::reference;
	using typename MyBase::const_reference;
	using typename MyBase::pointer;
	using typename MyBase::const_pointer;

	using typename MyBase::iterator;
	using typename MyBase::const_iterator;
	using typename MyBase::reverse_iterator;
	using typename MyBase::const_reverse_iterator;

	using typename MyBase::difference_type;
	using typename MyBase::size_type;

	/// @}
	//! @name construct/copy/destroy
	/// @{

	// empty

	//! Constructs an empty container.
	contiguous_single_base() noexcept(noexcept(key_compare()) &&
	                                  noexcept(allocator_type()))
	  : contiguous_single_base(key_compare(), allocator_type())
	{
	}

	//! Constructs an empty container using the comparison object and allocator.
	explicit contiguous_single_base(
	    const key_compare& comp,
	    const allocator_type& alloc = allocator_type())
	  : MyBase(comp, alloc)
	{
	}

	//! Constructs an empty container using the allocator.
	explicit contiguous_single_base(const allocator_type& alloc) : MyBase(alloc)
	{
	}

	// range

	//! Constructs an empty container using the comparison object and allocator,
	//! and inserts elements from the range [first,last).
	template <class InputIterator>
	contiguous_single_base(InputIterator first,
	                       InputIterator last,
	                       const key_compare& comp = key_compare(),
	                       const allocator_type& alloc = allocator_type())
	  : MyBase(comp, alloc)
	{
		insert(first, last);
	}

	//! Constructs using the allocator, and inserts elements from the range
	//! [first,last).
	template <class InputIterator>
	contiguous_single_base(InputIterator first,
	                       InputIterator last,
	                       const allocator_type& alloc)
	  : contiguous_single_base(first, last, key_compare(), alloc)
	{
	}

	// copy

	//! Constructs by copying other.
	contiguous_single_base(const contiguous_single_base& other) : MyBase(other)
	{
	}

	//! Constructs by copying other, using a different allocator.
	contiguous_single_base(const contiguous_single_base& other,
	                       const allocator_type& alloc)
	  : MyBase(other, alloc)
	{
	}

	// move

	//! Constructs by moving other.
	contiguous_single_base(contiguous_single_base&& other) noexcept(
	    noexcept(MyBase(std::move(other))))
	  : MyBase(std::move(other))
	{
	}

	//! Constructs by moving other, using a different allocator.
	contiguous_single_base(contiguous_single_base&& other,
	                       const allocator_type& alloc)
	  : MyBase(std::move(other), alloc)
	{
	}

	// initializer list

	//! Constructs an empty container using the comparison object and allocator,
	//! and inserts elements from the initializer list.
	contiguous_single_base(std::initializer_list<value_type> il,
	                       const key_compare& comp = key_compare(),
	                       const allocator_type& alloc = allocator_type())
	  : MyBase(comp, alloc)
	{
		insert(il.begin(), il.end());
	}

	//! Constructs using the allocator and inserts elements from the
	//! initializer list.
	contiguous_single_base(std::initializer_list<value_type> il,
	                       const allocator_type& alloc)
	  : contiguous_single_base(il, key_compare(), alloc)
	{
	}

	//! Destructor.
	~contiguous_single_base()
	{
	}

	// copy

	//! Copy-assigns other.
	contiguous_single_base& operator=(const contiguous_single_base& other)
	{
		MyBase::operator=(other);
		return *this;
	}
	// move

	//! Move-assigns other.
	contiguous_single_base& operator=(contiguous_single_base&& other) noexcept(
	    noexcept(other.MyBase::operator=(std::move(other))))
	{
		MyBase::operator=(std::move(other));
		return *this;
	}

	// initializer list

	//! Replaces the elements with the elements from the initializer list.
	contiguous_single_base& operator=(std::initializer_list<value_type> il)
	{
		this->clear();
		insert(il.begin(), il.end());
		return *this;
	}

	/// @}
	//! @name modifiers
	/// @{

	// single element

	/*!
	    Inserts the element, if it's not already in the container.
	    Returns a pair:
	     first: an iterator to the element (just inserted or already there).
	     second: true if the insertion took place.
	*/
	std::pair<iterator, bool> insert(const value_type& val)
	{
		return emplace(val);
	}


	//! @c insert(val) overload that takes an rvalue val.
	std::pair<iterator, bool> insert(value_type&& val)
	{
		return emplace(std::move(val));
	}

	//! @c insert(val) overload that takes a @c Value&&.
	//!	@c value_type must be constructible from @c Value&&.
	template <class Value,
	          class = std::enable_if_t<
	              std::is_constructible<value_type, Value&&>::value>>
	std::pair<iterator, bool> insert(Value&& val)
	{
		return emplace(std::forward<Value>(val));
	}

	// with hint

	/*!
	    Inserts the element, if it's not already in the container.
	     - Returns an iterator to the element (just inserted or already there).
	     - Uses the hint to find the insertion place (see @c emplace_hint).
	*/
	iterator insert(const_iterator hint, const value_type& val)
	{
		return emplace_hint(hint, val);
	}


	//! @c insert(hint,val) overload that takes an rvalue @c val.
	iterator insert(const_iterator hint, value_type&& val)
	{
		return emplace_hint(hint, std::move(val));
	}


	//! @c insert(hint,val) overload that takes a @c Value&&.
	//!	@c Value must be convertible to @c value_type.
	template <class Value,
	          class = std::enable_if_t<
	              std::is_constructible<value_type, Value&&>::value>>
	iterator insert(const_iterator hint, Value&& val)
	{
		return emplace_hint(hint, std::forward<Value>(val));
	}

	// range

	//! Inserts the elements in the range @c [first,last). Requires value_type
	//! to be MoveAssignable, if the container is a set.
	template <class InputIterator>
	void insert(InputIterator in_first, InputIterator in_last)
	{
		// Check the std::unique requirement for set.
		// Map pair wrapper is move-assignable by design.
		static_assert(std::is_move_assignable<impl_value_type>::value,
		              "set: value_type must be MoveAssignable to perform range "
		              "insertion");

		// Copy the inserted elements to a temporary vector.
		auto temp_data =
		    impl_container_type(in_first, in_last, this->data.get_allocator());

		// If the inserted range isn't sorted, stable sort it to preserve
		// equivalent element order.
		if (!std::is_sorted(temp_data.begin(), temp_data.end(),
		                    this->comparator))
			std::stable_sort(temp_data.begin(), temp_data.end(),
			                 this->comparator);

		// If there were no elements in the container to begin with.
		if (this->data.empty())
		{
			// Remove duplicates and swap data.

			auto equals = [&](const impl_value_type& x,
			                  const impl_value_type& y) {
				return !(this->comparator(x, y) || this->comparator(y, x));
			};
			auto new_end =
			    std::unique(temp_data.begin(), temp_data.end(), equals);
			temp_data.resize(std::distance(temp_data.begin(), new_end));
			this->data.swap(temp_data);
			return;
		}

		if (temp_data.empty())
			return;

		// Create a new container for all the elements.
		auto new_data = impl_container_type(this->data.get_allocator());
		new_data.reserve(this->data.size() + temp_data.size());

		// Unique merge data and temp_data into new_data.

		auto first = this->data.begin();
		auto first_end = this->data.end();
		auto second = temp_data.begin();
		auto second_end = temp_data.end();

		// Insert the first element.
		if (this->comparator(*first, *second)) // If first goes before second.
			new_data.push_back(std::move(*first++)); // Move first.
		else
		{
			if (this->comparator(*second,
			                     *first)) // If second goes before first.
				new_data.push_back(std::move(*second++)); // Move second.
			else
			{                                            // Both are equal.
				new_data.push_back(std::move(*first++)); // Move first.
				++second;                                // Skip second.
			}
		}

		// Insert the rest.
		while (true)
		{
			if (first == first_end) // Reached the end of the 1st range.
			{
				// Insert the rest of the 2nd range. Make sure it's unique.
				auto last = new_data.begin() + (new_data.size() - 1);
				auto j = second;
				auto j_end = second_end;

				while (j != j_end)
				{
					// if (*last < *j) means if(*last != *j) here.
					if (this->comparator(*last, *j))
					{
						// Ignore if same as last.
						new_data.emplace_back(std::move(*j));
						last = j;
					}
					++j;
				}

				break;
			}
			if (second == second_end) // Reached the end of the 2nd range.
			{
				// Insert the rest of the 1st range (no need to unique - it was
				// the original sorted data vector).
				new_data.insert(new_data.cend(), std::make_move_iterator(first),
				                std::make_move_iterator(first_end));
				break;
			}

			if (this->comparator(*first,
			                     *second)) // If first goes before second.
			{
				// If the last stored goes before second (i.e. it's not equal)
				if (this->comparator(new_data.back(), *first))
					new_data.push_back(std::move(*first++)); // Move 1st.
				else
					++first; // Skip duplicate.
			}
			else
			{
				if (this->comparator(*second,
				                     *first)) // If second goes before first.
				{
					// If the last stored goes before second (i.e. it's not
					// equal)
					if (this->comparator(new_data.back(), *second))
						new_data.push_back(std::move(*second++)); // Move 2nd.
					else
						++second; // Skip duplicate.
				}
				else // Both are equal
				{
					// If the last stored goes before first (i.e. it's not
					// equal)
					if (this->comparator(new_data.back(), *first))
						new_data.push_back(std::move(*first++)); // Move first.
					else
						++first; // Skip duplicate.
					++second;    // Skip second.
				}
			}
		}

		// Use the new vector.
		this->data.swap(new_data);
	}

	// initializer list

	//! Inserts the elements from the initializer list. Requires value_type
	//! to be MoveAssignable.
	void insert(std::initializer_list<value_type> il)
	{
		insert(il.begin(), il.end());
	}

	//! Erases all elements matching the key (at most 1 in a map/set).
	//! Returns the erased element count.
	size_type erase(const key_type& key)
	{
		auto result = find_or_upper_bound(key);
		if (!result.second) // If not found, none to be erased.
			return 0;

		this->data.erase(
		    static_cast<typename impl_container_type::const_iterator>(
		        result.first));
		return 1; // Erased one element.
	}

	//! Erases the element at @c position.
	//! Returns the iterator following the removed element.
	//! Tag used to disambiguate (see multimap).
	iterator erase(const_iterator position, size_t)
	{
		return MyBase::erase(position);
	}

	//! Erases the elements in the range [first,last).
	//! Returns the iterator following the last removed element.
	iterator erase(const_iterator first, const_iterator last)
	{
		return MyBase::erase(first, last);
	}

	/*!
	    Inserts the element by constructing it in-place with the given @c args,
	    if it's not already in the container.
	    Returns a pair:
	     first: an iterator to the element (just inserted or already there).
	     second: true if the insertion took place.
	*/
	template <class... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		// Emplace at the back.
		this->data.emplace_back(std::forward<Args>(args)...);

		// Find the element matching the key.
		auto result = find_or_upper_bound(this->begin(), this->end() - 1,
		                                  this->data.back());
		if (result.second) // If found.
		{
			// Remove the emplaced element.
			this->data.pop_back();
			return std::make_pair(result.first, false); // Not inserted.
		}

		// Otherwise, rotate the emplaced element to its proper place.
		// Important: rotating using the vector iterator, not
		// contiguous_single_base::iterator, which may be a wrapper that, when
		// dereferenced, returns a reference to a non-move-assignable type.
		std::rotate<typename impl_container_type::iterator>(this->data.begin() +
		                                                        (result.first -
		                                                         this->begin()),
		                                                    this->data.end() -
		                                                        1,
		                                                    this->data.end());

		return std::make_pair(result.first, true); // Inserted.
	}

	/*!
	    Inserts the element by constructing it in-place with the given @c args,
	    if it's not already in the container.
	     - Returns an iterator to the element (just inserted or already there).
	     - Uses the hint to find the insertion place. If the position is just
	     prior to hint, the search complexity is amortized constant.
	*/
	template <class... Args>
	iterator emplace_hint(const_iterator hint, Args&&... args)
	{
		// Save the hint in case the iterators are invalidated.
		auto hint_offset = hint - this->begin();

		// Emplace at the back.
		this->data.emplace_back(std::forward<Args>(args)...);

		// Find the element matching the key.
		auto result = find_or_upper_bound_hint(this->begin(), this->end() - 1,
		                                       this->begin() + hint_offset,
		                                       this->data.back());

		if (result.second) // If found.
		{
			// Remove the emplaced element.
			this->data.pop_back();
			return result.first; // Return the element already there.
		}

		// Otherwise, rotate the emplaced element to its proper place.
		// Important: rotating using the vector iterator, not
		// contiguous_single_base::iterator, which may be a wrapper that, when
		// dereferenced, returns a reference to a non-move-assignable type.
		std::rotate<decltype(this->data.end())>(this->data.begin() +
		                                            (result.first -
		                                             this->begin()),
		                                        this->data.end() - 1,
		                                        this->data.end());

		// Return an iterator the emplaced element.
		return result.first;
	}

	/// @}
	//! @name map/set operations
	/// @{

	//! Returns an iterator to the element with a key equivalent to key.
	//! If such element was not found, end() is returned.
	iterator find(const key_type& key)
	{
		auto result = find_or_upper_bound(key);
		if (result.second) // If found, return the iterator.
			return result.first;
		return this->end();
	}

	//! Const overload of find(key).
	const_iterator find(const key_type& key) const
	{
		auto result = find_or_upper_bound(key);
		if (result.second) // If found, return the iterator.
			return result.first;
		return this->end();
	}

	//! Returns an iterator to the element with a key that compares equivalent
	//! to the value @c x.
	//! If such element was not found, end() is returned.
	//! Only participates in overload resolution,
	//! if key_compare::is_transparent is valid and denotes a type.
	template <class K,
	          class = typename std::enable_if<
	              internal::check_is_transparent<key_compare, K>::value>::type>
	iterator find(const K& x)
	{
		auto result = find_or_upper_bound(x);
		if (result.second) // If found, return the iterator.
			return result.first;
		return this->end();
	}

	//! Const overload of @c find(x).
	template <class K,
	          class = typename std::enable_if<
	              internal::check_is_transparent<key_compare, K>::value>::type>
	const_iterator find(const K& x) const
	{
		auto result = find_or_upper_bound(x);
		if (result.second) // If found, return the iterator.
			return result.first;
		return this->end();
	}

	//! Returns the count of elements with a key matching @c key (at most 1 in a
	//! map/set).
	size_type count(const key_type& key) const
	{
		// If found - 1, otherwise 0.
		return find_or_upper_bound(key).second ? 1 : 0;
	}

	//! Returns the count of elements with a key that compares equivalent to the
	//! value @c x (at most 1 in a map/set).
	//! Only participates in overload resolution,
	//! if key_compare::is_transparent is valid and denotes a type.
	template <class K,
	          class = typename std::enable_if<
	              internal::check_is_transparent<key_compare, K>::value>::type>
	size_type count(const K& x) const
	{
		// If found - 1, otherwise 0.
		return find_or_upper_bound(x).second ? 1 : 0;
	}


	//! Return the pair (lower bound, upper bound). Compares keys with @c key.
	std::pair<iterator, iterator> equal_range(const key_type& key)
	{
		auto result = find_or_upper_bound(key);
		if (result.second) // If found, the next is greater.
			return std::make_pair(result.first, result.first + 1);

		// Otherwise, both are greater.
		return std::make_pair(result.first, result.first);
	}

	//! Return the the pair (lower bound, upper bound). Compares keys with @c
	//! key.
	std::pair<const_iterator, const_iterator> equal_range(
	    const key_type& key) const
	{
		auto result = find_or_upper_bound(key);
		if (result.second) // If found, the next is greater.
			return std::make_pair(result.first, result.first + 1);

		// Otherwise, both are greater.
		return std::make_pair(result.first, result.first);
	}


	//! Return the pair (lower bound, upper bound). Compares keys with @c x.
	//! Only participates in overload resolution,
	//! if key_compare::is_transparent is valid and denotes a type.
	template <class K,
	          class = std::enable_if<
	              internal::check_is_transparent<key_compare, K>::value>>
	std::pair<iterator, iterator> equal_range(const K& x)
	{
		auto result = find_or_upper_bound(x);
		if (result.second) // If found, the next is greater.
			return std::make_pair(result.first, result.first + 1);

		// Otherwise, both are greater.
		return std::make_pair(result.first, result.first);
	}

	//! Return the pair (lower bound, upper bound). Compares keys with @c x.
	//! Only participates in overload resolution,
	//! if key_compare::is_transparent is valid and denotes a type.
	template <class K,
	          class = typename std::enable_if<
	              internal::check_is_transparent<key_compare, K>::value>::type>
	std::pair<const_iterator, const_iterator> equal_range(const K& x) const
	{
		auto result = find_or_upper_bound(x);
		if (result.second) // If found, the next is greater.
			return std::make_pair(result.first, result.first + 1);

		// Otherwise, both are greater.
		return std::make_pair(result.first, result.first);
	}

	/// @}

protected:
	/*!
	    Return the lower bound in first.
	     - second is true, if the lower bound matches the key (i.e. its not the
	   upper bound).
	*/
	template <class T>
	std::pair<iterator, bool> find_or_upper_bound(iterator begin,
	                                              iterator end,
	                                              const T& v)
	{
		auto lbound = std::lower_bound(begin, end, v, this->comparator);
		// If not found, or only found upper bound (i.e. key!=lbound).
		if (lbound == end || this->comparator(v, *lbound))
			return std::make_pair(lbound, false);

		return std::make_pair(lbound, true); // found
	}

	template <class T>
	std::pair<const_iterator, bool> find_or_upper_bound(const_iterator begin,
	                                                    const_iterator end,
	                                                    const T& v) const
	{
		auto lbound = std::lower_bound(begin, end, v, this->comparator);
		if (lbound == this->cend() ||
		    this->comparator(v, *lbound)) // If not found, or only
			                              // found upper bound
			                              // (i.e. key!=lbound).
			return std::make_pair(lbound, false);

		return std::make_pair(lbound, true); // found
	}

	// Shorthands

	template <class T>
	std::pair<iterator, bool> find_or_upper_bound(const T& v)
	{
		return find_or_upper_bound(this->begin(), this->end(), v);
	}
	template <class T>
	std::pair<const_iterator, bool> find_or_upper_bound(const T& v) const
	{
		return find_or_upper_bound(this->begin(), this->end(), v);
	}



	/*!
	    Return the lower bound in first.
	     - second is true, if the lower bound matches the key (it's not the
	   upper bound).
	     - use a hint

	    If the position is just prior to hint, the complexity is amortized
	   constant.

	    Supports unique keys.
	*/
	template <class T>
	std::pair<iterator, bool> find_or_upper_bound_hint(iterator begin,
	                                                   iterator end,
	                                                   iterator hint,
	                                                   const T& v)
	{
		if (hint == end)
		{
			if (end - begin == 0 ||
			    this->comparator(*(end - 1), v)) // empty || last < key
				return std::make_pair(hint,
				                      false); // not found, and hint is the
			                                  // upper
			                                  // bound

			// therefore last >= key

			if (!this->comparator(v,
			                      *(end -
			                        1))) // last <= key, therefore last == key
				return std::make_pair(hint - 1,
				                      true); // found, and last is the lower
			                                 // bound

			// else: key < last [end], so the hint was wrong, find using
			// find_or_upper_bound
		}
		else if (hint == begin)
		{
			if (this->comparator(v, *hint)) // key < begin
				return std::make_pair(hint,
				                      false); // not found, and hint is the
			                                  // upper
			                                  // bound

			// else: begin <= key, so the hint was wrong, find using
			// find_or_upper_bound
		}
		else if (this->comparator(v, *hint)) // key < hint
		{
			if (this->comparator(*(hint - 1), v)) // prev < key < hint
				return std::make_pair(hint,
				                      false); // not found, and hint is the
			                                  // upper
			                                  // bound

			// else: key <= prev < hint

			if (!this->comparator(v,
			                      *(hint -
			                        1))) // key >= prev, therefore key == prev
				return std::make_pair(hint - 1,
				                      true); // found, and hint-1 is the lower
			                                 // bound

			// else: key < prev, so the hint was wrong, find using
			// find_or_upper_bound
		}
		// else: hint <= key, so the hint was wrong, find using
		// find_or_upper_bound

		return find_or_upper_bound(v);
	}

	// Shorthand

	template <class T>
	std::pair<iterator, bool> find_or_upper_bound_hint(const_iterator hint,
	                                                   const T& v)
	{
		return find_or_upper_bound_hint(this->begin(), this->end(),
		                                this->begin() + (hint - this->cbegin()),
		                                v);
	}
};
}
