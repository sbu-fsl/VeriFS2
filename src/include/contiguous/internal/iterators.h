#pragma once

/**
    @file iterators.h
    @brief Mutable and immutable iterators for contiguous containers.
*/

namespace contiguous
{
namespace internal
{

	// immutable iterator

	//! A random access iterator to @c const @c ValueType
	//! To obtain a Value&, it calls (*[wrapped iterator]).get().
	template <class ImplConstIterator, class ValueType, class Pointer>
	class wrapped_pair_const_iterator
	{
	protected:
		using impl_const_iterator = ImplConstIterator;
		impl_const_iterator it;

	public:
		using iterator_category = typename ImplConstIterator::iterator_category;
		using value_type = ValueType;
		using difference_type = typename ImplConstIterator::difference_type;
		using pointer = Pointer;
		using reference = const ValueType&;

		// empty iterator
		wrapped_pair_const_iterator()
		{
		}

		// from container iterator
		explicit wrapped_pair_const_iterator(impl_const_iterator it_in)
		  : it(it_in)
		{
		}

		// return reference
		reference operator*() const
		{
			return it->get(); // return the wrapped pair
		}

		// return pointer
		pointer operator->() const
		{
			return std::pointer_traits<pointer>::pointer_to(**this);
		}

		// preincrement
		wrapped_pair_const_iterator& operator++()
		{
			++it;
			return *this;
		}

		// postincrement
		wrapped_pair_const_iterator operator++(int)
		{
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		// predecrement
		wrapped_pair_const_iterator& operator--()
		{
			--it;
			return *this;
		}

		// postdecrement
		wrapped_pair_const_iterator operator--(int)
		{
			auto tmp = *this;
			--(*this);
			return tmp;
		}

		// add offset
		wrapped_pair_const_iterator& operator+=(difference_type offset)
		{
			it += offset;
			return *this;
		}

		// return this + offset
		wrapped_pair_const_iterator operator+(difference_type offset) const
		{
			auto tmp = *this;
			return tmp += offset;
		}

		// add -offset
		wrapped_pair_const_iterator& operator-=(difference_type offset)
		{
			return (*this) += (-offset);
		}

		// return this - offset
		wrapped_pair_const_iterator operator-(difference_type offset) const
		{
			auto tmp = *this;
			return tmp -= offset;
		}

		// difference of iterators
		difference_type operator-(
		    const wrapped_pair_const_iterator& right) const
		{
			return it - right.it;
		}

		// offset dereference
		reference operator[](difference_type offset) const
		{
			return *(*this + offset);
		}

		// equality
		bool operator==(const wrapped_pair_const_iterator& right) const
		{
			return it == right.it;
		}

		// inequality
		bool operator!=(const wrapped_pair_const_iterator& right) const
		{
			return it != right.it;
		}

		// less than
		bool operator<(const wrapped_pair_const_iterator& right) const
		{
			return it < right.it;
		}

		// greater than
		bool operator>(const wrapped_pair_const_iterator& right) const
		{
			return it > right.it;
		}

		// less or equal to
		bool operator<=(const wrapped_pair_const_iterator& right) const
		{
			return it <= right.it;
		}

		// greater or equal to
		bool operator>=(const wrapped_pair_const_iterator& right) const
		{
			return it >= right.it;
		}

		// convert to container const iterator
		explicit operator impl_const_iterator() const
		{
			return it;
		}
	};

	// mutable iterator

	//! A random access iterator to @c const @c ValueType.
	//! To obtain a ValueType&, it calls (*[wrapped iterator]).get().
	template <class ImplIterator,
	          class ImplConstIterator,
	          class ValueType,
	          class Pointer,
	          class ConstIterator>
	class wrapped_pair_iterator
	{
	protected:
		using impl_iterator = ImplIterator;
		using impl_const_iterator = ImplConstIterator;

		using const_iterator = ConstIterator;

		impl_iterator it;

	public:
		using iterator_category = typename ImplIterator::iterator_category;
		using value_type = ValueType;
		using difference_type = typename ImplIterator::difference_type;
		using pointer = Pointer;
		using reference = ValueType&;

		// empty iterator
		wrapped_pair_iterator()
		{
		}

		// from container iterator
		explicit wrapped_pair_iterator(impl_iterator it_in) : it(it_in)
		{
		}

		// return reference
		reference operator*()
		{
			return it->get(); // return the wrapped object reference
		}

		// return pointer
		pointer operator->()
		{
			return std::pointer_traits<pointer>::pointer_to(**this);
		}

		// preincrement
		wrapped_pair_iterator& operator++()
		{
			++it;
			return *this;
		}

		// postincrement
		wrapped_pair_iterator operator++(int)
		{
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		// predecrement
		wrapped_pair_iterator& operator--()
		{
			--it;
			return *this;
		}

		// postdecrement
		wrapped_pair_iterator operator--(int)
		{
			auto tmp = *this;
			--(*this);
			return tmp;
		}

		// add offset
		wrapped_pair_iterator& operator+=(difference_type offset)
		{
			it += offset;
			return *this;
		}

		// return this + offset
		wrapped_pair_iterator operator+(difference_type offset) const
		{
			auto tmp = *this;
			return tmp += offset;
		}

		// add -offset
		wrapped_pair_iterator& operator-=(difference_type offset)
		{
			return (*this) += (-offset);
		}

		// return this - offset
		wrapped_pair_iterator operator-(difference_type offset) const
		{
			auto tmp = *this;
			return tmp -= offset;
		}

		// difference of iterators
		difference_type operator-(const wrapped_pair_iterator& right) const
		{
			return it - right.it;
		}

		// offset dereference
		reference operator[](difference_type offset) const
		{
			return *(*this + offset);
		}

		// equality
		bool operator==(const wrapped_pair_iterator& right) const
		{
			return it == right.it;
		}

		// inequality
		bool operator!=(const wrapped_pair_iterator& right) const
		{
			return it != right.it;
		}

		// less than
		bool operator<(const wrapped_pair_iterator& right) const
		{
			return it < right.it;
		}

		// greater than
		bool operator>(const wrapped_pair_iterator& right) const
		{
			return it > right.it;
		}

		// less or equal to
		bool operator<=(const wrapped_pair_iterator& right) const
		{
			return it <= right.it;
		}

		// greater or equal to
		bool operator>=(const wrapped_pair_iterator& right) const
		{
			return it >= right.it;
		}


		// equality to const iterator
		bool operator==(const const_iterator& right) const
		{
			return it == impl_const_iterator(right);
		}

		// inequality to const iterator
		bool operator!=(const const_iterator& right) const
		{
			return it != impl_const_iterator(right);
		}

		// less than const iterator
		bool operator<(const const_iterator& right) const
		{
			return it < impl_const_iterator(right);
		}
		// greater than const iterator
		bool operator>(const const_iterator& right) const
		{
			return it > impl_const_iterator(right);
		}

		// less or equal to const iterator
		bool operator<=(const const_iterator& right) const
		{
			return it <= impl_const_iterator(right);
		}

		// greater or equal to const iterator
		bool operator>=(const const_iterator& right) const
		{
			return it >= impl_const_iterator(right);
		}


		// convert to container iterator
		explicit operator impl_iterator() const
		{
			return it;
		}

		// convert to container iterator
		explicit operator impl_const_iterator() const
		{
			return impl_const_iterator(it);
		}

		// convert to const_iterator
		operator const_iterator() const
		{
			return const_iterator(impl_const_iterator(it));
		}
	};
}
}