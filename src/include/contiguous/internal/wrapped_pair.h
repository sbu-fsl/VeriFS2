#pragma once

/**
    @file contiguous_settings.h
    @brief Move-assignable wrapper for std::pair<const T,M>.
*/

namespace contiguous
{
namespace internal
{
	/*!
	    Wrapper for a std::pair that allows for it to be stored in a
	    std::vector, even if one or both of the member types are not
	    assignable (e.g. const-qualified).

	    Requires:
	        std::is_move_constructible<Key>::value &&
	        std::is_move_constructible<Mapped>::value

	    This means that if Key = const K, then
	        std::is_constructible<K,const K&&>::value
	    must be true, i.e. it has to have one of the following constructors:
	    K(const K&), K(const K&&)
	*/
	template <class Key,
	          class Mapped,
	          bool = std::is_copy_constructible<std::pair<Key, Mapped>>::value>
	class wrapped_pair
	{
	protected:
		using T1 = Key; // stored pair types
		using T2 = Mapped;

		using value_type = std::pair<T1, T2>;

		// stored pair
		std::aligned_storage_t<sizeof(value_type), alignof(value_type)> p;

	public:
		static_assert(
		    std::is_move_constructible<T1>::value,
		    "Possibly const-qualified key type must be move constructible.");
		static_assert(
		    std::is_move_constructible<T2>::value,
		    "Possibly const-qualified mapped type must be move constructible.");

		// constructors

		// copy constructor
		wrapped_pair(const wrapped_pair& v) noexcept(
		    std::is_nothrow_copy_constructible<value_type>::value)
		{
			new (&p) value_type(v.get());
		}

		// move constructor
		wrapped_pair(wrapped_pair&& v) noexcept(
		    std::is_nothrow_move_constructible<value_type>::value)
		{
			new (&p)
			    value_type(std::move(v.get().first), std::move(v.get().second));
		}

		// default constructor
		template <class Dummy = void,
		          class = std::enable_if_t<
		              std::is_void<Dummy>::value &&
		              std::is_default_constructible<value_type>::value>>
		constexpr wrapped_pair() noexcept(
		    std::is_nothrow_default_constructible<value_type>::value)
		{
			new (&p) value_type();
		}

		// construct by copying member-wise
		template <
		    class Dummy = void,
		    class = std::enable_if_t<std::is_void<Dummy>::value &&
		                             std::is_copy_constructible<T1>::value &&
		                             std::is_copy_constructible<T2>::value>>
		constexpr wrapped_pair(const T1& x, const T2& y) noexcept(
		    std::is_nothrow_constructible<value_type, T1&, T2&>::value)
		{
			new (&p) value_type(x, y);
		}

		// construct by forwarding member-wise
		template <
		    class U,
		    class V,
		    class = std::enable_if_t<std::is_constructible<T1, U&&>::value &&
		                             std::is_constructible<T2, V&&>::value &&
		                             std::is_convertible<U, T1>::value &&
		                             std::is_convertible<V, T2>::value>>
		constexpr wrapped_pair(U&& x, V&& y) noexcept(
		    std::is_nothrow_constructible<value_type, U&&, V&&>::value)
		{
			new (&p) value_type(std::forward<U>(x), std::forward<V>(y));
		}

		// initializes members from the corresponding members of the argument
		template <class U,
		          class V,
		          class = std::enable_if_t<
		              std::is_constructible<T1, const U&>::value &&
		              std::is_constructible<T2, const V&>::value &&
		              std::is_convertible<const U&, T1>::value &&
		              std::is_convertible<const V&, T2>::value>>
		constexpr wrapped_pair(const std::pair<U, V>& other) noexcept(
		    std::is_nothrow_constructible<value_type,
		                                  const std::pair<U, V>&>::value)
		{
			new (&p) value_type(other);
		}

		// initializes members by forwarding the corresponding members of the
		// argument
		template <
		    class U,
		    class V,
		    class = std::enable_if_t<std::is_constructible<T1, U&&>::value &&
		                             std::is_constructible<T2, V&&>::value &&
		                             std::is_convertible<U, T1>::value &&
		                             std::is_convertible<V, T2>::value>>
		constexpr wrapped_pair(std::pair<U, V>&& other) noexcept(
		    std::is_nothrow_constructible<value_type, std::pair<U, V>&&>::value)
		{
			new (&p) value_type(std::move(other));
		}

		// piecewise construction
		template <class... Args1,
		          class... Args2,
		          class = std::enable_if_t<
		              std::is_constructible<T1, Args1&&...>::value &&
		              std::is_constructible<T2, Args2&&...>::value>>
		wrapped_pair(
		    std::piecewise_construct_t,
		    std::tuple<Args1...> first_args,
		    std::tuple<Args2...>
		        second_args) noexcept(std::
		                                  is_nothrow_constructible<
		                                      value_type,
		                                      std::piecewise_construct_t,
		                                      std::tuple<Args1...>&&,
		                                      std::tuple<Args2...>&&>::value)
		{
			new (&p) value_type(std::piecewise_construct, std::move(first_args),
			                    std::move(second_args));
		}

		// destructor

		~wrapped_pair()
		{
			get().~value_type();
		}

		// operators

		wrapped_pair& operator=(const wrapped_pair& other) noexcept(
		    noexcept(other.get()) &&
		    std::is_nothrow_destructible<value_type>::value &&
		    std::is_nothrow_copy_constructible<value_type>::value)
		{
			if (&other == this) // self-assignment check
				return *this;

			// destruct p and copy-construct from other.p
			get().~value_type();
			new (&p) value_type(other.get());

			return *this;
		}

		wrapped_pair& operator=(wrapped_pair&& other) noexcept(
		    noexcept(other.get()) &&
		    std::is_nothrow_destructible<value_type>::value &&
		    std::is_nothrow_move_constructible<value_type>::value)
		{
			if (&other == this) // self-assignment check
				return *this;

			// destruct p and move-construct from other.p
			get().~value_type();
			new (&p) value_type(std::move(other.get()));

			return *this;
		}

		// get the reference to the wrapped pair
		const value_type& get() const noexcept
		{
			return *reinterpret_cast<const value_type*>(&p);
		}

		// get the reference to the wrapped pair
		value_type& get() noexcept
		{
			return *reinterpret_cast<value_type*>(&p);
		}

		// swap

		void swap(wrapped_pair& other) noexcept(noexcept(std::swap(other,
		                                                           other)))
		{
			std::swap(*this, other);
		}

		template <class K, class M>
		friend constexpr bool operator==(const wrapped_pair<K, M>& x,
		                                 const wrapped_pair<K, M>& y);
		template <class K, class M>
		friend constexpr bool operator<(const wrapped_pair<K, M>& x,
		                                const wrapped_pair<K, M>& y);
		template <class K, class M>
		friend constexpr bool operator!=(const wrapped_pair<K, M>& x,
		                                 const wrapped_pair<K, M>& y);
		template <class K, class M>
		friend constexpr bool operator>(const wrapped_pair<K, M>& x,
		                                const wrapped_pair<K, M>& y);
		template <class K, class M>
		friend constexpr bool operator>=(const wrapped_pair<K, M>& x,
		                                 const wrapped_pair<K, M>& y);
		template <class K, class M>
		friend constexpr bool operator<=(const wrapped_pair<K, M>& x,
		                                 const wrapped_pair<K, M>& y);
	};
	///*
	template <class Key, class Mapped>
	class wrapped_pair<Key, Mapped, false>
	    : public wrapped_pair<Key, Mapped, true>
	{
	protected:
		using MyBase = wrapped_pair<Key, Mapped, true>;

		using typename MyBase::T1;
		using typename MyBase::T2;

		using typename MyBase::value_type;

	public:
		using MyBase::wrapped_pair;

		// default constructor
		template <class Dummy = void,
		          class = std::enable_if_t<
		              std::is_void<Dummy>::value &&
		              std::is_default_constructible<value_type>::value>>
		constexpr wrapped_pair() noexcept(
		    std::is_nothrow_default_constructible<value_type>::value)
		{
			new (&(this->p)) value_type();
		}

		wrapped_pair(const wrapped_pair&) = delete;
		wrapped_pair(wrapped_pair&&) = default;

		wrapped_pair& operator=(const wrapped_pair&) = delete;
		wrapped_pair& operator=(wrapped_pair&&) = default;
	};
	//*/
	template <class K, class M>
	constexpr bool operator==(const wrapped_pair<K, M>& x,
	                          const wrapped_pair<K, M>& y)
	{
		return x.get() == y.get();
	}

	template <class K, class M>
	constexpr bool operator<(const wrapped_pair<K, M>& x,
	                         const wrapped_pair<K, M>& y)
	{
		return x.get() < y.get();
	}

	template <class K, class M>
	constexpr bool operator!=(const wrapped_pair<K, M>& x,
	                          const wrapped_pair<K, M>& y)
	{
		return x.get() != y.get();
	}
	template <class K, class M>
	constexpr bool operator>(const wrapped_pair<K, M>& x,
	                         const wrapped_pair<K, M>& y)
	{
		return x.get() > y.get();
	}
	template <class K, class M>
	constexpr bool operator>=(const wrapped_pair<K, M>& x,
	                          const wrapped_pair<K, M>& y)
	{
		return x.get() >= y.get();
	}

	template <class K, class M>
	constexpr bool operator<=(const wrapped_pair<K, M>& x,
	                          const wrapped_pair<K, M>& y)
	{
		return x.get() <= y.get();
	}

	template <class K, class M>
	void swap(wrapped_pair<K, M>& x,
	          wrapped_pair<K, M>& y) noexcept(noexcept(x.swap(y)))
	{
		x.swap(y);
	}
}
}