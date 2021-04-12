#pragma once

#include <vector>

#include "internal/contiguous_multi_base.h"

/**
    @file multiset.h
    @brief @c std::multiset interface on an ordered vector.
*/

namespace contiguous
{

/*!
    @brief @c std::multiset interface on an ordered vector.

    @param Key key type
    @param Compare strict weak order comparator
    @param Alloc allocator
    @param Container container compatible with @c std::vector

*/
template <class Key,
          class Compare = std::less<Key>,
          class Alloc = std::allocator<Key>,
          template <class V, class A> class Container = std::vector>
class multiset : public contiguous_multi_base<
                     internal::set_traits<Key, Compare, Alloc, Container>>
{
private:
	// the base class, same as above
	using MyBase = contiguous_multi_base<
	    internal::set_traits<Key, Compare, Alloc, Container>>;

	using typename MyBase::impl_container_type;

public:
	//! @name types
	/// @{

	using typename MyBase::key_type;

	using typename MyBase::value_type;
	using typename MyBase::allocator_type;

	using typename MyBase::key_compare;
	using typename MyBase::value_compare;
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

	//! Constructs an empty multiset.
	multiset() noexcept(noexcept(key_compare()) && noexcept(allocator_type()))
	  : multiset(key_compare(), allocator_type())
	{
	}

	//! Constructs an empty multiset using the comparison object and allocator.
	explicit multiset(const key_compare& comp,
	                  const allocator_type& alloc = allocator_type())
	  : MyBase(comp, alloc)
	{
	}

	//! Constructs an empty multiset using the allocator.
	explicit multiset(const allocator_type& alloc) : MyBase(alloc)
	{
	}

	// range

	//! Constructs an empty multiset using the comparison object and allocator,
	//! and inserts elements from the range [first,last).
	template <class InputIterator>
	multiset(InputIterator first,
	         InputIterator last,
	         const key_compare& comp = key_compare(),
	         const allocator_type& alloc = allocator_type())
	  : MyBase(first, last, comp, alloc)
	{
	}

	//! Constructs using the allocator, and inserts elements from the range
	//! [first,last).
	template <class InputIterator>
	multiset(InputIterator first,
	         InputIterator last,
	         const allocator_type& alloc)
	  : multiset(first, last, key_compare(), alloc)
	{
	}

	// copy

	//! Constructs by copying other.
	multiset(const multiset& other) : MyBase(other)
	{
	}

	//! Constructs by copying other, using a different allocator.
	multiset(const multiset& other, const allocator_type& alloc)
	  : MyBase(other, alloc)
	{
	}

	// move

	//! Constructs by moving other.
	multiset(multiset&& other) noexcept(noexcept(MyBase(std::move(other))))
	  : MyBase(std::move(other))
	{
	}

	//! Constructs by moving other, using a different allocator.
	multiset(multiset&& other, const allocator_type& alloc)
	  : MyBase(std::move(other), alloc)
	{
	}

	// initializer list

	//! Constructs an empty multiset using the comparison object and allocator,
	//! and inserts elements from the initializer list.
	multiset(std::initializer_list<value_type> il,
	         const key_compare& comp = key_compare(),
	         const allocator_type& alloc = allocator_type())
	  : MyBase(il, comp, alloc)
	{
	}

	//! Constructs using the allocator and inserts elements from the
	//! initializer list.
	multiset(std::initializer_list<value_type> il, const allocator_type& alloc)
	  : multiset(il, key_compare(), alloc)
	{
	}

	//! Destructor.
	~multiset()
	{
	}

	// copy

	//! Copy-assigns other.
	multiset& operator=(const multiset& other)
	{
		MyBase::operator=(other);
		return *this;
	}
	// move

	//! Move-assigns other.
	multiset& operator=(multiset&& other) noexcept(
	    noexcept(other.MyBase::operator=(std::move(other))))
	{
		MyBase::operator=(std::move(other));
		return *this;
	}

	// initializer list

	//! Replaces the elements with the elements from the initializer list.
	multiset& operator=(std::initializer_list<value_type> il)
	{
		MyBase::operator=(il);
		return *this;
	}

	/// @}
	//! @name modifiers
	/// @{

	//! Erases all elements matching the key.
	//! Returns the erased element count.
	size_type erase(const key_type& key)
	{
		return MyBase::erase(key);
	}

	//! Erases the element at @c position.
	//! Returns the iterator following the removed element.
	iterator erase(const_iterator position)
	{
		return MyBase::erase(position, size_t());
	}

	//! Erases the elements in the range [first,last).
	//! Returns the iterator following the last removed element.
	iterator erase(const_iterator first, const_iterator last)
	{
		return MyBase::erase(first, last);
	}

	/// @}
	//! @name observers
	/// @{

	//! Returns a copy of the key comparator.
	key_compare key_comp() const
	{
		return this->comparator;
	}

	/// @}

	// friend operators

	template <class K, class P, class A, template <class, class> class C>
	friend bool operator==(const multiset<K, P, A, C>&,
	                       const multiset<K, P, A, C>&);
	template <class K, class P, class A, template <class, class> class C>
	friend bool operator!=(const multiset<K, P, A, C>&,
	                       const multiset<K, P, A, C>&);
	template <class K, class P, class A, template <class, class> class C>
	friend bool operator<(const multiset<K, P, A, C>&,
	                      const multiset<K, P, A, C>&);
	template <class K, class P, class A, template <class, class> class C>
	friend bool operator<=(const multiset<K, P, A, C>&,
	                       const multiset<K, P, A, C>&);
	template <class K, class P, class A, template <class, class> class C>
	friend bool operator>(const multiset<K, P, A, C>&,
	                      const multiset<K, P, A, C>&);
	template <class K, class P, class A, template <class, class> class C>
	friend bool operator>=(const multiset<K, P, A, C>&,
	                       const multiset<K, P, A, C>&);
};

//! equality
template <class K, class P, class A, template <class, class> class C>
bool operator==(const multiset<K, P, A, C>& lhs,
                const multiset<K, P, A, C>& rhs)
{
	return lhs.data == rhs.data;
}

//! inequality
template <class K, class P, class A, template <class, class> class C>
bool operator!=(const multiset<K, P, A, C>& lhs,
                const multiset<K, P, A, C>& rhs)
{
	return lhs.data != rhs.data;
}

//! less than
template <class K, class P, class A, template <class, class> class C>
inline bool operator<(const multiset<K, P, A, C>& lhs,
                      const multiset<K, P, A, C>& rhs)
{
	return lhs.data < rhs.data;
}

//! less or equal to
template <class K, class P, class A, template <class, class> class C>
inline bool operator<=(const multiset<K, P, A, C>& lhs,
                       const multiset<K, P, A, C>& rhs)
{
	return lhs.data <= rhs.data;
}

//! greater than
template <class K, class P, class A, template <class, class> class C>
inline bool operator>(const multiset<K, P, A, C>& lhs,
                      const multiset<K, P, A, C>& rhs)
{
	return lhs.data > rhs.data;
}

//! greater or equal to
template <class K, class P, class A, template <class, class> class C>
inline bool operator>=(const multiset<K, P, A, C>& lhs,
                       const multiset<K, P, A, C>& rhs)
{
	return lhs.data >= rhs.data;
}

//! swap
template <class K, class P, class A, template <class, class> class C>
inline void swap(multiset<K, P, A, C>& lhs,
                 multiset<K, P, A, C>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
	lhs.swap(rhs);
}
}
