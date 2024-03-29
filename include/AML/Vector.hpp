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
#include <array>
#include <cstddef>
#include <string>

#if AML_CXX20
#include <concepts>
#include <span>
#endif

#ifdef AML_LIBRARY
	#define AML_LIBRARY_VECTOR
#else
	#error AML library is required
#endif

//#define AML_PACK_VECTOR

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
namespace VI 
{
	inline constexpr index_t<0> X{}; ///< @c x 1D variable using indexes
	inline constexpr index_t<1> Y{}; ///< @c y 2D variable using indexes
	inline constexpr index_t<2> Z{}; ///< @c z 3D variable using indexes
	inline constexpr index_t<3> W{}; ///< @c w 4D variable using indexes
	inline constexpr index_t<4> V{}; ///< @c v 5D variable using indexes
						  
	inline constexpr index_t<0> first{}; ///< Index of first vector element
} // namespace VI

namespace detail 
{
	template<class T, Vectorsize S>
	struct VectorStorage {
	protected:
		T array[S];

		template<class... Ts> constexpr VectorStorage(Ts&&... vals) : array{ std::forward<Ts>(vals)...} {}
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
		//static_assert(std::is_object_v<T>, "A vector's T must be an object type");
		static_assert(!std::is_abstract_v<T>, "A vector's T cannot be an abstract class type");
		static_assert(aml::is_complete<T>, "A vector's T must be a complete type");

		using size_type = Vectorsize;
		using value_type = T;

		using reference = T&;
		using const_reference = const T&;

		static constexpr bool is_dynamic() noexcept						{ return (Size == aml::dynamic_extent); }
		static constexpr bool uses_static_array() noexcept				{ return (Size > 5) && !is_dynamic(); }
		static constexpr bool has_index(const size_type index) noexcept { return (Size > index); }

		static constexpr Vectorsize extent = aml::dynamic_extent;
	};

	template<class T, Vectorsize Size>
	struct VectorBase<T&, Size> : VectorBase<T, Size>
	{
		using value_type = std::reference_wrapper<T>;
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
#ifdef AML_PACK_VECTOR
	#if (AML_MSVC || AML_GCC)
		#pragma pack(push, 1)
	#else
		#error is not supported struct packing by compiler
	#endif
#endif
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
	
#ifndef AML_PACK_VECTOR
	/// The return type of the non-const begin() and end() methods
	using iterator			= std::conditional_t<Base::uses_static_array(),		  value_type*,	   aml::IndexIterator<Vector<value_type, Size>>>;

	/// The return type of the const begin(), cbegin() and end(), cend() methods
	using const_iterator	= std::conditional_t<Base::uses_static_array(), const value_type*, aml::ConstIndexIterator<Vector<value_type, Size>>>;
#else
	using iterator = value_type*;
	using const_iterator = const value_type*;
#endif

	static_assert(!Base::is_dynamic());

public:

	/// Static compile-time variable defining the size of the vector
	static constexpr size_type static_size = Size;

#if !AML_CXX20
	/**
		@brief Default constructor

		@warning Does not explicitly initialize elements
	*/
	constexpr
	Vector() AML_NOEXCEPT(std::is_nothrow_default_constructible_v<value_type>) 
		: Storage{} {}

#else
	constexpr
	Vector() AML_NOEXCEPT(std::is_nothrow_default_constructible_v<value_type>)
		= default;
#endif

private:

	template<std::size_t ArraySize, std::size_t... Idx> constexpr
	Vector(const value_type (&Array)[ArraySize], std::index_sequence<Idx...>) noexcept
		: Storage{Array[Idx]...} {}

public:

	/**
		@brief Initializes the vector using a static constant array
		@details The size of the input array must match the size of the vector

		@tparam ArraySize Input array size
		@param Array Input array
	*/
	template<std::size_t ArraySize> AML_CONSTEVAL
	Vector(const value_type (&Array)[ArraySize]) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<value_type>)
		: Vector(Array, std::make_index_sequence<ArraySize>{}) {
		static_assert(ArraySize == Size,
			"The size of the array does not equal to vector's size");
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
	template<class... Rest, 
		std::enable_if_t<enable_if_variadic_constructor<Rest...>, int> = 0
	> constexpr
	explicit Vector(Rest&&... r) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<value_type>)
		: Storage{ std::forward<Rest>(r)... } {}

private:

	template<class Val, std::size_t... Idx> constexpr
	Vector(Val&& v, std::index_sequence<Idx...>) noexcept
		: Storage{((void)Idx, v)...} {}

public:

	/**
		@brief Initializes the vector with 0
		@details To call this constructor overload you need to use @ref zero "aml::zero" as an first input parameter

		@see aml::zero
	*/
	AML_MSVC_CONSTEVAL
	explicit Vector([[maybe_unused]] const aml::zero_t) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<value_type>)
		: Vector(static_cast<value_type>(aml::zero), std::make_index_sequence<Size>{})
	{}

	/**
		@brief Initializes the vector with 1
		@details To call this constructor overload you need to use @ref one "aml::one" as an first input parameter

		@see aml::one
	*/
	AML_MSVC_CONSTEVAL
	explicit Vector([[maybe_unused]] const aml::one_t) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<value_type>)
		: Vector(static_cast<value_type>(aml::one), std::make_index_sequence<Size>{})
	{}

	/**
		@brief Initializes the unit vector
		@details To call this constructor overload you need to use @ref unit "aml::unit" as an first input parameter

		@see aml::unit
	*/
	template<std::size_t Dir> AML_MSVC_CONSTEVAL
	explicit Vector([[maybe_unused]] const aml::unit_t<Dir>) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<value_type>)
		: Vector(aml::zero)
	{
		static_assert(Base::has_index(Dir), "Unit must be in vector's range");

		(*this)[aml::index_v<Dir>] = static_cast<value_type>(aml::one);
	}

private:

	template<class U, std::size_t... Idx> constexpr
	Vector(const Vector<U, Size>& other, std::index_sequence<Idx...>) noexcept
		: Storage{static_cast<value_type>(other[Idx])...} {}

public:

	/**
		@brief Cast from Vector<U, Size> to Vector<T, Size>
	*/
	template<class U> constexpr
	explicit Vector(const Vector<U, Size>& other) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<value_type>) 
		: Vector(other, std::make_index_sequence<Size>{}) {}

	/**
		@brief Initializes the filled vector 
	*/
	constexpr
	explicit Vector(const aml::fill_initializer<value_type> fill_with) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<value_type>) 
		: Vector(fill_with.value, std::make_index_sequence<Size>{}) {}

	/**
		@brief Initializes the vector from dynamic vector

		@tparam U Value type of dynamic vector
		@param other Input dynamic vector

		@warning The size of the dynamic vector must be the same as the size of the current vector
	
		@see aml::Vector<T, dynamic_extent>
	*/
	template<class U> constexpr
	explicit Vector(const Vector<U, dynamic_extent>& other) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<value_type>) {
		AML_DEBUG_VERIFY(other.size() == Size, "The dynamic vector must have the same size");
		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = static_cast<value_type>(other[i]);
		});
	}

	constexpr
	Vector(const Vector&) AML_NOEXCEPT(std::is_nothrow_copy_constructible_v<value_type>) = default;

	constexpr
	Vector(Vector&&) AML_NOEXCEPT(std::is_nothrow_move_constructible_v<value_type>) = default;

	constexpr
	Vector& operator=(const Vector&) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<value_type>) = default;

	constexpr
	Vector& operator=(Vector&&) AML_NOEXCEPT(std::is_nothrow_move_assignable_v<value_type>) = default;

	/**
		@brief Returns size of the vector
	*/
	[[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ AML_CONSTEVAL static
	size_type size() noexcept {
		return Size;
	}

	/**
		@brief Returns a new vector with size @p OtherSize and with elements of the current
		
		@attention If the new size (<tt>OtherSize</tt>) is larger than the current size (<tt>Size</tt>), the remaining elements will be filled with 0
	*/
	template<Vectorsize OtherSize, class OtherT = value_type> [[nodiscard]] constexpr
	auto resize() const noexcept
	{
		Vector<OtherT, OtherSize> out;

		aml::static_for<OtherSize>([&](const auto i) {
			if constexpr (i < Size) {
				out[i] = static_cast<OtherT>((*this)[i]);
			} else {
				out[i] = static_cast<OtherT>(aml::zero);
			}
		});

		return out;
	}

private:
	
	template<class ArrayT, Vectorsize... Idx> constexpr
	decltype(auto) to_array_impl([[maybe_unused]] std::integer_sequence<Vectorsize, Idx...>) const noexcept {
		return std::array<ArrayT, Size>{static_cast<ArrayT>((*this)[aml::index_v<Idx>])...};
	}

public:

	template<class U = value_type> [[nodiscard]] constexpr
	auto to_array() const noexcept {
		return this->to_array_impl<U>(std::make_integer_sequence<Vectorsize, Size>{});
	}


#if AML_CXX20
	template<class T2_ = T,	std::enable_if_t<
		std::is_same_v<T2_, T> && (Base::uses_static_array()
	#ifdef AML_PACK_VECTOR
	|| true		
	#endif
	), int> = 0>
	auto to_span() noexcept
	{
		#ifndef AML_PACK_VECTOR
			return std::span<value_type, Size>(this->array);
		#else
			return std::span<value_type, std::dynamic_extent>(reinterpret_cast<value_type*>(this), Size);
		#endif
	}

	template<class T2_ = T, std::enable_if_t<
		std::is_same_v<T2_, T> && (Base::uses_static_array()
	#ifdef AML_PACK_VECTOR
	|| true
	#endif
	), int> = 0>
	auto to_span() const noexcept
	{
		#ifndef AML_PACK_VECTOR
			return std::span<const value_type, Size>(this->array);
		#else
			return std::span<const value_type, std::dynamic_extent>(reinterpret_cast<const value_type*>(this), Size);
		#endif
	}
#endif

	[[nodiscard]] AML_CONSTEXPR20
	std::string to_string() const noexcept 
	{
		std::string str;

		str += "(";
		aml::static_for<Size>([&](const auto i) {
			str += std::to_string((*this)[i]);
			if constexpr (i != (Size - 1)) str += ',';
		});
		str += ")";

		return str;
	}

	/**
		@brief Index operator overload to access field in compile-time 
		@details To call this operator overload you need to use \ref VI::index as an first input parameter

		@tparam I Compile-time index to the vector's field

		@return reference to vector's field

		@see aml::VI namespace
	*/
	template<size_type I> constexpr
	reference operator[]([[maybe_unused]] const index_t<I>) noexcept 
	{
		static_assert(I < Size, "Static vector index out of range");
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
	template<size_type I> constexpr
	const_reference operator[]([[maybe_unused]] const index_t<I>) const noexcept {
		return const_cast<Vector&>(*this)[aml::index_v<I>];
	}

	/**
		@brief Index operator overload to access the vector's field in runtime

		@param index Runtime index for the numbering of the vector field

		@return #reference to vector's field

		@see operator[](const VI::index<I>)
	*/
	constexpr
	reference operator[](const size_type index) noexcept 
	{
		AML_DEBUG_VERIFY(index < Size, "Vector index out of range | index: %zu", index);

		if constexpr (Base::uses_static_array()) {
			return Storage::array[index];
		} else {
#ifndef AML_PACK_VECTOR
			if constexpr (Base::has_index(0)) if (index == 0) return Storage::x;
			if constexpr (Base::has_index(1)) if (index == 1) return Storage::y;
			if constexpr (Base::has_index(2)) if (index == 2) return Storage::z;
			if constexpr (Base::has_index(3)) if (index == 3) return Storage::w;
			if constexpr (Base::has_index(4)) if (index == 4) return Storage::v;
			AML_UNREACHABLE;
#else
			// undefined behavior vvv
			return (reinterpret_cast<value_type*>(this))[index];
#endif
		}
	}

	constexpr
	reference first() noexcept { return (*this)[aml::index_v<0>]; }
	constexpr
	const_reference first() const noexcept { return const_cast<Vector&>(*this).first(); }

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
	iterator begin() AML_NOEXCEPT(std::is_nothrow_constructible_v<iterator, decltype(*this), decltype(0)>) {
		if constexpr (Base::uses_static_array()) {
			return Storage::array;
		} else {
#ifndef AML_PACK_VECTOR
			return iterator(*this, 0);
#else
			return reinterpret_cast<iterator>(this);
#endif
		}
	}

	/**
		@return End #iterator
	*/
	[[nodiscard]] constexpr
	iterator end() AML_NOEXCEPT(std::is_nothrow_constructible_v<iterator, decltype(*this), decltype(size())>) {
		if constexpr (Base::uses_static_array()) {
			return Storage::array + this->size();
		} else {
#ifndef AML_PACK_VECTOR
			return iterator(*this, this->size());
#else
			return reinterpret_cast<iterator>(this) + this->size();
#endif
		}
	}

	/**
		@return Begin #const_iterator
	*/
	[[nodiscard]] constexpr
	const_iterator begin() const AML_NOEXCEPT(std::is_nothrow_constructible_v<const_iterator, decltype(*this), decltype(0)>) {
		if constexpr (Base::uses_static_array()) {
			return Storage::array;
		} else {
#ifndef AML_PACK_VECTOR
			return const_iterator(*this, 0);
#else
			return reinterpret_cast<const_iterator>(this);
#endif
		}
	}

	/**
		@return End #const_iterator
	*/
	[[nodiscard]] constexpr
	const_iterator end() const AML_NOEXCEPT(std::is_nothrow_constructible_v<const_iterator, decltype(*this), decltype(size())>) {
		if constexpr (Base::uses_static_array()) {
			return Storage::array + this->size();
		} else {
#ifndef AML_PACK_VECTOR
			return const_iterator(*this, this->size());
#else
			return reinterpret_cast<const_iterator>(this) + this->size();
#endif
		}
	}

	/**
		@return Begin #const_iterator
	*/
	[[nodiscard]] constexpr
	const_iterator cbegin() const AML_NOEXCEPT(noexcept(begin())) {
		return begin();
	}

	/**
		@return End #const_iterator
	*/
	[[nodiscard]] constexpr
	const_iterator cend() const AML_NOEXCEPT(noexcept(end())) {
		return end();
	}

}; // class Vector<T, Size>
#ifdef AML_PACK_VECTOR
	#if (AML_MSVC || AML_GCC)
		#pragma pack(pop)
	#endif
#endif

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
	aml::common_type<First, Rest...>,
	(sizeof...(Rest) + 1)
>;

template<class T, std::size_t N>
Vector(const T(&)[N]) -> Vector<T, N>;

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
class Vector<Container, dynamic_extent> /** @cond */: public detail::VectorBase<aml::value_type_of<Container>, dynamic_extent> /** @endcond */
{
private:
	friend class Vector;

	using Base = detail::VectorBase<aml::value_type_of<Container>, dynamic_extent>;
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
	AML_CONSTEXPR20
	Vector() AML_NOEXCEPT(std::is_nothrow_default_constructible_v<container_type>) 
		= default;

	/**
		@brief Initializes the vector using a static constant array
		@details The vector will have the size of the input array

		@tparam ArraySize Input array size
		@param Array Input array
	*/
	template<std::size_t ArraySize> AML_CONSTEXPR20
	Vector(const value_type (&Array)[ArraySize]) AML_NOEXCEPT(noexcept(std::copy_n(Array, ArraySize, this->container.begin())))
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
	template<class... Rest, 
		std::enable_if_t<enable_if_variadic_constructor<Rest...>, int> = 0
	> AML_CONSTEXPR20
	explicit Vector(Rest&&... r) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<reference>)
		: Vector(aml::size_initializer(sizeof...(r))) 
	{
		Vectorsize i = 0;
		// C++17 fold expression
		((container[i++] = std::forward<Rest>(r)), ...);
	}

	/**
		@brief Creates the vector with size

		@warning Does not explicitly initialize elements

		@param initsz The size that will be used to create the vector

		@see aml::size_initializer
	*/
	AML_CONSTEXPR20
	explicit Vector(const aml::size_initializer initsz) AML_NOEXCEPT(noexcept(this->resize(initsz.size))) {
		this->resize(initsz.size);
	}

	/**
		@brief Creates the vector with size and fills it with 0

		@see aml::zero
	*/
	AML_CONSTEXPR20
	explicit Vector(const aml::size_initializer initsz, [[maybe_unused]] const aml::zero_t) AML_NOEXCEPT(std::is_nothrow_constructible_v<Vector, decltype(initsz), decltype(aml::fill_initializer<value_type>(static_cast<value_type>(0)))>)
		: Vector(initsz, aml::fill_initializer<value_type>(static_cast<value_type>(aml::zero))) {
	}

	/**
		@brief Creates the vector with size and fills it with 1

		@see aml::one
	*/
	AML_CONSTEXPR20
		explicit Vector(const aml::size_initializer initsz, [[maybe_unused]] const aml::one_t) AML_NOEXCEPT(std::is_nothrow_constructible_v<Vector, decltype(initsz), decltype(aml::fill_initializer<value_type>(static_cast<value_type>(0)))>)
		: Vector(initsz, aml::fill_initializer<value_type>(static_cast<value_type>(aml::one))) {
	}

	/**
		@brief Creates the unit vector

		@see aml::unit
	*/
	template<std::size_t Dir> AML_CONSTEXPR20
	explicit Vector(const aml::size_initializer initsz, [[maybe_unused]] const aml::unit_t<Dir>) AML_NOEXCEPT(std::is_nothrow_constructible_v<Vector, decltype(initsz), decltype(aml::zero)> && std::is_nothrow_copy_assignable_v<reference>)
		: Vector(initsz, aml::zero) {
		this->container[Dir] = static_cast<value_type>(aml::one);
	}

	/**
		@brief Creates a vector with size and fills it

		@param initsz The size that will be used to create the vector
		@param fill_with Size and what the vector will be filled with

		@see aml::fill_initializer
	*/
	AML_CONSTEXPR20
	explicit Vector(const aml::size_initializer initsz, const aml::fill_initializer<value_type> fill_with) AML_NOEXCEPT(std::is_nothrow_constructible_v<Vector, decltype(initsz)> && noexcept(std::fill(this->container.begin(), this->container.end(), fill_with.value)))
		: Vector(initsz) {
		std::fill(this->container.begin(), this->container.end(), fill_with.value);
	}

	/**
		@brief Cast from Vector<U, dynamic_extent> to Vector<T, dynamic_extent>
	*/
	template<class U> AML_CONSTEXPR20
	explicit Vector(const Vector<U, aml::dynamic_extent>& other) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<reference>)
		: Vector(aml::size_initializer(other.size())) 
	{
		std::transform(other.cbegin(), other.cend(), this->begin(),
			[](const auto& val) {
				return static_cast<value_type>(val);
			}
		);
	}

	/**
		@brief Creates dynamically allocated vector from statically allocated vector

		@tparam OtherSize The size of the static allocated vector at compile-time
	*/
	template<class U, Vectorsize OtherSize> AML_CONSTEXPR20
	explicit Vector(const Vector<U, OtherSize>& other) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<reference>)
		: Vector(aml::size_initializer(other.static_size))
	{
		std::transform(other.cbegin(), other.cend(), this->container.begin(),
			[](const auto& val) {
				return static_cast<value_type>(val);
			}
		);
	}

	AML_CONSTEXPR20
	Vector(const Vector&) AML_NOEXCEPT(std::is_nothrow_copy_constructible_v<container_type>) = default;

	AML_CONSTEXPR20
	Vector(Vector&&) AML_NOEXCEPT(std::is_nothrow_move_constructible_v<container_type>) = default;

	AML_CONSTEXPR20
	Vector& operator=(const Vector&) AML_NOEXCEPT(std::is_nothrow_copy_assignable_v<container_type>) = default;

	AML_CONSTEXPR20
	Vector& operator=(Vector&&) AML_NOEXCEPT(std::is_nothrow_move_assignable_v<container_type>) = default;

	/**
		@return Returns size of the vector
	*/
	[[nodiscard]] AML_CONSTEXPR20
	size_type size() const AML_NOEXCEPT(noexcept(this->container.size())) {
		return static_cast<size_type>(this->container.size());
	}

	/**
		@brief Changes size of the vector

		@param new_size New vector size

		@warning If the size changes by more than it was, the new vector elements will not be explicitly initialized 
	*/
	AML_CONSTEXPR20
	void resize(const size_type new_size) & AML_NOEXCEPT(noexcept(this->container.resize(new_size))) {
		this->container.resize(new_size);
	}

	AML_CONSTEXPR20
	auto&& resize(const size_type new_size) && AML_NOEXCEPT(noexcept(this->resize(new_size))) {
		this->resize(new_size);
		return std::move(*this);
	}

	[[nodiscard]] AML_CONSTEXPR20
	std::string to_string() const noexcept
	{
		std::string str;

		str += "(";
		for (auto i = this->begin(); i != this->end(); ++i)
		{
			str += std::to_string(*i);
			if (i != std::prev(this->end())) str += ',';
		}
		str += ")";

		return str;
	}

	/**
		@brief Index operator to access the vector's field

		@param index Index for the numbering of the vector field

		@return @ref reference to vector's field
	*/
	AML_CONSTEXPR20
	reference operator[](const size_type index) AML_NOEXCEPT(noexcept(this->container[index])) {
		return this->container[index];
	}

	/**
		@brief Index operator to access the vector's field

		@param index Runtime index for the numbering of the vector field

		@return @ref Vector<T, Size>::const_reference to vector's field
	*/
	AML_CONSTEXPR20
	const_reference operator[](const size_type index) const AML_NOEXCEPT(noexcept(this->container[index])) {
		return this->container[index];
	}

	constexpr
	reference first() noexcept { return (*this)[0]; }
	constexpr
	const_reference first() const noexcept { return const_cast<Vector&>(*this).first(); }

	/**
		@return Container %begin()
	*/
	[[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ AML_CONSTEXPR20
	iterator begin() AML_NOEXCEPT(noexcept(this->container.begin())) {
		return this->container.begin();
	}

	/**
		@return Container %end()
	*/
	[[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ AML_CONSTEXPR20
	iterator end() AML_NOEXCEPT(noexcept(this->container.end())) {
		return this->container.end();
	}

	/**
		@return Container %begin() const
	*/
	[[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ AML_CONSTEXPR20
	const_iterator begin() const AML_NOEXCEPT(noexcept(this->container.begin())) {
		return this->container.begin();
	}

	/**
		@return Container %end() const
	*/
	[[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ AML_CONSTEXPR20
	const_iterator end() const AML_NOEXCEPT(noexcept(this->container.end())) {
		return this->container.end();
	}

	/**
		@return Container %cbegin() const
	*/
	[[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ AML_CONSTEXPR20
	const_iterator cbegin() const AML_NOEXCEPT(noexcept(this->container.cbegin())) {
		return this->container.cbegin();
	}

	/**
		@return Container %cend() const
	*/
	[[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ AML_CONSTEXPR20
	const_iterator cend() const AML_NOEXCEPT(noexcept(this->container.cend())) {
		return this->container.cend();
	}

	/**
		@return Raw vector's container
	*/
	[[nodiscard]] AML_CONSTEXPR20
	container_type& get_container() & noexcept {
		return this->container;
	}

	[[nodiscard]] AML_CONSTEXPR20
	const container_type& get_container() const & noexcept {
		return this->container;
	}

	[[nodiscard]] AML_CONSTEXPR20
	container_type&& get_container() && noexcept(std::is_nothrow_move_constructible_v<container_type>) {
		return std::move(this->container);
	}


protected:
	/**
		@brief Container member, that uses a vector
		@details Have the same type as @c %Container template parameter
	*/
	container_type container;
}; // class Vector<Container, dynamic_extent>

}
// vvv std vvv / ^^^ aml ^^^
namespace std
{
	template<class T, ::aml::Vectorsize Size> 
	struct tuple_size<::aml::Vector<T, Size>> 
		: std::integral_constant<std::size_t, Size> {};

	template<std::size_t I, class T, ::aml::Vectorsize Size>
	struct tuple_element<I, ::aml::Vector<T, Size>> 
	{
		using type = typename ::aml::template Vector<T, Size>::value_type;
	};
} 
// ^^^ std ^^^ / vvv aml vvv
namespace aml 
{

template<std::size_t I, class T, Vectorsize Size> constexpr
	  auto&  get	   (Vector<T, Size>& vec) noexcept 
	{ return vec[aml::index_v<I>]; }
template<std::size_t I, class T, Vectorsize Size> constexpr 
const auto&  get (const Vector<T, Size>& vec) noexcept 
	{ return vec[aml::index_v<I>]; }
template<std::size_t I, class T, Vectorsize Size> constexpr
	  auto&& get	  (Vector<T, Size>&& vec) noexcept 
	{ return std::move(vec[aml::index_v<I>]); }
template<std::size_t I, class T, Vectorsize Size> constexpr
const auto&& get(const Vector<T, Size>&& vec) noexcept 
	{ return std::move(vec[aml::index_v<I>]); }


template<class Container, class U>
struct rebind_body<Vector<Container, aml::dynamic_extent>, U> {
	using type = aml::Vector<aml::rebind<Container, U>, aml::dynamic_extent>;
};
template<class T, Vectorsize Size, class U>
struct rebind_body<Vector<T, Size>, U> {
	using type = aml::Vector<U, Size>;
};


template<class First, Vectorsize FirstSize, class Second, Vectorsize SecondSize>
struct common_type_body<
	aml::Vector<First, FirstSize>,
	aml::Vector<Second, SecondSize>
> {
	template<bool first_dynamic, bool second_dynamic, class F, class S>
	struct get_common_val {
		using type = aml::container_common_type<F, S>;
	};

	template<class F, class S> struct get_common_val<true, false, F, S> {
		using type = aml::rebind<F, aml::common_type<
			aml::container_value_type<F>, S	
		>>;
	};
	template<class F, class S> struct get_common_val<false, true, F, S> 
		: get_common_val<true, false, S, F> {};

	template<class F, class S> struct get_common_val<false, false, F, S> {
		using type = aml::common_type<F, S>;
	};

	using vec_val_type = typename get_common_val<
		aml::Vector<First, FirstSize>::is_dynamic(),
		aml::Vector<Second, SecondSize>::is_dynamic(),
		First, Second
	>::type;

	using type = aml::Vector<vec_val_type, aml::max(FirstSize, SecondSize)>;
};

namespace detail
{
	template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
	void verify_vector_size(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept
	{
		if constexpr (left.is_dynamic() || right.is_dynamic()) {
			AML_DEBUG_VERIFY(left.size() == right.size(),
				"Vector's sizes must be equal | left size: %zu, right size: %zu", left.size(), right.size());
		} else {
			static_assert(LeftSize == RightSize, "Vector's sizes must be equal");
		}
	}

	template<Vectorsize Start = 0, class Action, class Vec> constexpr
	void iterate_vector(Action&& action, Vec&& vec) noexcept
	{
		if constexpr (vec.is_dynamic()) {
			for (Vectorsize i = Start; i < vec.size(); ++i) {
				action(i);
			}
		} else {
			aml::static_for<Start, vec.static_size>(action);
		}
	}

	template<class Result, bool is_dynamic, class Action, class Left> constexpr
	Result apply_vector_operation(Action&& action, Left&& left) noexcept
	{
		auto out = [&]() {
			if constexpr (is_dynamic) { return Result{left.size()}; }
			else { return Result{}; }
		}();

		iterate_vector([&](const auto i) {
			out[i] = action(i);
		}, left);

		return out;
	}

	template<class Operation, class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
	auto do_vector_operation(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept
	{
		verify_vector_size(left, right);
		using result_t = aml::rebind<
			aml::common_type<aml::remove_cvref<decltype(left)>, aml::remove_cvref<decltype(right)>>,
			aml::remove_cvref<decltype(Operation{}(left[0], right[0]))>
		>;
		return apply_vector_operation<
			result_t,
			left.is_dynamic() || right.is_dynamic()
		>([&](const auto i) {
			return Operation{}(left[i], right[i]);
		}, aml::constexpr_ternary<left.is_dynamic()>(right, left));
	}
	template<class Operation, bool swap_order, class Left, Vectorsize LeftSize, class Right> constexpr
	auto do_vector_operation(const Vector<Left, LeftSize>& left, const Right& right) noexcept
	{
		using result_t = aml::rebind<aml::remove_cvref<decltype(left)>, 
			aml::common_type<
				typename aml::remove_cvref<decltype(left)>::value_type, 
				aml::remove_cvref<decltype(right)>
			>
		>;
		return apply_vector_operation<
			result_t,
			left.is_dynamic()
		>([&](const auto i) {
			if constexpr (swap_order) {
				return Operation{}(right, left[i]);
			} else {
				return Operation{}(left[i], right);
			}
		}, left);
	}
	template<class Operation, class Left, Vectorsize LeftSize> constexpr
	auto do_vector_operation(const Vector<Left, LeftSize>& left) noexcept
	{
		return apply_vector_operation<
			aml::remove_cvref<decltype(left)>,
			left.is_dynamic()
		>([&](const auto i) {
			return Operation{}(left[i]);
		}, left);
	}

	template<class Operation, class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
	auto& do_vector_assign_operation(Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept
	{
		verify_vector_size(left, right);
		iterate_vector([&](const auto i) {
			Operation{}(left[i], right[i]);
		}, aml::constexpr_ternary<left.is_dynamic()>(right, left));
		return left;
	}
	template<class Operation, class Left, Vectorsize LeftSize, class Right> constexpr
	auto& do_vector_assign_operation(Vector<Left, LeftSize>& left, const Right& right) noexcept
	{
		iterate_vector([&](const auto i) {
			Operation{}(left[i], right);
		}, left);
		return left;
	}
}

#if 0 // old

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
		aml::Vector<rebind<Left, outtype>, aml::dynamic_extent> out(aml::size_initializer(firstvec.size()));		\
		dynamic_action																										\
		return out;																											\
	} else if constexpr (firstvec.is_dynamic() || secondvec.is_dynamic()) {													\
		AML_DEBUG_VERIFY((firstvec.size() == secondvec.size()), "Dynamic vector's and static vector's sizes must be equal");	\
		using Container_ = std::conditional_t<firstvec.is_dynamic(), Left, Right>;						\
		aml::Vector<rebind<Container_, outtype>, aml::dynamic_extent> out(aml::size_initializer(firstvec.size()));		\
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
		aml::Vector<rebind<containertype_, outtype>, aml::dynamic_extent>	\
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

#endif

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
	return detail::do_vector_operation<aml::plus>(left, right);
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
	//AML_OP_BODY2(left.is_dynamic() || right.is_dynamic(), left[i] += right[i]);
	return detail::do_vector_assign_operation<aml::plus_assign>(left, right);
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
	return detail::do_vector_operation<aml::minus>(left, right);
	//using out_type = decltype(std::declval<aml::value_type_of<decltype(left)>>() -
	//						  std::declval<aml::value_type_of<decltype(right)>>());
	//AML_OP_BODY1(out_type, left[i] - right[i]);
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
	return detail::do_vector_assign_operation<aml::minus_assign>(left, right);
	//AML_OP_BODY2(left.is_dynamic() || right.is_dynamic(), left[i] -= right[i]);
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
	return detail::do_vector_operation<aml::multiplies, false>(left, right);
	//using out_type = decltype(std::declval<aml::value_type_of<decltype(left)>>() * 
	//						  std::declval<Right>());
	//AML_OP_BODY3(out_type, left, Left, left.is_dynamic(), left[i] * right);
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
	return detail::do_vector_operation<aml::multiplies, true>(right, left);
	//return (right * left);
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
	return detail::do_vector_assign_operation<aml::multiplies_assign>(left, right);
	//AML_OP_BODY2(left.is_dynamic(), left[i] *= right);
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
	return detail::do_vector_operation<aml::divides, false>(left, right);
	//using out_type = decltype(std::declval<aml::value_type_of<decltype(left)>>() / 
	//						  std::declval<Right>());
	//AML_OP_BODY3(out_type, left, Left, left.is_dynamic(), left[i] / right);
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
	return detail::do_vector_operation<aml::divides, true>(right, left);
	//using out_type = decltype(std::declval<Left>() / 
	//						  std::declval<aml::value_type_of<decltype(right)>>());
	//AML_OP_BODY3(out_type, right, Right, right.is_dynamic(), left / right[i]);
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
	return detail::do_vector_assign_operation<aml::divides_assign>(left, right);
	//AML_OP_BODY2(left.is_dynamic(), left[i] /= right);
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
	return detail::do_vector_operation<aml::negate>(left);
	//using out_type = aml::value_type_of<Vector<Left, LeftSize>>;
	//AML_OP_BODY3(out_type, left, Left, left.is_dynamic(), -left[i]);
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

//#undef AML_OP_BODY1
//#undef AML_OP_BODY2
//#undef AML_OP_BODY3

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
	using result_t = aml::common_type<float, aml::value_type_of<decltype(vec)>>;
	result_t out = aml::sqr<result_t>(vec.first());

	detail::iterate_vector<1>([&](const auto i) {
		out += static_cast<result_t>(aml::sqr(vec[i]));
	}, vec);

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
	auto out = vec.first();

	detail::iterate_vector<1>([&](const auto i) {
		out += vec[i];
	}, vec);

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
	auto out = (left.first() * right.first());

	detail::verify_vector_size(left, right);

	detail::iterate_vector<1>([&](const auto i) {
		out += left[i] * right[i];
	}, left);

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
		AML_DEBUG_VERIFY((left.size() == right.size()) && (left.size() == 3), "The size of the vectors must be equal to 3 | left size: %zu, right size: %zu", left.size(), right.size());
	} else {
		static_assert((left.static_size == 3), "The size of the vectors must be equal to 3");
	}

	using result_t = aml::rebind<
		aml::common_type<aml::remove_cvref<decltype(left)>, aml::remove_cvref<decltype(right)>>,
		aml::remove_cvref<decltype((left[0] * right[0]) - (left[0] * right[0]))>
	>;

	const auto& a = left; const auto& b = right;

	auto out = [&]() {
		if constexpr (left.is_dynamic()) { return result_t{left.size()}; }
		else if constexpr (right.is_dynamic()) { return result_t{right.size()}; }
		else { return result_t{}; }
	}();

	using namespace aml::VI;

	out[X] = (a[Y] * b[Z]) - (a[Z] * b[Y]);
	out[Y] = (a[Z] * b[X]) - (a[X] * b[Z]);
	out[Z] = (a[X] * b[Y]) - (a[Y] * b[X]);

	return out;
}
};

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
	@brief Normalizes the vector. @f$ \frac{\vec{a}}{ || \vec{a} || } @f$
*/
template<class T, Vectorsize Size> [[nodiscard]] constexpr
auto normalize(const Vector<T, Size>& vec) noexcept 
{
	using disttype = decltype(aml::dist(vec));

	const disttype inv_mag = static_cast<disttype>(1) / aml::dist(vec);
	return (vec * inv_mag);
}

/**
	@brief Type alias for @ref aml::Vector<Container, dynamic_extent> and it container
	@details Uses #aml::rebind to change @c std::vector value type

	@see Vector<Container, dynamic_extent> @n
		 aml::rebind
*/
template<class T>
using DVector = Vector<std::vector<T>, aml::dynamic_extent>;

#if 0
template<class First, class... Ts>
struct common_type_body<aml::Vector<First, aml::dynamic_extent>, aml::Vector<Ts, aml::dynamic_extent>...> 
{
	static_assert((aml::is_same_specialization<First, Ts> && ...));

	using common_val_type = std::common_type_t<aml::container_value_type<First>, aml::container_value_type<Ts>...>;

	using cont_type = aml::rebind_container<First, common_val_type>;

	using type = aml::Vector<cont_type, aml::dynamic_extent>;
};
#endif



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
