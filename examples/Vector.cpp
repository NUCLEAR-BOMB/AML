#include <AML/Vector.hpp>

int main() 
{
	// Creating a three-dimensional vector
	//          vvv Type of elements in a vector
	aml::Vector<int, 3> a(1, -2, 3); // <<< initializes the vector with values: 1, -2, 3
	//    Vector size vvv
	aml::Vector<float, 3> b(10.f, 5.1f, 2.5f);

	// Sum up the vectors
	auto c = a + b;

	// Apply the dot product of vectors
	//  vvv scalar
	auto d = aml::dot(c, a);
	
	// You can also do it like this
	//  vvvvvvvvv
	using aml::dot;
	d = c <dot> a;
	
	// Apply cross product 
	//  vvv 3D vector
	auto f = aml::cross(a, b);
	
	// And with aml::cross too
	//  vvvvvvvvvvv
	using aml::cross;
	f = a <cross> b;

	// Cast vector "b" to the type aml::Vector<int, 3>, which has vector "a"
	a = static_cast<aml::Vector<int, 3>>(b);

	// Check if "a" and "b" vectors are equal
	bool is_a_equal_b = (a == b);

	// Multiply vector "a" by a scalar and place the result in "a"
	a *= 10;

	// Normalizes the vector "b"
	auto g = aml::normalize(b);

	// Multiply vector "g" by a vector "a" length
	auto h = g * aml::dist(a);

	// Cross product with unit vectors (0, 1, 0) and (0, 0, 1)
	//  vvv (1, 0, 0)
	auto i = aml::cross(aml::Vector<int, 3>(aml::unit<1>), aml::Vector<int, 3>(aml::unit<2>));
}

// untested but should work