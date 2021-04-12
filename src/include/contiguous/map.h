#pragma once

#include <stdexcept>
#include "internal/contiguous_single_base.h"

/**
    @file map.h
    @brief @c std::map interface on an ordered vector.
*/

namespace contiguous
{

/*!
    @brief @c std::map interface on an ordered vector.

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
class map : public contiguous_single_base<
                internal::map_traits<Key, T, Compare, Alloc, Container>>
{
private:
	// the base class, same as above
	using MyBase = contiguous_single_base<
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

	//! Constructs an empty map.
	map() noexcept(noexcept(key_compare()) && noexcept(allocator_type()))
	  : map(key_compare(), allocator_type())
	{
	}

	//! Constructs an empty map using the comparison object and allocator.
	explicit map(const key_compare& comp,
	             const allocator_type& alloc = allocator_type())
	  : MyBase(comp, alloc)
	{
	}

	//! Constructs an empty map using the allocator.
	explicit map(const allocator_type& alloc) : MyBase(alloc)
	{
	}

	// range

	//! Constructs an empty map using the comparison object and allocator,
	//! and inserts elements from the range [first,last).
	template <class InputIterator>
	map(InputIterator first,
	    InputIterator last,
	    const key_compare& comp = key_compare(),
	    const allocator_type& alloc = allocator_type())
	  : MyBase(first, last, comp, alloc)
	{
	}

	//! Constructs using the allocator, and inserts elements from the range
	//! [first,last).
	template <class InputIterator>
	map(InputIterator first, InputIterator last, const allocator_type& alloc)
	  : map(first, last, key_compare(), alloc)
	{
	}

	// copy

	//! Constructs by copying other.
	map(const map& other) : MyBase(other)
	{
	}

	//! Constructs by copying other, using a different allocator.
	map(const map& other, const allocator_type& alloc) : MyBase(other, alloc)
	{
	}

	// move

	//! Constructs by moving other.
	map(map&& other) noexcept(noexcept(MyBase(std::move(other))))
	  : MyBase(std::move(other))
	{
	}

	//! Constructs by moving other, using a different allocator.
	map(map&& other, const allocator_type& alloc)
	  : MyBase(std::move(other), alloc)
	{
	}

	// initializer list


	//! Constructs an empty map using the comparison object and allocator,
	//! and inserts elements from the initializer list.
	map(std::initializer_list<value_type> il,
	    const key_compare& comp = key_compare(),
	    const allocator_type& alloc = allocator_type())
	  : MyBase(il, comp, alloc)
	{
	}

	//! Constructs using the allocator and inserts elements from the
	//! initializer list.
	map(std::initializer_list<value_type> il, const allocator_type& alloc)
	  : map(il, key_compare(), alloc)
	{
	}

	//! Destructor.
	~map()
	{
	}

	// copy

	//! Copy-assigns other.
	map& operator=(const map& other)
	{
		MyBase::operator=(other);
		return *this;
	}

	// move

	//! Move-assigns other.
	map& operator=(map&& other) noexcept(
	    noexcept(other.MyBase::operator=(std::move(other))))
	{
		MyBase::operator=(std::move(other));
		return *this;
	}

	// initializer list

	//! Replaces the elements with the elements from the initializer list.
	map& operator=(std::initializer_list<value_type> il)
	{
		MyBase::operator=(il);
		return *this;
	}

	/// @}
	//! @name element access
	/// @{

	// operator[]

	//! Returns a reference to the value mapped to a key equivalent	to @c key,
	//! performing an insertion of a @c value_type(key,mapped_type()) if such
	//! key does not already exist.
	mapped_type& operator[](const key_type& key)
	{
		auto result = this->find_or_upper_bound(key);
		if (result.second) // If found, return a reference to the mapped value.
			return result.first->second;

		// Emplace and return a reference to the mapped value.
		return data_emplace(result.first, key, mapped_type())->second;
	}

	//! Returns a reference to the value mapped to a key equivalent	to @c key,
	//! performing an insertion a @c value_type(std::move(key),mapped_type()) if
	//! such key does not already exist.
	mapped_type& operator[](key_type&& key)
	{
		auto result = this->find_or_upper_bound(key);
		if (result.second) // If found, return a reference to the mapped value.
			return result.first->second;

		// Emplace and return a reference to the mapped value.
		return data_emplace(result.first, std::move(key), mapped_type())
		    ->second;
	}

	// at

	//! Returns a reference to the value mapped to a key equivalent	to @c key,
	//! if such key exists. Otherwise, throws @c std::out_of_range.
	mapped_type& at(const key_type& key)
	{
		auto result = this->find_or_upper_bound(key);
		if (result.second) // If found, return a reference.
			return result.first->second;

		throw std::out_of_range("invalid map<K,T,A,C> key");
	}


	//! Returns a reference to the value mapped to a key equivalent	to @c key,
	//! if such key exists. Otherwise, throws @c std::out_of_range.
	const mapped_type& at(const key_type& key) const
	{
		auto result = this->find_or_upper_bound(key);
		if (result.second) // If found, return a reference.
			return result.first->second;

		throw std::out_of_range("invalid map<K,T,A,C> key");
	}

	/// @}
	//! @name modifiers
	/// @{

	// insert_or_assign

	/*!
	    If the key @c key already exists in the map, assigns @c obj to the
	   mapped
	    value.
	    Otherwise, inserts a @c value_type constructed using @c key and @c args.
	     - Returns a pair:
	        first: the iterator to the element (just inserted or already there).
	        second: true if the insertion took place.
	*/
	template <class M>
	std::pair<iterator, bool> insert_or_assign(const key_type& key, M&& obj)
	{
		auto result = this->find_or_upper_bound(key);
		if (result.second) // If found, assign, return the iterator and false.
		{
			(*result.first).second = std::forward<M>(obj);
			return std::make_pair(result.first, false);
		}

		// Otherwise, return the emplaced value's iterator and true.
		return std::make_pair(data_emplace(result.first, key, std::move(obj)),
		                      true);
	}

	//! @c insert_or_assign(key,obj) overload that takes an rvalue key.
	template <class M>
	std::pair<iterator, bool> insert_or_assign(key_type&& key, M&& obj)
	{
		auto result = this->find_or_upper_bound(key);
		if (result.second) // If found, assign, return the iterator and false.
		{
			(*result.first).second = std::forward<M>(obj);
			return std::make_pair(result.first, false);
		}

		// Otherwise, return the emplaced value's iterator and true.
		return std::make_pair(data_emplace(result.first, std::move(key),
		                                   std::move(obj)),
		                      true);
	}

	/*!
	    If the key @c key already exists in the map, assigns @c obj to the
	   mapped
	    value.
	    Otherwise, inserts a @c value_type constructed using @c key and @c args.
	     - Returns the iterator to the element (just inserted or already there).
	     - Uses the hint to find the insertion place (see @c emplace_hint).
	*/
	template <class M>
	iterator insert_or_assign(const_iterator hint, const key_type& key, M&& obj)
	{
		auto result = this->find_or_upper_bound_hint(hint, key);
		if (result.second) // If found, assign, return the iterator.
		{
			(*result.first).second = std::forward<M>(obj);
			return result.first;
		}

		// Otherwise, return the emplaced value's iterator.
		return data_emplace(result.first, key, std::move(obj));
	}

	//! @c insert_or_assign(hint,key,obj) overload that takes an rvalue key.
	template <class M>
	iterator insert_or_assign(const_iterator hint, key_type&& key, M&& obj)
	{
		auto result = this->find_or_upper_bound_hint(hint, key);
		if (result.second) // If found, assign, return the iterator.
		{
			(*result.first).second = std::forward<M>(obj);
			return result.first;
		}

		// Otherwise, return the emplaced value's iterator.
		return data_emplace(result.first, std::move(key), std::move(obj));
	}

	// try_emplace

	/*!
	    If the key @c key already exists in the map, does nothing.
	    Otherwise, inserts a @c value_type constructed using @c key and @c args.
	     - Returns a pair:
	        first: the iterator to the element (just inserted or already there).
	        second: true if the insertion took place.
	*/
	template <class... Args>
	std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args)
	{
		auto result = this->find_or_upper_bound(key);
		if (result.second) // If found, return the iterator and false.
			return std::make_pair(result.first, false);

		// Otherwise, return the emplaced value's iterator and true.
		return std::make_pair(data_emplace(result.first, key,
		                                   std::forward<Args>(args)...),
		                      true);
	}

	//! @c try_emplace(key,args) overload that takes an rvalue key.
	template <class... Args>
	std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args)
	{
		auto result = this->find_or_upper_bound(key);
		if (result.second) // If found, return the iterator and false.
			return std::make_pair(result.first, false);

		// Otherwise, return the emplaced value's iterator and true.
		return std::make_pair(data_emplace(result.first, std::move(key),
		                                   std::forward<Args>(args)...),
		                      true);
	}

	/*!
	    If the key @c key already exists in the map, does nothing.
	    Otherwise, inserts a @c value_type constructed using @c key and @c args.
	     - Returns the iterator to the element (just inserted or already there).
	     - Uses the hint to find the insertion place (see @c emplace_hint).
	*/
	template <class... Args>
	iterator try_emplace(const_iterator hint,
	                     const key_type& key,
	                     Args&&... args)
	{
		auto result = this->find_or_upper_bound_hint(hint, key);
		if (result.second) // If found, return the iterator.
			return result.first;

		// Otherwise, return the emplaced value's iterator.
		return data_emplace(result.first, key, std::forward<Args>(args)...);
	}


	//! @c try_emplace(hint,key,args) overload that takes an rvalue key.
	template <class... Args>
	iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args)
	{
		auto result = this->find_or_upper_bound_hint(hint, key);
		if (result.second) // If found, return the iterator.
			return result.first;

		// Otherwise, return the emplaced value's iterator.
		return data_emplace(result.first, std::move(key),
		                    std::forward<Args>(args)...);
	}

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
	//! http://lists.llvm.org/pipermail/cfe-commits/Week-of-Mon-20150427/128182.html
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

protected:
	//! Emplace an element before @c pos and return its @c iterator.
	template <class... Args>
	iterator data_emplace(const_iterator pos,
	                      const key_type& key,
	                      Args&&... args)
	{
		return iterator(
		    this->data
		        .emplace(static_cast<
		                     typename impl_container_type::const_iterator>(pos),
		                 std::piecewise_construct, std::forward_as_tuple(key),
		                 std::forward_as_tuple(std::forward<Args>(args)...)));
	}

	//! Emplace an element before @c pos and return its @c iterator.
	template <class... Args>
	iterator data_emplace(const_iterator pos, key_type&& key, Args&&... args)
	{
		return iterator(
		    this->data
		        .emplace(static_cast<
		                     typename impl_container_type::const_iterator>(pos),
		                 std::piecewise_construct,
		                 std::forward_as_tuple(std::move(key)),
		                 std::forward_as_tuple(std::forward<Args>(args)...)));
	}

	// friend operators

	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator==(const map<K, M, P, A, C>&,
	                       const map<K, M, P, A, C>&);
	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator!=(const map<K, M, P, A, C>&,
	                       const map<K, M, P, A, C>&);
	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator<(const map<K, M, P, A, C>&, const map<K, M, P, A, C>&);
	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator<=(const map<K, M, P, A, C>&,
	                       const map<K, M, P, A, C>&);
	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator>(const map<K, M, P, A, C>&, const map<K, M, P, A, C>&);
	template <class K,
	          class M,
	          class P,
	          class A,
	          template <class, class> class C>
	friend bool operator>=(const map<K, M, P, A, C>&,
	                       const map<K, M, P, A, C>&);
};

//! equality
template <class K, class M, class P, class A, template <class, class> class C>
bool operator==(const map<K, M, P, A, C>& lhs, const map<K, M, P, A, C>& rhs)
{
	return lhs.data == rhs.data;
}

//! inequality
template <class K, class M, class P, class A, template <class, class> class C>
bool operator!=(const map<K, M, P, A, C>& lhs, const map<K, M, P, A, C>& rhs)
{
	return lhs.data != rhs.data;
}

//! less than
template <class K, class M, class P, class A, template <class, class> class C>
inline bool operator<(const map<K, M, P, A, C>& lhs,
                      const map<K, M, P, A, C>& rhs)
{
	return lhs.data < rhs.data;
}

//! less or equal to
template <class K, class M, class P, class A, template <class, class> class C>
inline bool operator<=(const map<K, M, P, A, C>& lhs,
                       const map<K, M, P, A, C>& rhs)
{
	return lhs.data <= rhs.data;
}

//! greater than
template <class K, class M, class P, class A, template <class, class> class C>
inline bool operator>(const map<K, M, P, A, C>& lhs,
                      const map<K, M, P, A, C>& rhs)
{
	return lhs.data > rhs.data;
}

//! greater or equal to
template <class K, class M, class P, class A, template <class, class> class C>
inline bool operator>=(const map<K, M, P, A, C>& lhs,
                       const map<K, M, P, A, C>& rhs)
{
	return lhs.data >= rhs.data;
}

//! swap
template <class K, class M, class P, class A, template <class, class> class C>
inline void swap(map<K, M, P, A, C>& lhs,
                 map<K, M, P, A, C>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
	lhs.swap(rhs);
}
}
