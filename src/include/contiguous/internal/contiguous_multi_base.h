#pragma once

#include "contiguous_base.h"

/**
    @file contiguous_multi_base.h
    @brief Common functionality of associative containers that support
   equivalent keys - @c contiguous_multimap and @c contiguous_multiset.
*/

namespace contiguous
{

/*!
    @brief Common functionality of associative containers that support
   equivalent keys - @c contiguous_multimap and @c contiguous_multiset.
*/
template <class Traits>
class contiguous_multi_base : public contiguous_base<Traits>
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
	contiguous_multi_base() noexcept(noexcept(key_compare()) &&
	                                 noexcept(allocator_type()))
	  : contiguous_multi_base(key_compare(), allocator_type())
	{
	}

	//! Constructs an empty container using the comparison object and allocator.
	explicit contiguous_multi_base(
	    const key_compare& comp,
	    const allocator_type& alloc = allocator_type())
	  : MyBase(comp, alloc)
	{
	}

	//! Constructs an empty container using the allocator.
	explicit contiguous_multi_base(const allocator_type& alloc) : MyBase(alloc)
	{
	}

	// range

	//! Constructs an empty container using the comparison object and allocator,
	//! and inserts elements from the range [first,last).
	template <class InputIterator>
	contiguous_multi_base(InputIterator first,
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
	contiguous_multi_base(InputIterator first,
	                      InputIterator last,
	                      const allocator_type& alloc)
	  : contiguous_multi_base(first, last, key_compare(), alloc)
	{
	}

	// copy

	//! Constructs by copying other.
	contiguous_multi_base(const contiguous_multi_base& other) : MyBase(other)
	{
	}

	//! Constructs by copying other, using a different allocator.
	contiguous_multi_base(const contiguous_multi_base& other,
	                      const allocator_type& alloc)
	  : MyBase(other, alloc)
	{
	}

	// move

	//! Constructs by moving other.
	contiguous_multi_base(contiguous_multi_base&& other) noexcept(
	    noexcept(MyBase(std::move(other))))
	  : MyBase(std::move(other))
	{
	}

	//! Constructs by moving other, using a different allocator.
	contiguous_multi_base(contiguous_multi_base&& other,
	                      const allocator_type& alloc)
	  : MyBase(std::move(other), alloc)
	{
	}

	// initializer list

	//! Constructs an empty container using the comparison object and allocator,
	//! and inserts elements from the initializer list.
	contiguous_multi_base(std::initializer_list<value_type> il,
	                      const key_compare& comp = key_compare(),
	                      const allocator_type& alloc = allocator_type())
	  : MyBase(comp, alloc)
	{
		insert(il.begin(), il.end());
	}

	//! Constructs using the allocator and inserts elements from the
	//! initializer list.
	contiguous_multi_base(std::initializer_list<value_type> il,
	                      const allocator_type& alloc)
	  : contiguous_multi_base(il, key_compare(), alloc)
	{
	}

	//! Destructor.
	~contiguous_multi_base()
	{
	}

	// copy

	//! Copy-assigns other.
	contiguous_multi_base& operator=(const contiguous_multi_base& other)
	{
		MyBase::operator=(other);
		return *this;
	}
	// move

	//! Move-assigns other.
	contiguous_multi_base& operator=(contiguous_multi_base&& other) noexcept(
	    noexcept(other.MyBase::operator=(std::move(other))))
	{
		MyBase::operator=(std::move(other));
		return *this;
	}

	// initializer list@c value_type

	//! Replaces the elements with the elements from the initializer list.
	contiguous_multi_base& operator=(std::initializer_list<value_type> il)
	{
		this->clear();
		insert(il.begin(), il.end());
		return *this;
	}

	/// @}
	//! @name modifiers
	/// @{

	// single element

	//! Insert the value at its upper bound.
	iterator insert(const value_type& val)
	{
		return emplace(val);
	}


	//! @c insert(val) overload that takes an rvalue @c val.
	iterator insert(value_type&& val)
	{
		return emplace(std::move(val));
	}


	//! @c insert(val) overload that takes a @c Value&&.
	//!	@c value_type must be constructible from @c Value&&.
	template <class Value,
	          class = std::enable_if_t<
	              std::is_constructible<value_type, Value&&>::value>>
	iterator insert(Value&& val)
	{
		return emplace(std::forward<Value>(val));
	}

	// with hint

	/*!
	    Inserts the element, in the position as close as possible, just prior,
	    to @c hint (see @c emplace_hint).
	    Returns an iterator to the inserted element.
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
	//!	@c value_type must be constructible from @c Value&&.
	template <class Value,
	          class = std::enable_if_t<
	              std::is_constructible<value_type, Value&&>::value>>
	iterator insert(const_iterator hint, Value&& val)
	{
		return emplace_hint(hint, std::forward<Value>(val));
	}

	// range


	//! Inserts the elements in the range @c [first,last). Requires value_type
	//! to be MoveAssignable, if the container is a multiset.
	template <class InputIterator>
	void insert(InputIterator in_first, InputIterator in_last)
	{
		// Check the std::stable_sort requirement for multiset.
		// Multimap pair wrapper is move-assignable by design.
		static_assert(std::is_move_assignable<impl_value_type>::value,
		              "multiset: value_type must be MoveAssignable to perform "
		              "range "
		              "insertion");

		// Save the number of elements before the insertion.
		auto data_initial_size = this->data.size();

		// Insert all the elements at the back.
		this->data.insert(this->data.end(), in_first, in_last);

		auto first = this->data.begin();
		auto middle = first + data_initial_size;
		auto last = this->data.end();

		// If the inserted range isn't sorted, stable sort it to preserve
		// equivalent element order.
		if (!std::is_sorted(middle, last, this->comparator))
			std::stable_sort(middle, last, this->comparator);

		// Merge the two ranges
		std::inplace_merge(first, middle, last, this->comparator);
	}

	// initializer list


	//! Inserts the elements from the initializer list. Requires value_type to
	//! be MoveAssignable.
	void insert(std::initializer_list<value_type> il)
	{
		insert(il.begin(), il.end());
	}


	//! Erases all elements matching the key.
	//! Returns the erased element count.
	size_type erase(const key_type& key)
	{
		auto range = equal_range(key);
		auto count = std::distance(range.first, range.second);
		this->data
		    .erase(static_cast<typename impl_container_type::const_iterator>(
		               range.first),
		           static_cast<typename impl_container_type::const_iterator>(
		               range.second));
		return count;
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

	//! Inserts the element at the upper bound by constructing it in-place with
	//! the given @c args.
	//! Returns an iterator to the inserted element.
	template <class... Args>
	iterator emplace(Args&&... args)
	{
		// Emplace at the back.
		this->data.emplace_back(std::forward<Args>(args)...);

		// Get the upper bound to position.
		auto pos = std::upper_bound(this->begin(), this->end() - 1,
		                            this->data.back(), this->comparator);

		// Rotate the emplaced element to its proper place.
		// Important: rotating using the vector iterator, not
		// contiguous_multi_base::iterator, which may be a wrapper that, when
		// dereferenced, returns a reference to a non-move-assignable type.
		std::rotate<decltype(this->data.end())>(this->data.begin() +
		                                            (pos - this->begin()),
		                                        this->data.end() - 1,
		                                        this->data.end());

		// Return an iterator the emplaced element.
		return pos;
	}

	/*!
	    Inserts the element by constructing it in-place in the position as close
	    as possible, just prior, to @c hint. If the position is just prior to
	   hint,
	    the search complexity is amortized constant.
	*/
	template <class... Args>
	iterator emplace_hint(const_iterator hint, Args&&... args)
	{
		// Save the hint in case the iterators are invalidated.
		auto hint_offset = hint - this->begin();

		// Emplace at the back.
		this->data.emplace_back(std::forward<Args>(args)...);

		// Get the insertion position.
		auto pos = get_insert_position_hint(this->begin(), this->end() - 1,
		                                    this->begin() + hint_offset,
		                                    this->data.back());

		// Rotate the emplaced element to its proper place.
		// Important: rotating using the vector iterator, not
		// contiguous_multi_base::iterator, which may be a wrapper that, when
		// dereferenced, returns a reference to a non-move-assignable type.
		std::rotate<decltype(this->data.end())>(this->data.begin() +
		                                            (pos - this->begin()),
		                                        this->data.end() - 1,
		                                        this->data.end());

		// Return an iterator the emplaced element.
		return pos;
	}

	/// @}
	//! @name map/set operations
	/// @{

	//! Returns an iterator to any element with a key equivalent to key.
	//! If such element was not found, end() is returned.
	iterator find(const key_type& key)
	{
		auto lbound = this->lower_bound(key);

		// If not found, or only found upper bound (i.e. val != lbound).
		if (lbound == this->end() || this->comparator(key, *lbound))
			return this->end();

		return lbound; // found
	}

	//! Const overload of @c find(key).
	const_iterator find(const key_type& key) const
	{
		auto lbound = this->lower_bound(key);

		// If not found, or only found upper bound (i.e. val != lbound).
		if (lbound == this->end() || this->comparator(key, *lbound))
			return this->end();

		return lbound; // found
	}

	//! Returns an iterator to any element with a key that compares equivalent
	//! to the value @c x.
	//! If such element was not found, end() is returned.
	//! Only participates in overload resolution,
	//! if key_compare::is_transparent is valid and denotes a type.
	template <class K,
	          class = typename std::enable_if<
	              internal::check_is_transparent<key_compare, K>::value>::type>
	iterator find(const K& x)
	{
		auto lbound = this->lower_bound(x);

		// If not found, or only found upper bound (i.e. val != lbound).
		if (lbound == this->end() || this->comparator(x, *lbound))
			return this->end();

		return lbound; // found
	}

	//! Const overload of @c find(x).
	template <class K,
	          class = typename std::enable_if<
	              internal::check_is_transparent<key_compare, K>::value>::type>
	const_iterator find(const K& x) const
	{
		auto lbound = this->lower_bound(x);

		// If not found, or only found upper bound (i.e. val != lbound).
		if (lbound == this->end() || this->comparator(x, *lbound))
			return this->end();

		return lbound; // found
	}


	//! Returns the count of elements with a key matching @c key.
	size_type count(const key_type& key) const
	{
		auto range = equal_range(key);
		return std::distance(range.first, range.second);
	}

	//! Returns the count of elements with a key that compares equivalent to the
	//! value @c x.
	//! Only participates in overload resolution,
	//! if key_compare::is_transparent is valid and denotes a type.
	template <class K,
	          class = typename std::enable_if<
	              internal::check_is_transparent<key_compare, K>::value>::type>
	size_type count(const K& x) const
	{
		auto range = equal_range(x);
		return std::distance(range.first, range.second);
	}

	//! Returns the pair (lower bound, upper bound). Compares the keys to @c
	//! key.
	std::pair<iterator, iterator> equal_range(const key_type& key)
	{
		return std::equal_range(this->begin(), this->end(), key,
		                        this->comparator);
	}

	//! Const overload of @c equal_range(key).
	std::pair<const_iterator, const_iterator> equal_range(
	    const key_type& key) const
	{
		return std::equal_range(this->cbegin(), this->cend(), key,
		                        this->comparator);
	}

	//! Returns the pair (lower bound, upper bound).
	//! Compares the keys to the value @c x.
	//! Only participates in overload resolution,
	//! if key_compare::is_transparent is valid and denotes a type.
	template <class K,
	          class = typename std::enable_if<
	              internal::check_is_transparent<key_compare, K>::value>::type>
	std::pair<iterator, iterator> equal_range(const K& x)
	{
		return std::equal_range(this->begin(), this->end(), x,
		                        this->comparator);
	}

	//! Const overload of @c equal_range(x).
	template <class K,
	          class = typename std::enable_if<
	              internal::check_is_transparent<key_compare, K>::value>::type>
	std::pair<const_iterator, const_iterator> equal_range(const K& x) const
	{
		return std::equal_range(this->cbegin(), this->cend(), x,
		                        this->comparator);
	}

	/// @}

protected:
	/*
	    Find the position to insert v as close as possible to hint.
	    If the position is just prior to hint, the complexity is amortized
	   constant.

	    Supports equivalent keys.
	*/
	template <class T>
	iterator get_insert_position_hint(iterator begin,
	                                  iterator end,
	                                  iterator hint,
	                                  const T& v) const
	{
		if (hint == end)
		{
			// empty || last <= key
			if (end - begin == 0 || !this->comparator(v, *(end - 1)))
				return hint; // end() is the correct insertion place (the upper
			                 // bound)

			// else: key < last [end], so the hint was wrong, find upper_bound
			// to insert
			// as close to the end as possible
			return std::upper_bound(begin, end, v, this->comparator);
		}
		else if (hint == begin)
		{
			if (!this->comparator(*hint, v)) // key <= begin
				return hint; // begin() is the correct insertion place (the
			                 // upper bound)

			// else: [begin] begin < key, so the hint was wrong, find
			// lower_bound to insert
			// as close to the beginning as possible
			return std::lower_bound(begin, end, v, this->comparator);
		}
		else if (!this->comparator(*hint, v)) // key <= hint
		{
			if (!this->comparator(v, *(hint - 1))) // previous <= key <= hint
				return hint; // hint is the correct insertion place (just prior
			                 // to hint, and ordering is preserved)

			// else: key < previous < hint, so the hint was wrong, find
			// upper_bound to insert
			// as close to hint as possible
			return std::upper_bound(begin, end, v, this->comparator);
		}

		// else: hint < key, so the hint was wrong, find lower_bound to insert
		// as close to hint as possible
		return std::lower_bound(begin, end, v, this->comparator);
	}
};
}
