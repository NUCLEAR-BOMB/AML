#pragma once

#include <AML/Iterator.hpp>
#include <AML/Tools.hpp>
#include <AML/MathFunctions.hpp>

#include <cstddef>
#include <type_traits>
#include <algorithm>
#include <vector>
#include <iterator>

AML_NAMESPACE

using Vectorsize = std::size_t;

namespace VI
{
	template<Vectorsize I>
	using index = std::integral_constant<Vectorsize, I>;

	inline constexpr index<0> X{};
	inline constexpr index<1> Y{};
	inline constexpr index<2> Z{};
	inline constexpr index<3> W{};
	inline constexpr index<4> V{};

	inline constexpr index<0> first{};
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

template<class T, Vectorsize Size>
class Vector : public detail::VectorBase<T, Size>, public detail::VectorStorage<T, Size>
{
	using Storage = detail::VectorStorage<T, Size>;
	using Base	  = detail::VectorBase<T, Size>;
public:
	using size_type			= typename Base::size_type;
	using value_type		= typename Base::value_type;

	using reference			= typename Base::reference;
	using const_reference	= typename Base::const_reference;

	using iterator			= std::conditional_t<Base::uses_static_array(),		  T*,	   aml::IndexIterator<Vector<T, Size>>>;
	using const_iterator	= std::conditional_t<Base::uses_static_array(), const T*, aml::ConstIndexIterator<Vector<T, Size>>>;

public:

	static constexpr size_type static_size = Size;

	constexpr
	Vector() noexcept {
		
	}

	template<std::size_t ArraySize> constexpr
	Vector(const T (&Array)[ArraySize]) noexcept {
		static_assert(ArraySize == Size, 
			"The size of the array does not equal to vector's size");

		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = Array[i];
		});
	}

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

	constexpr
	explicit Vector(const aml::zero_t) noexcept {
		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = static_cast<T>(0);
		});
	}

	constexpr
	explicit Vector(const aml::one_t) noexcept {
		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = static_cast<T>(1);
		});
	}

	template<std::size_t Dir> constexpr 
	explicit Vector(const aml::unit_t<Dir>) noexcept {
		static_assert(Base::has_index(Dir), "Unit must be in vector's range");
		aml::static_for<Size>([&](const auto i) {
			if constexpr (i == Dir) (*this)[i] = static_cast<T>(1);
			else					(*this)[i] = static_cast<T>(0);
		});
	}

	template<class U> constexpr
	explicit Vector(const Vector<U, Size>& other) noexcept {
		aml::static_for<Size>([&](const auto i) {
			(*this)[i] = static_cast<T>(other[i]);
		});
	}

	template<class U> constexpr
	explicit Vector(const Vector<U, aml::dynamic_extent>& other) noexcept {
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

	[[nodiscard]] constexpr static AML_FORCEINLINE
	size_type size() noexcept {
		return Size;
	}

	template<size_type I> constexpr AML_FORCEINLINE
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

	template<size_type I> constexpr AML_FORCEINLINE
	const_reference operator[](const VI::index<I>) const noexcept {
		return const_cast<Vector&>(*this)[VI::index<I>{}];
	}

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

	constexpr
	const_reference operator[](const size_type index) const noexcept {
		return const_cast<Vector&>(*this)[index];
	}

	[[nodiscard]] constexpr
	iterator begin() noexcept {
		if constexpr (Base::uses_static_array()) {
			return Storage::array;
		} else {
			return iterator(*this, 0);
		}
	}

	[[nodiscard]] constexpr
	iterator end() noexcept {
		if constexpr (Base::uses_static_array()) {
			return Storage::array + size();
		} else {
			return iterator(*this, size());
		}
	}

	[[nodiscard]] constexpr
	const_iterator begin() const noexcept {
		if constexpr (Base::uses_static_array()) {
			return Storage::array;
		} else {
			return const_iterator(*this, 0);
		}
	}

	[[nodiscard]] constexpr
	const_iterator end() const noexcept {
		if constexpr (Base::uses_static_array()) {
			return Storage::array + size();
		} else {
			return const_iterator(*this, size());
		}
	}

	[[nodiscard]] constexpr
	const_iterator cbegin() const noexcept {
		return begin();
	}

	[[nodiscard]] constexpr
	const_iterator cend() const noexcept {
		return end();
	}

};

template<class Container>
class Vector<Container, aml::dynamic_extent> : public detail::VectorBase<aml::get_value_type<Container>, aml::dynamic_extent>
{
private:
	friend class Vector;

	using Base = detail::VectorBase<typename Container::value_type, aml::dynamic_extent>;
public:
	using container_type = Container;

	using size_type			= typename Base::size_type;
	using value_type		= typename Base::value_type;

	using reference			= typename Base::reference;
	using const_reference	= typename Base::const_reference;

	using iterator			= typename container_type::iterator;
	using const_iterator	= typename container_type::const_iterator;

	static_assert(std::is_default_constructible_v<container_type>,	"Container must be default constructible");
	static_assert(aml::has_container_structure<container_type>,		"Container must have a container structure");

private:
	using container_size_type = typename container_type::size_type;

	AML_CONSTEXPR_DYNAMIC_ALLOC
	void container_resize(const size_type new_size) noexcept {
		this->container.resize(static_cast<container_size_type>(new_size));
	}
public:

	AML_CONSTEXPR_DYNAMIC_ALLOC
	Vector() noexcept {

	}

	template<std::size_t ArraySize> AML_CONSTEXPR_DYNAMIC_ALLOC
	Vector(const value_type (&Array)[ArraySize]) noexcept {
		container_resize(ArraySize);
		std::copy_n(Array, ArraySize, this->container.begin());
	}

	template<class First, class... Rest> AML_CONSTEXPR_DYNAMIC_ALLOC
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

	template<std::size_t InitSize> AML_CONSTEXPR_DYNAMIC_ALLOC
	Vector(const aml::size_initializer<InitSize> initsz) noexcept {
		container_resize(initsz.size);
	}

	template<std::size_t FillSize> AML_CONSTEXPR_DYNAMIC_ALLOC
	Vector(const aml::fill_initializer<value_type, FillSize> fill_with) noexcept {
		container_resize(fill_with.size);
		std::fill(this->container.begin(), this->container.end(), fill_with.value);
	}

	template<class U> AML_CONSTEXPR_DYNAMIC_ALLOC
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

	template<class U, Vectorsize OtherSize> AML_CONSTEXPR_DYNAMIC_ALLOC
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

	AML_CONSTEXPR_DYNAMIC_ALLOC
	Vector(const Vector&) noexcept = default;

	AML_CONSTEXPR_DYNAMIC_ALLOC
	Vector(Vector&&) noexcept = default;

	AML_CONSTEXPR_DYNAMIC_ALLOC
	Vector& operator=(const Vector&) noexcept = default;

	AML_CONSTEXPR_DYNAMIC_ALLOC
	Vector& operator=(Vector&&) noexcept = default;

	[[nodiscard]] AML_CONSTEXPR_DYNAMIC_ALLOC
	size_type size() const noexcept {
		return static_cast<size_type>(this->container.size());
	}

	AML_CONSTEXPR_DYNAMIC_ALLOC
	reference operator[](const size_type index) noexcept {
		return this->container[index];
	}

	AML_CONSTEXPR_DYNAMIC_ALLOC
	const_reference operator[](const size_type index) const noexcept {
		return this->container[index];
	}


	[[nodiscard]] AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE
	iterator begin() noexcept {
		return this->container.begin();
	}

	[[nodiscard]] AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE
	iterator end() noexcept {
		return this->container.end();
	}

	[[nodiscard]] AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE
	const_iterator begin() const noexcept {
		return this->container.begin();
	}

	[[nodiscard]] AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE
	const_iterator end() const noexcept {
		return this->container.end();
	}

	[[nodiscard]] AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE
	const_iterator cbegin() const noexcept {t
		return this->container.cbegin();
	}

	[[nodiscard]] AML_CONSTEXPR_DYNAMIC_ALLOC AML_FORCEINLINE
	const_iterator cend() const noexcept {
		return this->container.cend();
	}

protected:
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
		using left_container = aml::get_container<std::decay_t<decltype(firstvec)>::container_type>;						\
		using right_container = aml::get_container<std::decay_t<decltype(secondvec)>::container_type>;						\
		aml::verify_container_parameters<left_container, right_container>();												\
		aml::Vector<left_container::create<outtype>, aml::dynamic_extent> out(aml::size_initializer<>(firstvec.size()));	\
		dynamic_action																										\
		return out;																											\
	} else if constexpr (firstvec.is_dynamic() || secondvec.is_dynamic()) {													\
		AML_DEBUG_VERIFY(firstvec.size() == secondvec.size(), "Dynamic vector's and static vector's sizes must be equal");	\
		using container_ = aml::get_container<std::conditional_t<firstvec.is_dynamic(), Left, Right>>;						\
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
		using vec_container = aml::get_container<containertype_>;													\
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

template<class T>
using DVector = aml::Vector<detail::DVector_default_container<T>, aml::dynamic_extent>;


AML_NAMESPACE_END
