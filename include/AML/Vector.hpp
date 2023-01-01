/** @file */
#pragma once

#include <AML/Iterator.hpp>
#include <AML/Tools.hpp>
#include <AML/MathFunctions.hpp>

#include <cstddef>
#include <type_traits>
#include <algorithm>
#include <vector>

AML_NAMESPACE

/**
	@brief The main alias for vector's size type
	@details More created for code readability

	@see Vector
*/
using Vectorsize = std::size_t;

/**
	@brief Namespace for compile-time vector indexes
	@details Use them to access vector's fields from @ref Vector::operator[](const VI::index<I>) and @ref Vector::operator[](const VI::index<I>) const

	@see Vector
*/
namespace VI
{
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
}

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
		static_assert(std::is_object_v<T>,		"A vector's T must be an object type");
		static_assert(!std::is_abstract_v<T>,	"A vector's T cannot be an abstract class type");
		static_assert(aml::is_complete<T>,		"A vector's T must be a complete type");

		using size_type = Vectorsize;
		using value_type = T;

		using reference = T&;
		using const_reference = const T&;

		static constexpr bool is_dynamic() noexcept				  { return (Size == aml::dynamic_extent); }
		static constexpr bool uses_static_array() noexcept		  { return (Size > 5) && !is_dynamic(); }
		static constexpr bool has_index(size_type index) noexcept { return (Size > index); }

		static constexpr Vectorsize extent = aml::dynamic_extent;
	};
}



/**
	@brief The representation of a vector from linear algebra as a statically allocated template class

	@tparam T The type of the elements
	@tparam Size The static vector size

	@see Vector<Container, dynamic_extent>
*/
template<class T, Vectorsize Size>
class Vector /** @cond */: public detail::VectorBase<T, Size>, public detail::VectorStorage<T, Size> /** @endcond */
{
	using Storage = detail::VectorStorage<T, Size>;
	using Base	  =	detail::VectorBase<T, Size>;
public:

	using size_type			= typename Base::size_type;		  ///< The size type that vector uses
	using value_type		= typename Base::value_type;	  ///< The value type that vector uses

	using reference			= typename Base::reference;		  ///< Type that the non-const index operator returns
	using const_reference	= typename Base::const_reference; ///< Type that const index operator returns

	/// The return type of the non-const \ref begin() and \ref end() "end()" methods
	using iterator			= std::conditional_t<Base::uses_static_array(),		  T*,	   aml::IndexIterator<Vector<T, Size>>>;

	/// The return type of the const \ref begin(), \ref cbegin() and \ref end(), \ref cend() methods
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

	/**
		@brief Initializes the vector using variadic arguments
		@details All variable arguments must be a non-narrowing transformation to a main vector type

		@tparam First,Rest... Variable input variadic templates 
		@param f,r Variadic arguments
	*/
	template<class First, class... Rest> constexpr
	explicit Vector(First&& f, Rest&&... r) noexcept {
		static_assert(!(aml::is_narrowing_conversion<T, First> || (aml::is_narrowing_conversion<T, Rest> || ...)), 
			"Variadic parameter types have a narrowing conversion");
		static_assert((sizeof...(r) + 1) == Size, 
			"Bad number of variadic parameters");

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

		@see @ref zero "aml::zero"
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

		@see @ref one "aml::one"
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

		@see @ref unit "aml::unit"
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
		@brief Initializes the vector from dynamic vector

		@tparam U Value type of dynamic vector
		@param other Input dynamic vector

		@warning The size of the dynamic vector must be the same as the size of the current vector
	
		@see Vector<T, dynamic_extent>
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
	[[nodiscard]] constexpr static /** @cond */ AML_FORCEINLINE /** @endcond */
	size_type size() noexcept {
		return Size;
	}

	/**
		@brief Index operator overload to access field in compile-time 
		@details To call this operator overload you need to use \ref VI::index as an first input parameter

		@tparam I Compile-time index to the vector's field

		@return @ref reference to vector's field

		@see VI namespace
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

		@return @ref const_reference to vector's field

		@see VI namespace
	*/
	template<size_type I> constexpr /** @cond */ AML_FORCEINLINE /** @endcond */
	const_reference operator[](const VI::index<I>) const noexcept {
		return const_cast<Vector&>(*this)[VI::index<I>{}];
	}

	/**
		@brief Index operator overload to access the vector's field in runtime

		@param index Runtime index for the numbering of the vector field

		@return @ref reference to vector's field

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

		@return @ref const_reference to vector's field

		@see operator[](const VI::index<I>) const
	*/
	constexpr
	const_reference operator[](const size_type index) const noexcept {
		return const_cast<Vector&>(*this)[index];
	}

	/**
		@return Begin @ref iterator
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
		@return End @ref iterator
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
		@return Begin @ref const_iterator
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
		@return End @ref const_iterator
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
		@return Begin @ref const_iterator
	*/
	[[nodiscard]] constexpr
	const_iterator cbegin() const noexcept {
		return begin();
	}

	/**
		@return End @ref const_iterator
	*/
	[[nodiscard]] constexpr
	const_iterator cend() const noexcept {
		return end();
	}

};

/**
	@brief The representation of a vector from linear algebra as a dynamically allocated template class
	@details
		%Vector, that uses dynamically allocated container @n@n

		<b>The Container must have this template layout:</b>
		@code
			template<template<class T, class... Parameters> class Container>
		@endcode

		@c T - vector's value type @n

		@warning 
				@c T must not affect additional container parameters. @n
				This also includes default parameters that use <tt>T</tt>. @n
				For example, @c std::vector will not work because it has a second template parameter that uses its @c T

		@c Parameters... - Additional parameters @n

		<b>An example of a container structure:</b>
		@code
			template<class T, class... Parameters>
			struct Container
			{
				using size_type		 = ... ;

				using iterator		 = ... ;
				using const_iterator = ... ;

				size_type size() const;
				void resize(size_type new_size);

				iterator begin();
				iterator end();

				const_iterator begin() const;
				const_iterator end() const;

				const_iterator cbegin() const;
				const_iterator cend() const;

				T& front();

				T& operator[](size_type index);
				const T& operator[](size_type index) const;
			};
		@endcode

	@tparam Container The type of container that will be used

	@see @ref Vector "Vector<T, Size>" @n
		 dynamic_extent
*/
template<class Container>
class Vector<Container, dynamic_extent> /** @cond */: public detail::VectorBase<aml::get_value_type<Container>, dynamic_extent> /** @endcond */
{
private:
	friend class Vector;

	using Base = detail::VectorBase<typename Container::value_type, aml::dynamic_extent>;
public:

	/**
		@brief The type of container that uses the vector
	*/
	using container_type = Container;

	using size_type			= std::common_type_t<typename Base::size_type, typename container_type::size_type>; ///< The size type that vector uses
	using value_type		= typename Base::value_type; ///< The value type that vector uses

	using reference			= typename Base::reference; ///< Type that the non-const index operator returns
	using const_reference	= typename Base::const_reference; ///< Type that const index operator returns

	/// The return type of the non - const @ref begin() and @ref end() methods
	using iterator			= typename container_type::iterator;

	/// The return type of the const \ref begin(), \ref cbegin() and \ref end(), \ref cend() methods
	using const_iterator	= typename container_type::const_iterator;

	//static_assert(std::is_default_constructible_v<container_type>,	"Container must be default constructible");
	static_assert(aml::has_container_structure<container_type>,		"Container must have a container structure");

private:
	AML_CONSTEXPR_DYNAMIC_ALLOC
	void container_resize(const size_type new_size) noexcept {
		this->container.resize(new_size);
	}
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
	Vector(const value_type (&Array)[ArraySize]) noexcept {
		container_resize(ArraySize);
		std::copy_n(Array, ArraySize, this->container.begin());
	}

	/**
		@brief Initializes the vector using variadic arguments
		@details All variable arguments must be a non-narrowing transformation to a main vector type @n
				 The vector will have the size of the number of variadic arguments

		@tparam First,Rest... Variable input variadic templates
		@param f,r Variadic arguments
	*/
	template<class First, class... Rest> /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	explicit Vector(First&& f, Rest&&... r) noexcept {
		static_assert(!(aml::is_narrowing_conversion<value_type, First> || (aml::is_narrowing_conversion<value_type, Rest> || ...)),
			"Variadic parameter types have a narrowing conversion");

		container_resize(1 + sizeof...(r));
		
		this->container.front() = std::forward<First>(f);

		Vectorsize i = 1;
		([&] {
			this->container[i] = std::forward<Rest>(r);
			++i;
		}(), ...);
	}

	/**
		@brief Creates the vector with size

		@warning Does not explicitly initialize elements

		@param initsz The size that will be used to create the vector

		@see aml::size_initializer
	*/
	template<std::size_t InitSize> /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	Vector(const aml::size_initializer<InitSize> initsz) noexcept {
		container_resize(initsz.size);
	}

	/**
		@brief Creates a vector with size and fills it

		@param fill_with Size and what the vector will be filled with

		@see aml::fill_initializer
	*/
	template<std::size_t FillSize> /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	Vector(const aml::fill_initializer<value_type, FillSize> fill_with) noexcept {
		container_resize(fill_with.size);
		std::fill(this->container.begin(), this->container.end(), fill_with.value);
	}

	/**
		@brief Cast from Vector<U, dynamic_extent> to Vector<T, dynamic_extent>
	*/
	template<class U> /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	explicit Vector(const Vector<U, aml::dynamic_extent>& other) noexcept 
	{
		using other_value_type = aml::get_value_type<decltype(other)>;
		container_resize(other.size());

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
	{
		using other_value_type = aml::get_value_type<decltype(other)>;
		container_resize(other.static_size);

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

	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC /** @endcond */
	size_type size() const noexcept {
		return static_cast<size_type>(this->container.size());
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
	const_iterator cbegin() const noexcept {t
		return this->container.cbegin();
	}

	/**
		@return Container %cend() const
	*/
	[[nodiscard]] /** @cond */ AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE /** @endcond */
	const_iterator cend() const noexcept {
		return this->container.cend();
	}

protected:
	/**
		@brief %Container member, that uses a vector
		@details Have the same type as @c Container template parameter
	*/
	container_type container;
};

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
		AML_DEBUG_VERIFY(left.size() == right.size(), "Dynamic vector's sizes must be equal");								\
		using left_container = aml::get_container_data<std::decay_t<decltype(firstvec)>::container_type>;						\
		using right_container = aml::get_container_data<std::decay_t<decltype(secondvec)>::container_type>;						\
		aml::verify_container_parameters<left_container, right_container>();												\
		aml::Vector<left_container::create<outtype>, aml::dynamic_extent> out(aml::size_initializer<>(firstvec.size()));	\
		dynamic_action																										\
		return out;																											\
	} else if constexpr (firstvec.is_dynamic() || secondvec.is_dynamic()) {													\
		AML_DEBUG_VERIFY(firstvec.size() == secondvec.size(), "Dynamic vector's and static vector's sizes must be equal");	\
		using container_ = aml::get_container_data<std::conditional_t<firstvec.is_dynamic(), Left, Right>>;						\
		aml::Vector<container_::create<outtype>, aml::dynamic_extent> out(aml::size_initializer<>(firstvec.size()));		\
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
		for (Vectorsize i = 0; i < left.size(); ++i) {			\
			out[i] = action_;									\
		}														\
	,	aml::static_for<left.static_size>([&](const auto i) {	\
			out[i] = action_;									\
		});														\
)

#define AML_OP_BODY2(isdynamic, action_) \
	if constexpr (isdynamic) {									\
		for (Vectorsize i = 0; i < left.size(); ++i) {			\
			action_;											\
		}														\
	} else {													\
		aml::static_for<LeftSize>([&](const auto i) {			\
			action_;											\
		});														\
	}

#define AML_OP_BODY3(outtype, vec_, containertype_, isdynamic, action_)												\
	if constexpr (isdynamic) {																						\
		using vec_container = aml::get_container_data<containertype_>;													\
		aml::Vector<vec_container::create<outtype>, aml::dynamic_extent> out(aml::size_initializer<>(vec_.size()));	\
		for (Vectorsize i = 0; i < vec_.size(); ++i) {																\
			out[i] = action_;																						\
		}																											\
		return out;																									\
	} else {																										\
		aml::Vector<outtype, vec_.static_size> out;																	\
		aml::static_for<vec_.static_size>([&](const auto i) {														\
			out[i] = action_;																						\
		});																											\
		return out;																									\
	}


template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator+(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept {
	using out_type = decltype(std::declval<aml::get_value_type<decltype(left)>>() + 
							  std::declval<aml::get_value_type<decltype(right)>>());
	AML_OP_BODY1(out_type, left[i] + right[i]);
}

template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
auto operator+=(Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept {
	AML_OP_BODY2(left.is_dynamic() || right.is_dynamic(), left[i] += right[i]);
}

template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator-(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept {
	using out_type = decltype(std::declval<aml::get_value_type<decltype(left)>>() -
							  std::declval<aml::get_value_type<decltype(right)>>());
	AML_OP_BODY1(out_type, left[i] - right[i]);
}

template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
auto operator-=(Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept {
	AML_OP_BODY2(left.is_dynamic() || right.is_dynamic(), left[i] -= right[i]);
}

template<class Left, Vectorsize LeftSize, class Right> [[nodiscard]] constexpr
auto operator*(const Vector<Left, LeftSize>& left, const Right& right) noexcept {
	using out_type = decltype(std::declval<aml::get_value_type<decltype(left)>>() * 
							  std::declval<Right>());
	AML_OP_BODY3(out_type, left, Left, left.is_dynamic(), left[i] * right);
}

template<class Left, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator*(const Left& left, const Vector<Right, RightSize>& right) noexcept {
	return (right * left);
}

template<class Left, Vectorsize LeftSize, class Right> constexpr
auto operator*=(Vector<Left, LeftSize>& left, const Right& right) noexcept {
	AML_OP_BODY2(left.is_dynamic(), left[i] *= right);
}

template<class Left, Vectorsize LeftSize, class Right> [[nodiscard]] constexpr
auto operator/(const Vector<Left, LeftSize>& left, const Right& right) noexcept {
	using out_type = decltype(std::declval<aml::get_value_type<decltype(left)>>() / 
							  std::declval<Right>());
	AML_OP_BODY3(out_type, left, Left, left.is_dynamic(), left[i] / right);
}

template<class Left, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator/(const Left& left, const Vector<Right, RightSize>& right) noexcept {
	using out_type = decltype(std::declval<Left>() / 
							  std::declval<aml::get_value_type<decltype(right)>>());
	AML_OP_BODY3(out_type, right, Right, right.is_dynamic(), left / right[i]);
}

template<class Left, Vectorsize LeftSize, class Right> constexpr
auto operator/=(Vector<Left, LeftSize>& left, const Right& right) noexcept {
	AML_OP_BODY2(left.is_dynamic(), left[i] /= right);
}

template<class Left, Vectorsize LeftSize> [[nodiscard]] constexpr
auto operator-(const Vector<Left, LeftSize>& left) noexcept {
	using out_type = aml::get_value_type<Vector<Left, LeftSize>>;
	AML_OP_BODY3(out_type, left, Left, left.is_dynamic(), -left[i]);
}

template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator*(const Vector<Left, LeftSize>&, const Vector<Right, RightSize>&) noexcept = delete;
template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
auto operator*=(Vector<Left, LeftSize>&, const Vector<Right, RightSize>&) noexcept = delete;

template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto operator/(const Vector<Left, LeftSize>&, const Vector<Right, RightSize>&) noexcept = delete;
template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> constexpr
auto operator/=(Vector<Left, LeftSize>&, const Vector<Right, RightSize>&) noexcept = delete;

#undef AML_OP_BODY1
#undef AML_OP_BODY2
#undef AML_OP_BODY3

template<class Left, class Right, Vectorsize LeftSize, Vectorsize RightSize> [[nodiscard]] constexpr
bool operator==(const Vector<Left, LeftSize>& left, const Vector<Right, RightSize>& right) noexcept 
{
	if (left.size() != right.size()) return false;

	for (Vectorsize i = 0; i < left.size(); ++i) {
		if (aml::not_equal(left[i], right[i])) return false;
	}
	return true;
}

template<class Left, class Right, Vectorsize LeftSize, Vectorsize RightSize> [[nodiscard]] constexpr
bool operator!=(const aml::Vector<Left, LeftSize>& left, const aml::Vector<Right, RightSize>& right) noexcept {
	return !(left == right);
}


template<class OutType = aml::selectable_unused, class T, Vectorsize Size> [[nodiscard]] constexpr
auto dist(const aml::Vector<T, Size>& vec) noexcept 
{
	using outtype = std::common_type_t<float, aml::get_value_type<decltype(vec)>>;
	outtype out = aml::sqr<outtype>(vec[VI::first]);

	AML_VECTOR_FOR_LOOP(1, vec,				\
		out += aml::sqr<outtype>(vec[i]);
	);

	return aml::selectable_convert<OutType>(aml::sqrt(out));
}

template<class OutType = aml::selectable_unused, class T, Vectorsize Size> [[nodiscard]] constexpr
auto sum_of(const aml::Vector<T, Size>& vec) noexcept 
{
	auto out = vec[VI::first];

	AML_VECTOR_FOR_LOOP(1, vec,				\
		out += vec[i];
	);

	return aml::selectable_convert<OutType>(out);
}

template<class OutType = aml::selectable_unused, class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto dist_between(const aml::Vector<Left, LeftSize>& left, const aml::Vector<Right, RightSize>& right) noexcept {
	return aml::dist<OutType>(left - right);
}

template<class OutType = aml::selectable_unused, class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto dot(const aml::Vector<Left, LeftSize>& left, const aml::Vector<Right, RightSize>& right) noexcept
{
	auto out = (left[VI::first] * right[VI::first]);

	if constexpr (left.is_dynamic() || right.is_dynamic()) {
		AML_DEBUG_VERIFY(left.size() == right.size(), "Vector's sizes must be equal");
		for (Vectorsize i = 1; i < left.size(); ++i) {
			out += left[i] * right[i];
		}
	} else {
		static_assert(LeftSize == RightSize, "Vector's sizes must be equal");
		aml::static_for<1, LeftSize>([&](const auto i) {
			out += left[i] * right[i];
		});
	}

	return aml::selectable_convert<OutType>(out);
}

template<class Left, Vectorsize LeftSize, class Right, Vectorsize RightSize> [[nodiscard]] constexpr
auto cross(const aml::Vector<Left, LeftSize>& left, const aml::Vector<Right, RightSize>& right) noexcept
{
	if constexpr (left.is_dynamic() || right.is_dynamic()) {
		AML_DEBUG_VERIFY((left.size() == right.size()) && (left.size() == 3), "The size of the vectors must be equal to 3");
	} else {
		static_assert((left.static_size == 3), "The size of the vectors must be equal to 3");
	}

	using left_value_type  = aml::get_value_type<decltype(left)>;
	using right_value_type = aml::get_value_type<decltype(right)>;

	using out_type = decltype((std::declval<left_value_type>() * std::declval<right_value_type>()) - 
							  (std::declval<left_value_type>() * std::declval<right_value_type>()));

	using namespace aml::VI;
	const auto& a = left; const auto& b = right;

	AML_VECTOR_FUNCTION_BODY1(out_type, left, right,	\
		out[X] = (a[Y] * b[Z]) - (a[Z] * b[Y]);			\
		out[Y] = (a[Z] * b[X]) - (a[X] * b[Z]);			\
		out[Z] = (a[X] * b[Y]) - (a[Y] * b[X]);			\
	);
}

template<class T, Vectorsize Size> [[nodiscard]] constexpr
auto normalize(const aml::Vector<T, Size>& vec) noexcept 
{
	using disttype = decltype(aml::dist(vec));

	const disttype inv_mag = static_cast<disttype>(1) / aml::dist(vec);
	return (vec * inv_mag);
}

namespace detail {
	template<class T>
	struct DVector_default_container : std::vector<T> {};
}

/**
	@brief Type alias for @ref Vector<Container, dynamic_extent> "dynamic vector" and it container
	@details A simple @c std::vector in template parameter @c Container will not work. @n
			 This uses a wrapper around @c std::vector

	@see Vector<Container, dynamic_extent>
*/
template<class T>
using DVector = aml::Vector<detail::DVector_default_container<T>, aml::dynamic_extent>;


AML_NAMESPACE_END
