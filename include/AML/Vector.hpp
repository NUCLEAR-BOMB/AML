/** @file */
#pragma once

#include <AML/Iterator.hpp>
#include <AML/Tools.hpp>
#include <AML/MathFunctions.hpp>

#include <cstddef>
#include <type_traits>
#include <algorithm>
#include <vector>
#include <utility>

#if AML_CXX20
#include <concepts>
#endif

#ifdef AML_LIBRARY
	#define AML_LIBRARY_VECTOR
#else
	#error AML library is required
#endif

namespace aml 
{

/**
	@brief The main alias for vector's size type
	@details More created for code readability

	@see Vector
*/
using Vectorsize = std::size_t;

/**
	@brief Namespace for compile-time vector indexes
	@details Use them to access vector's fields from #Vector::operator[](const VI::index<I>) and #Vector::operator[](const VI::index<I>) const

	@see Vector<T, Size> @n
			Vector<Container, dynamic_extent>
*/
namespace VI {
	/**
		@brief Template type alias to access the vector field at compile time

		@tparam I Index of vector field number
	*/
	template<Vectorsize I>
	using index = std::integral_constant<Vectorsize, I>;

	inline constexpr index<0> X{}; ///< @c x 1D variable using indexes
	inline constexpr index<1> Y{}; ///< @c y 2D variable using indexes
	inline constexpr index<2> Z{}; ///< @c z 3D variable using indexes
	inline constexpr index<3> W{}; ///< @c w 4D variable using indexes
	inline constexpr index<4> V{}; ///< @c v 5D variable using indexes

	inline constexpr index<0> first{}; ///< Index of first vector element
} // namespace VI

namespace detail 
{
	template<class T, Vectorsize S>
	struct VectorStorage {
	protected:
		T array[S];
	};

	template<class T>
	struct VectorStorage<T, 1> {
		T x;
	};

	template<class T>
	struct VectorStorage<T, 2> {
		T x, y;
	};

	template<class T>
	struct VectorStorage<T, 3> {
		T x, y, z;
	};

	template<class T>
	struct VectorStorage<T, 4> {
		T x, y, z, w;
	};

	template<class T>
	struct VectorStorage<T, 5> {
		T x, y, z, w, v;
	};

	template<class T, Vectorsize Size>
	struct VectorBase
	{
		static_assert(std::is_object_v<T>, "A vector's T must be an object type");
		static_assert(!std::is_abstract_v<T>, "A vector's T cannot be an abstract class type");
		static_assert(aml::is_complete<T>, "A vector's T must be a complete type");

		using size_type = Vectorsize;
		using value_type = T;

		using reference = T&;
		using const_reference = const T&;

		static constexpr bool is_dynamic() noexcept { return (Size == aml::dynamic_extent); }
		static constexpr bool uses_static_array() noexcept { return (Size > 5) && !is_dynamic(); }
		static constexpr bool has_index(const size_type index) noexcept { return (Size > index); }

		static constexpr Vectorsize extent = aml::dynamic_extent;
	};
} // namespace detail

#if AML_CXX20
	template<class Container>
	concept is_support_dynamic_vector_container = requires(Container& cont, const Container& ccont, const Vectorsize vecsize)
	{
		typename Container::value_type;
		typename Container::size_type;

		typename Container::iterator;
		typename Container::const_iterator;

		typename Container::reference;
		typename Container::const_reference;

		{ cont.size() } -> std::same_as<typename Container::size_type>;
		{ cont.resize(vecsize) };

		{ cont.begin() }	-> std::same_as<typename Container::iterator>;
		{ ccont.begin() }	-> std::same_as<typename Container::const_iterator>;
		{ cont.begin() }    -> std::same_as<typename Container::iterator>;
		{ ccont.end() }		-> std::same_as<typename Container::const_iterator>;

		{ ccont.cbegin() }	-> std::same_as<typename Container::const_iterator>;
		{ ccont.cend() }	-> std::same_as<typename Container::const_iterator>;

		{ cont[vecsize] }	-> std::same_as<typename Container::reference>;
		{ ccont[vecsize] }	-> std::same_as<typename Container::const_reference>;
	}
	&& !aml::is_narrowing_conversion<typename Container::size_type, Vectorsize>;
#endif // AML_CXX20

	/**
		@brief The representation of a vector from linear algebra as a statically allocated template class

		@tparam T The type of the elements
		@tparam Size The static vector size

		@see aml::Vector<Container, dynamic_extent>
	*/
template<class T, Vectorsize Size>
#if AML_CXX20
	//requires (Size != aml::dynamic_extent)
#endif
class Vector /** @cond */: public detail::VectorBase<T, Size>, public detail::VectorStorage<T, Size> /** @endcond */
{
	using Storage = detail::VectorStorage<T, Size>;
	using Base	  =	detail::VectorBase<T, Size>;
public:

	using size_type			= typename Base::size_type;		  ///< The size type that vector uses
	using value_type		= typename Base::value_type;	  ///< The value type that vector uses

	using reference			= typename Base::reference;		  ///< Type that the non-const index operator returns
	using const_reference	= typename Base::const_reference; ///< Type that const index operator returns

	/// The return type of the non-const begin() and end() methods
	using iterator			= std::conditional_t<Base::uses_static_array(),		  T*,	   aml::IndexIterator<Vector<T, Size>>>;

	/// The return type of the const begin(), cbegin() and end(), cend() methods
	using const_iterator	= std::conditional_t<Base::uses_static_array(), const T*, aml::ConstIndexIterator<Vector<T, Size>>>;

public:

	/// Static compile-time variable defining the size of the vector
	static constexpr size_type static_size = Size;

	/**
		@brief Default constructor

		@warning Does not explicitly initialize elements
	*/
	constexpr
	Vector() noexcept {
		
	}

	/**
		@brief Initializes the vector using a static constant array
		@details The size of the input array must match the size of the vector

		@tparam ArraySize Input array size
		@param Array Input array
	*/
	template<std::size_t ArraySize> constexpr
	Vector(const T (&Array)[ArraySize]) noexcept {
		static_assert(ArraySize == Size, 
			"The size of the array does not equal to vector's size");

		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = Array[i];
		});
	}

private:
	template<class... Ts>
	static constexpr bool enable_if_variadic_constructor = (
		!(aml::is_narrowing_conversion<Ts, value_type> || ...)
		&& (sizeof...(Ts) == Size)
	);
public:

	/**
		@brief Initializes the vector using variadic arguments
		@details All variable arguments must be a non-narrowing transformation to a main vector type

		@tparam First,Rest... Variable input variadic templates 
		@param f,r Variadic arguments
	*/
	template<class First, class... Rest, 
		std::enable_if_t<enable_if_variadic_constructor<First, Rest...>, int> = 0
	> constexpr
	explicit Vector(First&& f, Rest&&... r) noexcept 
	{
		(*this)[VI::first] = std::forward<First>(f);

		Vectorsize i = 1;
		([&] {
			(*this)[i] = std::forward<Rest>(r); 
			++i;
		}(), ...);
	}

	/**
		@brief Initializes the vector with 0
		@details To call this constructor overload you need to use @ref zero "aml::zero" as an first input parameter

		@see aml::zero
	*/
	constexpr
	explicit Vector(const aml::zero_t) noexcept {
		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = static_cast<T>(0);
		});
	}

	/**
		@brief Initializes the vector with 1
		@details To call this constructor overload you need to use @ref one "aml::one" as an first input parameter

		@see aml::one
	*/
	constexpr
	explicit Vector(const aml::one_t) noexcept {
		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = static_cast<T>(1);
		});
	}

	/**
		@brief Initializes the unit vector
		@details To call this constructor overload you need to use @ref unit "aml::unit" as an first input parameter

		@see aml::unit
	*/
	template<std::size_t Dir> constexpr 
	explicit Vector(const aml::unit_t<Dir>) noexcept {
		static_assert(Base::has_index(Dir), "Unit must be in vector's range");
		aml::static_for<Size>([&](const auto i) {
			if constexpr (i == Dir) (*this)[i] = static_cast<T>(1);
			else					(*this)[i] = static_cast<T>(0);
		});
	}

	/**
		@brief Cast from Vector<U, Size> to Vector<T, Size>
	*/
	template<class U> constexpr
	explicit Vector(const Vector<U, Size>& other) noexcept {
		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = static_cast<T>(other[i]);
		});
	}

	/**
		@brief Initializes the filled vector 
	*/
	constexpr
	explicit Vector(const aml::fill_initializer<value_type> fill_with) noexcept {
		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = fill_with.value;
		});
	}

	/**
		@brief Initializes the vector from dynamic vector

		@tparam U Value type of dynamic vector
		@param other Input dynamic vector

		@warning The size of the dynamic vector must be the same as the size of the current vector
	
		@see aml::Vector<T, dynamic_extent>
	*/
	template<class U> constexpr
	explicit Vector(const Vector<U, dynamic_extent>& other) noexcept {
		AML_DEBUG_VERIFY(other.size() == Size, "The dynamic vector must have the same size");
		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = static_cast<T>(other[i]);
		});
	}

	constexpr
	Vector(const Vector&) noexcept = default;

	constexpr
	Vector(Vector&&) noexcept = default;

	constexpr
	Vector& operator=(const Vector&) noexcept = default;

	constexpr
	Vector& operator=(Vector&&) noexcept = default;

	/**
		@return Returns size of the vector
	*/
	[[nodiscard]] AML_CONSTEVAL static /** @cond */ AML_FORCEINLINE /** @endcond */
	size_type size() noexcept {
		return Size;
	}

	/**
		@brief Index operator overload to access field in compile-time 
		@details To call this operator overload you need to use \ref VI::index as an first input parameter

		@tparam I Compile-time index to the vector's field

		@return #reference to vector's field

		@see aml::VI namespace
	*/
	template<size_type I> constexpr /** @cond */ AML_FORCEINLINE /** @endcond */
	reference operator[](const VI::index<I>) noexcept {
		if constexpr (Base::uses_static_array()) {
			return Storage::array[I];
		} else {
				 if constexpr (I == 0) return Storage::x;
			else if constexpr (I == 1) return Storage::y;
			else if constexpr (I == 2) return Storage::z;
			else if constexpr (I == 3) return Storage::w;
			else if constexpr (I == 4) return Storage::v;
			AML_UNREACHABLE;
		}
	}

	/**
		@brief Index operator overload to access field in compile-time
		@details To call this operator overload you need to use \ref VI::index as an first input parameter

		@tparam I Compile-time index to the vector's field

		@return #const_reference to vector's field

		@see VI namespace
	*/
	template<size_type I> constexpr /** @cond */ AML_FORCEINLINE /** @endcond */
	const_reference operator[](const VI::index<I>) const noexcept {
		return const_cast<Vector&>(*this)[VI::index<I>{}];
	}

	/**
		@brief Index operator overload to access the vector's field in runtime

		@param index Runtime index for the numbering of the vector field

		@return #reference to vector's field

		@see operator[](const VI::index<I>)
	*/
	constexpr
	reference operator[](const size_type index) noexcept {
		if constexpr (Base::uses_static_array()) {
			return Storage::array[index];
		} else {
			if constexpr (Base::has_index(0)) if (index == 0) return Storage::x;
			if constexpr (Base::has_index(1)) if (index == 1) return Storage::y;
			if constexpr (Base::has_index(2)) if (index == 2) return Storage::z;
			if constexpr (Base::has_index(3)) if (index == 3) return Storage::w;
			if constexpr (Base::has_index(4)) if (index == 4) return Storage::v;
			AML_UNREACHABLE;
		}
	}

	/**
		@brief Index operator overload to access the vector's field in runtime

		@param index Runtime index for the numbering of the vector field

		@return #const_reference to vector's field

		@see operator[](const VI::index<I>) const
	*/
	constexpr
	const_reference operator[](const size_type index) const noexcept {
		return const_cast<Vector&>(*this)[index];
	}

	/**
		@return Begin #iterator
	*/
	[[nodiscard]] constexpr
	iterator begin() noexcept {
		if constexpr (Base::uses_static_array()) {
			return Storage::array;
		} else {
			return iterator(*this, 0);
		}
	}

	/**
		@return End #iterator
	*/
	[[nodiscard]] constexpr
	iterator end() noexcept {
		if constexpr (Base::uses_static_array()) {
			return Storage::array + size();
		} else {
			return iterator(*this, size());
		}
	}

	/**
		@return Begin #const_iterator
	*/
	[[nodiscard]] constexpr
	const_iterator begin() const noexcept {
		if constexpr (Base::uses_static_array()) {
			return Storage::array;
		} else {
			return const_iterator(*this, 0);
		}
	}

	/**
		@return End #const_iterator
	*/
	[[nodiscard]] constexpr
	const_iterator end() const noexcept {
		if constexpr (Base::uses_static_array()) {
			return Storage::array + size();
		} else {
			return const_iterator(*this, size());
		}
	}

	/**
		@return Begin #const_iterator
	*/
	[[nodiscard]] constexpr
	const_iterator cbegin() const noexcept {
		return begin();
	}

	/**
		@return End #const_iterator
	*/
	[[nodiscard]] constexpr
	const_iterator cend() const noexcept {
		return end();
	}

}; // class Vector<T, Size>
/**
	@example Vector.cpp
	
	An example of basic use of @ref aml::Vector<T, Size>
*/


/**
	@brief The representation of a vector from linear algebra as a dynamically allocated template class
	@details
	@par Requirement for @p Container:
		<b>The @p Container must have this template layout:</b>
		@code
			template<template<class T, class... Parameters> class Container>
		@endcode
		@n
		@c T - vector's value type @n
		@c Parameters... - Additional parameters
		@warning 
				@c T must not affect additional container parameters. @n
				This also includes default parameters that use <tt>T</tt>. @n
				For example, @c std::vector will not work because it has a second template parameter that uses its @c T
		@par
		<b>An example of a container structure:</b>
		@code
			template<class T, class... Parameters>
			struct Container
			{
				using value_type	  = T;
				using size_type		  = ... ;

				using iterator		  = ... ;
				using const_iterator  = ... ;

				using reference		  = ... ;
				using const_reference = ... ;

				size_type size();
				void resize(size_type new_size);

				iterator begin();
				iterator end();

				const_iterator begin() const;
				const_iterator end() const;

				const_iterator cbegin() const;
				const_iterator cend() const;

				reference operator[](size_type index);
				const_reference operator[](size_type index) const;
			};
		@endcode

	@tparam Container The type of container that will be used

	@see @ref aml::Vector<T, Size> @n
		 aml::dynamic_extent
*/

#if AML_CXX20
template<aml::is_support_dynamic_vector_container Container>
#else // !AML_CXX20
template<class Container>
#endif // !AML_CXX20
class Vector<Container, dynamic_extent> /** @cond */: public detail::VectorBase<aml::get_value_type<Container>, dynamic_extent> /** @endcond */
{
private:
	friend class Vector;

	using Base = detail::VectorBase<aml::get_value_type<Container>, dynamic_extent>;
public:

	/**
		@brief The type of container that uses the vector
	*/
	using container_type = Container;

	using size_type			= std::common_type_t<typename Base::size_type, typename container_type::size_type>; ///< The size type that vector uses
	using value_type		= typename Base::value_type;		///< The value type that vector uses

	using reference			= typename container_type::reference;			///< Type that the non-const index operator returns
	using const_reference	= typename container_type::const_reference;	///< Type that const index operator returns

	/// The return type of the non - const begin() and end() methods
	using iterator			= typename container_type::iterator;

	/// The return type of the const begin(), cbegin() and end(), cend() methods
	using const_iterator	= typename container_type::const_iterator;

	//static_assert(std::is_default_constructible_v<container_type>,	"Container must be default constructible");
	//static_assert(aml::has_container_structure<container_type>,		"Container must have a container structure");

public:

	/**
		@brief Default constructor
		@details The container type must also have a default constructor

		@warning Does not explicitly initialize elements
	*/
	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	Vector() noexcept {

	}

	/**
		@brief Initializes the vector using a static constant array
		@details The vector will have the size of the input array

		@tparam ArraySize Input array size
		@param Array Input array
	*/
	template<std::size_t ArraySize> /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	Vector(const value_type (&Array)[ArraySize]) noexcept 
		: Vector(aml::size_initializer(ArraySize)) {
		std::copy_n(Array, ArraySize, this->container.begin());
	}

private:
	template<class... Ts>
	static constexpr bool enable_if_variadic_constructor = (
		!(aml::is_narrowing_conversion<Ts, value_type> || ...)
	);
public:

	/**
		@brief Initializes the vector using variadic arguments
		@details All variable arguments must be a non-narrowing transformation to a main vector type @n
				 The vector will have the size of the number of variadic arguments

		@tparam First,Rest... Variable input variadic templates
		@param f,r Variadic arguments
	*/
	template<class First, class... Rest, 
		std::enable_if_t<enable_if_variadic_constructor<First, Rest...>, int> = 0
	> /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	explicit Vector(First&& f, Rest&&... r) noexcept 
		: Vector(aml::size_initializer(1 + sizeof...(r))) 
	{
		container[0] = std::forward<First>(f);

		Vectorsize i = 1;
		// fold expression
		((container[i++] = std::forward<Rest>(r)), ...);
	}

	/**
		@brief Creates the vector with size

		@warning Does not explicitly initialize elements

		@param initsz The size that will be used to create the vector

		@see aml::size_initializer
	*/
	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	explicit Vector(const aml::size_initializer initsz) noexcept {
		this->container.resize(initsz.size);
	}

	/**
		@brief Creates the vector with size and fills it with 0

		@see aml::zero
	*/
	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	explicit Vector(const aml::size_initializer initsz, const aml::zero_t) noexcept 
		: Vector(initsz, aml::fill_initializer<value_type>(static_cast<value_type>(0))) {
	}

	/**
		@brief Creates the vector with size and fills it with 1

		@see aml::one
	*/
	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
		explicit Vector(const aml::size_initializer initsz, const aml::one_t) noexcept
		: Vector(initsz, aml::fill_initializer<value_type>(static_cast<value_type>(1))) {
	}

	/**
		@brief Creates the unit vector

		@see aml::unit
	*/
	template<std::size_t Dir> /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	explicit Vector(const aml::size_initializer initsz, const aml::unit_t<Dir>) noexcept
		: Vector(initsz, aml::zero) {
		this->container[Dir] = static_cast<value_type>(1);
	}

	/**
		@brief Creates a vector with size and fills it

		@param initsz The size that will be used to create the vector
		@param fill_with Size and what the vector will be filled with

		@see aml::fill_initializer
	*/
	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	explicit Vector(const aml::size_initializer initsz, const aml::fill_initializer<value_type> fill_with) noexcept 
		: Vector(initsz) {
		std::fill(this->container.begin(), this->container.end(), fill_with.value);
	}

	/**
		@brief Cast from Vector<U, dynamic_extent> to Vector<T, dynamic_extent>
	*/
	template<class U> /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	explicit Vector(const Vector<U, aml::dynamic_extent>& other) noexcept 
		: Vector(aml::size_initializer(other.size())) 
	{
		using other_value_type = aml::get_value_type<decltype(other)>;

		std::transform(other.container.cbegin(), other.container.cend(), this->container.begin(),
			[](const other_value_type& val) {
				return static_cast<value_type>(val);
			}
		);
	}

	/**
		@brief Creates dynamically allocated vector from statically allocated vector

		@tparam OtherSize The size of the static allocated vector at compile-time
	*/
	template<class U, Vectorsize OtherSize> /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	explicit Vector(const Vector<U, OtherSize>& other) noexcept
		: Vector(aml::size_initializer(other.static_size))
	{
		using other_value_type = aml::get_value_type<decltype(other)>;

		std::transform(other.cbegin(), other.cend(), this->container.begin(),
			[](const other_value_type& val) {
				return static_cast<value_type>(val);
			}
		);
	}

	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	Vector(const Vector&) noexcept = default;

	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	Vector(Vector&&) noexcept = default;

	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	Vector& operator=(const Vector&) noexcept = default;

	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	Vector& operator=(Vector&&) noexcept = default;

	/**
		@return Returns size of the vector
	*/
	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	size_type size() const noexcept {
		return static_cast<size_type>(this->container.size());
	}

	/**
		@brief Changes size of the vector

		@param new_size New vector size

		@warning If the size changes by more than it was, the new vector elements will not be explicitly initialized 
	*/
	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	void resize(const size_type new_size) & noexcept {
		this->container.resize(new_size);
	}

	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	auto&& resize(const size_type new_size) && noexcept {
		this->resize(new_size);
		return std::move(*this);
	}

	/**
		@brief Index operator to access the vector's field

		@param index Index for the numbering of the vector field

		@return @ref reference to vector's field
	*/
	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	reference operator[](const size_type index) noexcept {
		return this->container[index];
	}

	/**
		@brief Index operator to access the vector's field

		@param index Runtime index for the numbering of the vector field

		@return @ref Vector<T, Size>::const_reference to vector's field
	*/
	/** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	const_reference operator[](const size_type index) const noexcept {
		return this->container[index];
	}

	/**
		@return Container %begin()
	*/
	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE /** @endcond */
	iterator begin() noexcept {
		return this->container.begin();
	}

	/**
		@return Container %end()
	*/
	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE /** @endcond */
	iterator end() noexcept {
		return this->container.end();
	}

	/**
		@return Container %begin() const
	*/
	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE /** @endcond */
	const_iterator begin() const noexcept {
		return this->container.begin();
	}

	/**
		@return Container %end() const
	*/
	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE /** @endcond */
	const_iterator end() const noexcept {
		return this->container.end();
	}

	/**
		@return Container %cbegin() const
	*/
	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE /** @endcond */
	const_iterator cbegin() const noexcept {
		return this->container.cbegin();
	}

	/**
		@return Container %cend() const
	*/
	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE /** @endcond */
	const_iterator cend() const noexcept {
		return this->container.cend();
	}

	/**
		@return Raw vector's container
	*/
	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	container_type& get_container() & noexcept {
		return this->container;
	}

	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	const container_type& get_container() const & noexcept {
		return this->container;
	}

	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	container_type&& get_container() && noexcept {
		return std::move(this->container);
	}


protected:
	/**
		@brief Container member, that uses a vector
		@details Have the same type as @c %Container template parameter
	*/
	container_type container;
}; // class Vector<Container, dynamic_extent>

/**
	@brief Class template argument deduction (deduction guides) for @ref aml::Vector<T, Size>

	@details <b>Sample: </b>
			 @code
				aml::Vector vec(1, 2, 3, 4, 5);

				static_assert(std::is_same_v<typename decltype(vec)::value_type, int>, "");
				static_assert(vec.size() == 5, "");
			 @endcode
*/
template<class First, class... Rest>
Vector(First&&, Rest&&...) -> Vector<
	std::common_type_t<First, Rest...>, 
	(sizeof...(Rest) + 1)
>;

#define AML_VECTOR_FOR_LOOP(from_, vec_, action_)					\
	if constexpr (vec_.is_dynamic()) {								\
		for (Vectorsize i = from_; i < vec_.size(); ++i) {			\
			action_													\
		}															\
	} else {														\
		aml::static_for<from_, vec_.static_size>([&](const auto i) {\
			action_													\
		});															\
	}

#define AML_VECTOR_FUNCTION_BODY2(outtype, firstvec, secondvec, dynamic_action, static_action)								\
	if constexpr (firstvec.is_dynamic() && secondvec.is_dynamic()) {														\
		AML_DEBUG_VERIFY((left.size() == right.size()), "Dynamic vector's sizes must be equal");							\
		aml::verify_container_parameters<Left, Right>();																	\
		aml::Vector<rebind_container<Left, outtype>, aml::dynamic_extent> out(aml::size_initializer(firstvec.size()));		\
		dynamic_action																										\
		return out;																											\
	} else if constexpr (firstvec.is_dynamic() || secondvec.is_dynamic()) {													\
		AML_DEBUG_VERIFY((firstvec.size() == secondvec.size()), "Dynamic vector's and static vector's sizes must be equal");	\
		using Container_ = typename std::conditional_t<firstvec.is_dynamic(), Left, Right>;						\
		aml::Vector<rebind_container<Container_, outtype>, aml::dynamic_extent> out(aml::size_initializer(firstvec.size()));		\
		dynamic_action																										\
		return out;																											\
	} else {																												\
		static_assert(firstvec.static_size == secondvec.static_size, "Static vector's sizes must be equal");				\
		aml::Vector<outtype, firstvec.static_size> out;																		\
		static_action																										\
		return out;																											\
	}

#define AML_VECTOR_FUNCTION_BODY1(outtype, firstvec, secondvec, action_)	\
	AML_VECTOR_FUNCTION_BODY2(outtype, firstvec, secondvec, action_, action_)

#define AML_OP_BODY1(outtype, action_)							\
	AML_VECTOR_FUNCTION_BODY2(outtype, left, right,				\
	{	for (Vectorsize i = 0; i < left.size(); ++i) {			\
			out[i] = action_;									\
		}														\
	},															\
	{	aml::static_for<left.static_size>([&](const auto i) {	\
			out[i] = action_;									\
		});														\
	}															\
	)

#define AML_OP_BODY2(isdynamic, action_) \
	if constexpr (isdynamic) {									\
		for (Vectorsize i = 0; i < left.size(); ++i) {			\
			action_;											\
		}														\
	} else {													\
		aml::static_for<left.static_size>([&](const auto i) {	\
			action_;											\
		});														\
	}															\
	return left;

#define AML_OP_BODY3(outtype, vec_, containertype_, isdynamic, action_)				\
	if constexpr (isdynamic) {														\
		aml::Vector<rebind_container<containertype_, outtype>, aml::dynamic_extent>	\
			out(aml::size_initializer(vec_.size()));								\
		for (Vectorsize i = 0; i < vec_.size(); ++i) {								\
			out[i] = action_;														\
		}																			\
		return out;																	\
	} else {																		\
		aml::Vector<outtype, vec_.static_size> out;									\
		aml::static_for<vec_.static_size>([&](const auto i) {						\
			out[i] = action_;														\
		});																			\
		return out;																	\
	}

/**
	@brief Sums up the two vectors
	@details @f$   \vec{a} + \vec{b} 
				 = (\vec{a}_x, \vec{a}_y, ...) + (\vec{b}_x, \vec{b}_y, ...) 
				 = (\vec{a}_x + \vec{b}_x, \vec{a}_y + \vec{b}_y, ...) @f$

	@param left A vector that will be summed up with @p right vector
	@param right A vector that will be summed up with @p left vector

	@return A new vector which is the result of summation
*/
template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator+(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept {
	using out_type = decltype(std::declval<aml::get_value_type<decltype(left)>>() + 
							  std::declval<aml::get_value_type<decltype(right)>>());
	AML_OP_BODY1(out_type, left[i] + right[i]);
}

/**
	@brief Sums up the @p left vector with the @p right vector
	@details @f$ (\vec{a}_x, \vec{a}_y, ...) += (\vec{b}_x, \vec{b}_y, ...) 
				 = (\vec{a}_x += \vec{b}_x, \vec{a}_y += \vec{b}_y, ...) @f$

	@param[in,out] left %Vector, which will be summed with @p right, and the result will be written in this vector
	@param[in] right A vector that will be summed up with @p left

	@return Reference to summed @p left
*/
template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
auto& operator+=(Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept {
	AML_OP_BODY2(left.is_dynamic() || right.is_dynamic(), left[i] += right[i]);
}

/**
	@brief Subtracts two vectors
	@details @f$ \vec{a} - \vec{b} = (\vec{a}_x, \vec{a}_y, ...) - (\vec{b}_x, \vec{b}_y, ...) 
				 = (\vec{a}_x - \vec{b}_x, \vec{a}_y - \vec{b}_y, ...) @f$

	@param left A vector that will be subtracted with @p right vector
	@param right A vector that will be subtracted with @p left vector

	@return A new vector which is the result of subtraction
*/
template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator-(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept {
	using out_type = decltype(std::declval<aml::get_value_type<decltype(left)>>() -
							  std::declval<aml::get_value_type<decltype(right)>>());
	AML_OP_BODY1(out_type, left[i] - right[i]);
}

/**
	@brief Subtracts the @p left vector with the @p right vector
	@details @f$ (\vec{a}_x, \vec{a}_y, ...) -= (\vec{b}_x, \vec{b}_y, ...) 
				 = (\vec{a}_x -= \vec{b}_x, \vec{a}_y -= \vec{b}_y, ...) @f$

	@param[in,out] left %Vector, which will be subtracted with @p right, and the result will be written in this vector
	@param[in] right A vector that will be subtracted with @p left

	@return Reference to subtracted @p left
*/
template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
auto& operator-=(Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept {
	AML_OP_BODY2(left.is_dynamic() || right.is_dynamic(), left[i] -= right[i]);
}

/**
	@brief Multiplies vector by scalar
	@details @f$ \vec{a} * s = (\vec{a}_x, \vec{a}_y, ...) * s 
			     = s * (\vec{a}_x, \vec{a}_y, ...) 
				 = (\vec{a}_x * s, \vec{a}_y * s, ...) @f$

	@param left A vector that will be multiplied by @p right scalar
	@param right Scalar to be multiplied with the @p left vector

	@return A new vector which is the result of multiplication

	@see operator*(const Left&, const Vector<Right, RightSize>&)
*/
template<class Left, Vectorsize LeftSize, class Right> [[nodiscard]] constexpr
auto operator*(const Vector<Left, LeftSize>& left, const Right& right) noexcept {
	using out_type = decltype(std::declval<aml::get_value_type<decltype(left)>>() * 
							  std::declval<Right>());
	AML_OP_BODY3(out_type, left, Left, left.is_dynamic(), left[i] * right);
}

/**
	@brief Multiplies vector by scalar
	@details @f$ s * \vec{a} 
			 = s * (\vec{a}_x, \vec{a}_y, ...) 
			 = (\vec{a}_x, \vec{a}_y, ...) * s 
			 = (\vec{a}_x * s, \vec{a}_y * s, ...) @f$

	@param left Scalar to be multiplied with the @p right vector
	@param right A vector that will be multiplied by @p left scalar

	@return A new vector which is the result of multiplication

	@see operator*(const Vector<Left, LeftSize>&, const Right&)
*/
template<class Left, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator*(const Left& left, const Vector<Right, RightSize>& right) noexcept {
	return (right * left);
}

/**
	@brief Multiplies the @p left vector with the @p right scalar
	@details @f$ (\vec{a}_x, \vec{a}_y, ...) *= s 
				 = (\vec{a}_x *= s, \vec{a}_y *= s, ...) @f$

	@param[in,out] left %Vector, which will be multiplied with @p right scalar, and the result will be written in this vector
	@param[in] right A scalar that will be multiplied with @p left vector

	@return Reference to multiplied @p left
*/
template<class Left, Vectorsize LeftSize, class Right> constexpr
auto& operator*=(Vector<Left, LeftSize>& left, const Right& right) noexcept {
	AML_OP_BODY2(left.is_dynamic(), left[i] *= right);
}

/**
	@brief Divides vector by scalar
	@details @f$ \vec{a} / s 
			 = (\vec{a}_x, \vec{a}_y, ...) / s 
			 = (\vec{a}_x / s, \vec{a}_y / s, ...) @f$

	@param left A vector that will be divided by @p right scalar
	@param right Scalar to be divided with the @p left vector

	@return A new vector which is the result of division

	@see operator/(const Left&, const Vector<Right, RightSize>&)
*/
template<class Left, Vectorsize LeftSize, class Right> [[nodiscard]] constexpr
auto operator/(const Vector<Left, LeftSize>& left, const Right& right) noexcept {
	using out_type = decltype(std::declval<aml::get_value_type<decltype(left)>>() / 
							  std::declval<Right>());
	AML_OP_BODY3(out_type, left, Left, left.is_dynamic(), left[i] / right);
}

/**
	@brief Divides vector by scalar
	@details @f$ s / \vec{a} 
				 = s / (\vec{a}_x, \vec{a}_y, ...) 
				 = (s / \vec{a}_x, s / \vec{a}_y, ...) @f$

	@param left Scalar to be divided with the @p right vector
	@param right A vector that will be divided by @p left scalar

	@return A new vector which is the result of division

	@see operator/(const Vector<Left, LeftSize>&, const Right&)
*/
template<class Left, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator/(const Left& left, const Vector<Right, RightSize>& right) noexcept {
	using out_type = decltype(std::declval<Left>() / 
							  std::declval<aml::get_value_type<decltype(right)>>());
	AML_OP_BODY3(out_type, right, Right, right.is_dynamic(), left / right[i]);
}

/**
	@brief Divides the @p left vector with the @p right scalar
	@details @f$ (\vec{a}_x, \vec{a}_y, ...) /= s 
				 = (\vec{a}_x /= s, \vec{a}_y /= s, ...) @f$

	@param[in,out] left %Vector, which will be divided with @p right scalar, and the result will be written in this vector
	@param[in] right A scalar that will be divided with @p left vector

	@return Reference to divided @p left
*/
template<class Left, Vectorsize LeftSize, class Right> constexpr
auto& operator/=(Vector<Left, LeftSize>& left, const Right& right) noexcept {
	AML_OP_BODY2(left.is_dynamic(), left[i] /= right);
}

/**
	@brief Negates the vector
	@details @f$ -\vec{a} 
				 = -(\vec{a}_x, \vec{a}_y, ...) 
				 = (-\vec{a}_x, -\vec{a}_y, ...) @f$ @n

	@param left A vector that will be negatived

	@return New negatived vector
*/
template<class Left, Vectorsize LeftSize> [[nodiscard]] constexpr
auto operator-(const Vector<Left, LeftSize>& left) noexcept {
	using out_type = aml::get_value_type<Vector<Left, LeftSize>>;
	AML_OP_BODY3(out_type, left, Left, left.is_dynamic(), -left[i]);
}

/**
	@brief Do not use. Use #aml::dot and #aml::cross to apply vector multiplication

	@see aml::dot @n
		 aml::cross
*/
template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator*(const Vector<Left, LeftSize>&, const Vector<Right, RightSize>&) noexcept = delete;

template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
auto& operator*=(Vector<Left, LeftSize>&, const Vector<Right, RightSize>&) noexcept = delete;

template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator/(const Vector<Left, LeftSize>&, const Vector<Right, RightSize>&) noexcept = delete;

template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
auto& operator/=(Vector<Left, LeftSize>&, const Vector<Right, RightSize>&) noexcept = delete;

#undef AML_OP_BODY1
#undef AML_OP_BODY2
#undef AML_OP_BODY3

/**
	@brief Checks if the vectors are equal

	@attention 
			The size of the @p left and @p right can be different and this also affects the result

	@param left Compares with @p right vector
	@param right Compares with @p left vector

	@return Is the @p left and @p right are equal

	@see operator!=(const Vector<Left, LeftSize>&, const Vector<Right, RightSize>&)
*/
template<class Left, class Right, Vectorsize LeftSize, Vectorsize RightSize> [[nodiscard]] constexpr
bool operator==(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept
{
	if constexpr (left.is_dynamic() || right.is_dynamic()) {
		if (left.size() != right.size()) return false;
	} else {
		if constexpr (left.size() != right.size()) return false;
	}

	for (Vectorsize i = 0; i < left.size(); ++i) {
		if (aml::not_equal(left[i], right[i])) return false;
	}
	return true;
}

/**
	@brief Checks if the vectors are not equal

	@attention
			The size of the @p left and @p right can be different and this also affects the result

	@param left Compares with @p right vector
	@param right Compares with @p left vector

	@return Is the @p left and @p right are not equal

	@see operator==(const Vector<Left, LeftSize>&, const Vector<Right, RightSize>&)
*/
template<class Left, class Right, Vectorsize LeftSize, Vectorsize RightSize> [[nodiscard]] constexpr
bool operator!=(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept {
	return !(left == right);
}

/**
	@brief Vector distance, lenght, norm. @f$ || \vec{a} || @f$
	@details @f$ = \sqrt{{\vec{a}_x}^{2}+{\vec{a}_y}^{2}+{\vec{a}_z}^{2}+...} @f$

	@param vec A vector from which its length will be calculated

	@note
		The return value type will be @c float, or <tt>value type</tt> if more precisely @c float

*/
template<class OutType = selectable_unused, class T, Vectorsize Size> [[nodiscard]] constexpr
auto dist(const aml::Vector<T, Size>& vec) noexcept 
{
	using outtype = std::common_type_t<float, aml::get_value_type<decltype(vec)>>;
	outtype out = aml::sqr<outtype>(vec[VI::first]);

	AML_VECTOR_FOR_LOOP(1, vec,				\
		out += aml::sqr<outtype>(vec[i]);
	);

	return aml::selectable_convert<OutType>(aml::sqrt(out));
}

/**
	@brief Sum of all vector's elements. @f$ \sum_{i=0}^{n} \vec{a}_{i} @f$
	@details @f$ = \vec{a}_x + \vec{a}_y + \vec{a}_ z + ... @f$

	@param vec A vector from which the sum of all its elements will be calculated
*/
template<class OutType = selectable_unused, class T, Vectorsize Size> [[nodiscard]] constexpr
auto sum_of(const Vector<T, Size>& vec) noexcept 
{
	auto out = vec[VI::first];

	AML_VECTOR_FOR_LOOP(1, vec,				\
		out += vec[i];
	);

	return aml::selectable_convert<OutType>(out);
}

/**
	@brief Distance between vectors. @f$ || \vec{a} - \vec{b} || @f$
	@details @f$ = \sqrt{(x_1-x_2)^2+(y_1-y_2)^2+...} @f$

	@see aml::dist(const aml::Vector<T, Size>&)
*/
template<class OutType = selectable_unused, class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto dist_between(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept {
	return aml::dist<OutType>(left - right);
}

// vvvvv dot product impl vvvvv
struct dot_fn {
template<class OutType = selectable_unused, class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator()(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) const noexcept
{
	auto out = (left[VI::first] * right[VI::first]);

	if constexpr (left.is_dynamic() || right.is_dynamic()) {
		AML_DEBUG_VERIFY(left.size() == right.size(), "Vector's sizes must be equal");
		for (Vectorsize i = 1; i < left.size(); ++i) {
			out += left[i] * right[i];
		}
	}
	else {
		static_assert(LeftSize == RightSize, "Vector's sizes must be equal");
		aml::static_for<1, LeftSize>([&](const auto i) {
			out += left[i] * right[i];
		});
	}

	return aml::selectable_convert<OutType>(out);
}
};

/**
	@brief Linearly algebraic scalar product, dot product of vectors. @f$ \vec{a} \cdot \vec{b} @f$
	@details @f$ = \sum_{i=0}^{n} \vec{a}_i \vec{b}_i
					= \vec{a}_x \vec{b}_x + \vec{a}_y * \vec{b}_y + ... @f$ @n

				Can be used as the operator (vecres = vec1 @<dot@> vec2)

				[Wikipedia page](https://en.wikipedia.org/wiki/Dot_product)

	@param left  First input vector
	@param right Second input vector

	@note
			The size of the vectors must be equal

	@see @ref aml::Vector<T, Size> @n
		 aml::Vector<Container, dynamic_extent>
*/
inline constexpr dot_fn dot;

#ifndef AML_NO_CUSTOM_OPERATORS
namespace detail {
	template<class T>
	struct dot_op_proxy {
		const T& v;
	};

	template<class T> constexpr
	const dot_op_proxy<T> operator<(const T& left, const dot_fn&) { return {left}; }

	template<class Left, class Right, Vectorsize LeftSize, Vectorsize RightSize> constexpr
	decltype(auto) operator>(const dot_op_proxy<Vector<Left, LeftSize>>& left, const Vector<Right, RightSize>& right) {
		return aml::dot(left.v, right);
	}
}
#endif

// vvvvv cross product impl vvvvv
struct cross_fn
{
template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator()(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) const noexcept
{
	if constexpr (left.is_dynamic() || right.is_dynamic()) {
		AML_DEBUG_VERIFY((left.size() == right.size()) && (left.size() == 3), "The size of the vectors must be equal to 3");
	}
	else {
		static_assert((left.static_size == 3), "The size of the vectors must be equal to 3");
	}

	using left_value_type = aml::get_value_type<decltype(left)>;
	using right_value_type = aml::get_value_type<decltype(right)>;

	using out_type = decltype((std::declval<left_value_type>()* std::declval<right_value_type>()) -
		(std::declval<left_value_type>() * std::declval<right_value_type>()));

	using namespace aml::VI;
	const auto& a = left; const auto& b = right;

	AML_VECTOR_FUNCTION_BODY1(out_type, left, right, \
		out[X] = (a[Y] * b[Z]) - (a[Z] * b[Y]);			\
		out[Y] = (a[Z] * b[X]) - (a[X] * b[Z]);			\
		out[Z] = (a[X] * b[Y]) - (a[Y] * b[X]);			\
	);
}
};

#ifndef AML_NO_CUSTOM_OPERATORS
namespace detail
{
	template<class T>
	struct cross_op_proxy {
		const T& v;
	};

	template<class T> constexpr
	const cross_op_proxy<T> operator<(const T& left, const cross_fn&) { return {left}; }

	template<class Left, class Right, Vectorsize LeftSize, Vectorsize RightSize> constexpr
	decltype(auto) operator>(const cross_op_proxy<Vector<Left, LeftSize>>& left, const Vector<Right, RightSize>& right) {
		return aml::cross(left.v, right);
	}
}
#endif

/**
	@brief Linearly algebraic cross product of vectors. @f$ \vec{a} \times \vec{b} @f$
	@details @f$ = (\vec{a}_y \vec{b}_z - \vec{a}_z \vec{b}_y,
				    \vec{a}_z \vec{b}_x - \vec{a}_x \vec{b}_z,
				    \vec{a}_x \vec{b}_y - \vec{a}_y \vec{b}_x) 
				 = \begin{vmatrix}
				       \hat{i} & \hat{j} & \hat{k} \\
					   \vec{a}_x & \vec{a}_y & \vec{a}_z \\
					   \vec{b}_x & \vec{b}_y & \vec{b}_z
				   \end{vmatrix} @f$
			@n
			Can be used as the operator (vecres = vec1 @<cross@> vec2)
			@n
			[Wikipedia page](https://en.wikipedia.org/wiki/Cross_product)

	@attention
				The size of the vectors must be equal to 3

	@todo 
				Add seven-dimensional cross product @n
				https://en.wikipedia.org/wiki/Seven-dimensional_cross_product
*/
inline constexpr aml::cross_fn cross;

/**
	@brief Normalizes the vector. @f$ \frac{\vec{a}}{ || \vec{a} || } @f$
*/
template<class T, Vectorsize Size> [[nodiscard]] constexpr
auto normalize(const Vector<T, Size>& vec) noexcept 
{
	using disttype = decltype(aml::dist(vec));

	const disttype inv_mag = static_cast<disttype>(1) / aml::dist(vec);
	return (vec * inv_mag);
}

namespace detail {
	template<class T>
	struct DVector_default_container : std::vector<T> {};
} // namespace detail

/**
	@brief Type alias for @ref aml::Vector<Container, dynamic_extent> and it container
	@details A simple @c std::vector in template parameter @c Container will not work. @n
			 This uses a wrapper around @c std::vector

	@see Vector<Container, dynamic_extent>
*/
template<class T>
using DVector = Vector<detail::DVector_default_container<T>, aml::dynamic_extent>;


namespace detail
{
	template<class>
	struct is_vector_impl
		: std::false_type {};

	template<class T, Vectorsize Size>
	struct is_vector_impl<Vector<T, Size>>
		: std::true_type {};

	template<class Container>
	struct is_vector_impl<Vector<Container, aml::dynamic_extent>>
		: std::true_type {};
} // namespace detail

/// Checks if @p T is a vector from linear algebra
template<class T>
inline constexpr bool is_vector = detail::is_vector_impl<T>::value;

/**
	@brief Namespace for short aliases of the @ref aml::Vector<T, Size> type

	@details Usage: @n
			 @code
				using namespace aml::short_vector_alias;
			 @endcode
*/
namespace short_vector_alias
{
	template<class T> using vec2 = aml::Vector<T, 2>;
	template<class T> using vec3 = aml::Vector<T, 3>;
	template<class T> using vec4 = aml::Vector<T, 4>;
	template<class T> using vec5 = aml::Vector<T, 5>;

	using vec2i = vec2<int>;
	using vec2u = vec2<unsigned>;
	using vec2f = vec2<float>;
	using vec2d = vec2<double>;

	using vec3i = vec3<int>;
	using vec3u = vec3<unsigned>;
	using vec3f = vec3<float>;
	using vec3d = vec3<double>;

	using vec4i = vec4<int>;
	using vec4u = vec4<unsigned>;
	using vec4f = vec4<float>;
	using vec4d = vec4<double>;

	using vec5i = vec5<int>;
	using vec5u = vec5<unsigned>;
	using vec5f = vec5<float>;
	using vec5d = vec5<double>;
} // namespace short_vector_alias

} // namespace aml
