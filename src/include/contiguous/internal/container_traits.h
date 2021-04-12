#pragma once

/**
    @file container_traits.h
    @brief Traits - packed container template parameters.
*/

namespace contiguous
{
namespace internal
{
	// Traits for set and multiset.
	template <class KeyType,
	          class CompareType,
	          class AllocType,
	          template <class V, class A> class ContainerType>
	class set_traits
	{
	public:
		using Key = KeyType;

		using Container = ContainerType<KeyType, AllocType>;
		using ImplContainer = Container;

		using Compare = CompareType;
		using ValueCompare = Compare;

		using ConstIterator = typename Container::const_iterator;
		using Iterator = ConstIterator;
	};


	// Traits for map and multimap.
	template <class KeyType,
	          class T,
	          class CompareType,
	          class AllocType,
	          template <class V, class A> class ContainerType>
	class map_traits
	{
	private:
		// const-qualify key type

		using QualifiedKeyType = qualify_key<KeyType>;

		// get value types

		using Value = typename std::pair<QualifiedKeyType, T>;
		using ImplValue = wrapped_pair<QualifiedKeyType, T>;

		// get alloc types

		using Alloc = typename std::allocator_traits<
		    AllocType>::template rebind_alloc<Value>;
		using ImplAlloc = typename std::allocator_traits<
		    Alloc>::template rebind_alloc<ImplValue>;

		// get pointer type

		using Pointer = typename std::allocator_traits<Alloc>::pointer;
		using ConstPointer =
		    typename std::allocator_traits<Alloc>::const_pointer;

	public:
		using Key = KeyType;

		using Container = ContainerType<Value, Alloc>;
		using ImplContainer = ContainerType<ImplValue, ImplAlloc>;

		using Compare = CompareType;
		using ValueCompare = wrapped_pair_compare<QualifiedKeyType, T, Compare>;

		using ConstIterator =
		    wrapped_pair_const_iterator<typename ImplContainer::const_iterator,
		                                Value,
		                                ConstPointer>;

		using Iterator =
		    wrapped_pair_iterator<typename ImplContainer::iterator,
		                          typename ImplContainer::const_iterator,
		                          Value,
		                          Pointer,
		                          ConstIterator>;
	};
}
}