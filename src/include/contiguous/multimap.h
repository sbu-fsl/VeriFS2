#pragma once

#include "internal/contiguous_multi_base.h"

/**
    @file multimap.h
    @brief @c std::multimap interface on an ordered vector.
*/

namespace contiguous
{

/*!
    @brief @c std::multimap interface on an ordered vector.

    @param Key key type
    @param T mapped type
    @param Compare strict weak order comparator
    @param Alloc allocator
    @param Container container compatible with @c std::vector

*/
template <class Key,
          class T,
          class Compare = std::less<Key>,
          class Alloc =
              std::allocator<std::pair<internal::qualify_key<Key>, T>>,
          template <class V, class A> class Container = std::vector>
class multimap : public contiguous_multi_base<
                     internal::map_traits<Key, T, Compare, Alloc, Container>>
{
private:
	// the base class, same as above
	using MyBase = contiguous_multi_base<
	    internal::map_traits<Key, T, Compare, Alloc, Container>>;

	using typename MyBase::impl_container_type;

public:
	//! @name types
	/// @{

	using typename MyBase::key_type;
	using mapped_type = T;
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

	//! Constructs an empty multimap.
	multimap() noexcept(noexcept(key_compare()) && noexcept(allocator_type()))
	  : multimap(key_compare(), allocator_type())
	{
	}

	//! Constructs an empty multimap using the comparison object and allocator.
	explicit multimap(const key_compare& comp,
	                  const allocator_type& alloc = allocator_type())
	  : MyBase(comp, alloc)
	{
	}

	//! Constructs an empty multimap using the allocator.
	explicit multimap(const allocator_type& alloc) : MyBase(alloc)
	{
	}

	// range

	//! Constructs an empty multimap using the comparison object and allocator,
	//! and inserts elements from the range [first,last).
	template <class InputIterator>
	multimap(InputIterator first,
	         InputIterator last,
	         const key_compare& comp = key_compare(),
	         const allocator_type& alloc = allocator_type())
	  : MyBase(first, last, comp, alloc)
	{
	}

	//! Constructs using the allocator, and inserts elements from the range
	//! [first,last).
	template <class InputIterator>
	multimap(InputIterator first,
	         InputIterator last,
	         const allocator_type& alloc)
	  : multimap(first, last, key_compare(), alloc)
	{
	}

	// copy

	//! Constructs by copying other.
	multimap(const multimap& other) : MyBase(other)
	{
	}

	//! Constructs by copying other, using a different allocator.
	multimap(const multimap& other, const allocator_type& alloc)
	  : MyBase(other, alloc)
	{
	}

	// move

	//! Constructs by moving other.
	multimap(multimap&& other) noexcept(noexcept(MyBase(std::move(other))))
	  : MyBase(std::move(other))
	{
	}

	//! Constructs by moving other, using a different allocator.
	multimap(multimap&& other, const allocator_type& alloc)
	  : MyBase(std::move(other), alloc)
	{
	}

	// initializer list

	//! Constructs an empty multimap using the comparison object and allocator,
	//! and inserts elements from the initializer list.
	multimap(std::initializer_list<value_type> il,
	         const key_compare& comp = key_compare(),
	         const allocator_type& alloc = allocator_type())
	  : MyBase(il, comp, alloc)
	{
	}

	//! Constructs using the allocator and inserts elements from the
	//! initializer list.
	multimap(std::initializer_list<value_type> il, const allocator_type& alloc)
	  : multimap(il, key_compare(), alloc)
	{
	}

	//! Destructor.
	~multimap()
	{
	}

	// copy

	//! Copy-assigns other.
	multimap& operator=(const multimap& other)
	{
		MyBase::operator=(other);
		return *this;
	}
	// move

	//! Move-assigns other.
	multimap& operator=(multimap&& other) noexcept(
	    noexcept(other.MyBase::operator=(std::move(other))))
	{
		MyBase::operator=(std::move(other));
		return *this;
	}

	// initializer list

	//! Replaces the elements with the elements from the initializer list.
	multimap& operator=(std::initializer_list<value_type> il)
	{
		MyBase::operator=(il);
		return *this;
	}

	/// @}
	//! @name modifiers
	/// @{


	//! Erases all elements matching the key (at most 1 in a map/set).
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

	//! @c erase(pos) - fix for an ambiguity.
	//! See: http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#2059
	template <class = std::enable_if_t<
	              !std::is_same<iterator, const_iterator>::value>>
	iterator erase(iterator position)
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
		return this->comparator.key_comp();
	}

	/// @}

	// friend operators

	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator==(const multimap<K, M, P, A, C>&,
	                       const multimap<K, M, P, A, C>&);
	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator!=(const multimap<K, M, P, A, C>&,
	                       const multimap<K, M, P, A, C>&);
	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator<(const multimap<K, M, P, A, C>&,
	                      const multimap<K, M, P, A, C>&);
	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator<=(const multimap<K, M, P, A, C>&,
	                       const multimap<K, M, P, A, C>&);
	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator>(const multimap<K, M, P, A, C>&,
	                      const multimap<K, M, P, A, C>&);
	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator>=(const multimap<K, M, P, A, C>&,
	                       const multimap<K, M, P, A, C>&);
};

//! equality
template <class K, class M, class P, class A, template <class, class> class C>
bool operator==(const multimap<K, M, P, A, C>& lhs,
                const multimap<K, M, P, A, C>& rhs)
{
	return lhs.data == rhs.data;
}

//! inequality
template <class K, class M, class P, class A, template <class, class> class C>
bool operator!=(const multimap<K, M, P, A, C>& lhs,
                const multimap<K, M, P, A, C>& rhs)
{
	return lhs.data != rhs.data;
}

//! less than
template <class K, class M, class P, class A, template <class, class> class C>
inline bool operator<(const multimap<K, M, P, A, C>& lhs,
                      const multimap<K, M, P, A, C>& rhs)
{
	return lhs.data < rhs.data;
}

//! less or equal to
template <class K, class M, class P, class A, template <class, class> class C>
inline bool operator<=(const multimap<K, M, P, A, C>& lhs,
                       const multimap<K, M, P, A, C>& rhs)
{
	return lhs.data <= rhs.data;
}

//! greater than
template <class K, class M, class P, class A, template <class, class> class C>
inline bool operator>(const multimap<K, M, P, A, C>& lhs,
                      const multimap<K, M, P, A, C>& rhs)
{
	return lhs.data > rhs.data;
}

//! greater or equal to
template <class K, class M, class P, class A, template <class, class> class C>
inline bool operator>=(const multimap<K, M, P, A, C>& lhs,
                       const multimap<K, M, P, A, C>& rhs)
{
	return lhs.data >= rhs.data;
}

//! swap
template <class K, class M, class P, class A, template <class, class> class C>
inline void swap(multimap<K, M, P, A, C>& lhs,
                 multimap<K, M, P, A, C>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
	lhs.swap(rhs);
}
}
