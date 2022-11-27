#pragma once

#include <AML/_AMLCore.hpp>

#include <cstddef>

AML_NAMESPACE

using Vectorsize = std::size_t;

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



};

AML_NAMESPACE_END
