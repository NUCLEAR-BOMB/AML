#pragma once

#include <AML/_AMLCore.hpp>

#include <cstddef>

AML_NAMESPACE

using Vectorsize = std::size_t;

namespace VI
{
	template<Vectorsize I>
	struct Index {};

	inline constexpr Index<0> X;
	inline constexpr Index<1> Y;
	inline constexpr Index<2> Z;
	inline constexpr Index<3> W;
	inline constexpr Index<4> V;
}

namespace detail
{
	template<class T, Vectorsize S>
	struct VectorStorage {
	protected:
		T array[S];
		static constexpr Vectorsize size = S;
	};

	template<class T>
	struct VectorStorage<T, 1> {
		T x;
	protected:
		static constexpr Vectorsize size = 1;
	};

	template<class T>
	struct VectorStorage<T, 2> {
		T x, y;
	protected:
		static constexpr Vectorsize size = 2;
	};

	template<class T>
	struct VectorStorage<T, 3> {
		T x, y, z;
	protected:
		static constexpr Vectorsize size = 3;
	};

	template<class T>
	struct VectorStorage<T, 4> {
		T x, y, z, w;
	protected:
		static constexpr Vectorsize size = 4;
	};

	template<class T>
	struct VectorStorage<T, 5> {
		T x, y, z, w, v;
	protected:
		static constexpr Vectorsize size = 5;
	};

}

template<class T, Vectorsize Size>
class Vector : public detail::VectorStorage<T, Size>
{
	using Storage = detail::VectorStorage<T, Size>;
public:

	using size_type = Vectorsize;
	using value_type = T;

	using reference = T&;
	using const_reference = const T&;

	constexpr
	Vector(const T (&Array)[Size]) noexcept {
		if constexpr (Storage::size >= 1) Storage::x = Array[0];
		if constexpr (Storage::size >= 2) Storage::y = Array[1];
		if constexpr (Storage::size >= 3) Storage::z = Array[2];
		if constexpr (Storage::size >= 4) Storage::w = Array[3];
		if constexpr (Storage::size >= 5) Storage::v = Array[4];
	}

	[[nodiscard]] constexpr
	size_type size() const noexcept {
		return Storage::size;
	}

	template<Vectorsize I> constexpr
	reference operator[](VI::Index<I>) noexcept {
			 if constexpr (I == 0) return Storage::x;
		else if constexpr (I == 1) return Storage::y;
		else if constexpr (I == 2) return Storage::z;
		else if constexpr (I == 3) return Storage::w;
		else if constexpr (I == 4) return Storage::v;
	}

	template<Vectorsize I> constexpr
	const_reference operator[](VI::Index<I>) const noexcept {
		return const_cast<Vector&>(*this)[VI::Index<I>{}];
	}

};

AML_NAMESPACE_END
