
#include "Testing.hpp"

#include <AML/Complex.hpp>

namespace {

DEFINE_TEST(complex_init)
{
	DEFINE_VAR aml::Complex c1(100);
	TEST_EQUALS(aml::Re(c1), 100);
	TEST_EQUALS(aml::Im(c1), 0);

	DEFINE_VAR aml::Complex c2(1, 2);
	TEST_EQUALS(aml::Re(c2), 1);
	TEST_EQUALS(aml::Im(c2), 2);
}

DEFINE_TEST(complex_copy)
{
	DEFINE_VAR aml::Complex c1(-1, -2);
	TEST_EQUALS(aml::Re(c1), -1);
	TEST_EQUALS(aml::Im(c1), -2);

	DEFINE_VAR aml::Complex c2 = c1;
	TEST_EQUALS(aml::Re(c2), -1);
	TEST_EQUALS(aml::Im(c2), -2);

	TEST_EQUALS(c2, c1);
}

DEFINE_TEST(complex_cast)
{
	DEFINE_VAR aml::Complex c1(1.5f, -0.5f);
	
	constexpr auto c2 = static_cast<aml::Complex<int>>(c1);

	TEST_EQUALS(c2, aml::Complex(1, 0));
}

DEFINE_TEST(complex_Re_Im)
{
	DEFINE_VAR aml::Complex c1(1, 2);

	TEST_EQUALS(aml::Re(c1), 1);
	TEST_EQUALS(aml::Im(c1), 2);

	TEST_EQUALS(aml::Re(100), 100);
	TEST_EQUALS(aml::Im(1e10f), 0);
}

DEFINE_TEST(complex_operators)
{
	DEFINE_VAR aml::Complex c1(4.5f, 2.f);
	DEFINE_VAR aml::Complex c2(1, -1);

	TEST_EQUALS((c1 + c2),	aml::Complex(5.5f, 1.f));
	TEST_EQUALS((c2 + c1),	(c1 + c2));
	TEST_EQUALS((c1 + 2),	aml::Complex(6.5f, 2.f));
	TEST_EQUALS((5.f + c2), aml::Complex(6.f, -1.f));

	TEST_EQUALS((c1 - c2),	aml::Complex(3.5f, 3.f));
	TEST_EQUALS((c2 - c1),	-(c1 - c2));
	TEST_EQUALS((c1 - 2.f),	aml::Complex(2.5f, 2.f));
	TEST_EQUALS((1.2f - c2),aml::Complex(0.2f, 1.f));

	TEST_EQUALS((c1 * c2),	aml::Complex(6.5f, -2.5f));
	TEST_EQUALS((c2 * c1),	(c1 * c2));
	TEST_EQUALS((c1 * 2.0),	aml::Complex(9.0, 4.0));
	TEST_EQUALS((3 * c2),	aml::Complex(3, -3));

	TEST_EQUALS((c1 / c2),	aml::Complex(1.25f, 3.25f));
	TEST_EQUALS((c2 / c1),	(1 / (c1 / c2)));
	TEST_EQUALS((c1 / 1.25f),aml::Complex(3.6f, 1.6f));
	TEST_EQUALS((1.f / c2),	aml::Complex(0.5f, 0.5f));

	FORCE_COMPILE_TIME({
		aml::Complex c = c1;
		c += c2;
		c -= aml::Complex(1);
		if (c != aml::Complex(4.5f, 1.f)) throw 0;
	});

	FORCE_COMPILE_TIME({
		aml::Complex c = c2;
		c *= c;
		c /= c2;
		c *= 10;
		if (c != (c2 * 10)) throw 0;
	});

	TEST_EQUALS(-c1, aml::Complex(-4.5f, -2.f));
}

DEFINE_TEST(complex_functions)
{
	DEFINE_VAR aml::Complex c1(3, 4);

	TEST_EQUALS(aml::abs(c1), 5.f);
	TEST_EQUALS(aml::sqrt(c1), aml::Complex(2.f, 1.f));

	TEST_EQUALS(aml::csqrt(c1), aml::sqrt(c1));
	TEST_EQUALS(aml::csqrt(-1), aml::Complex(0.f, 1.f));

	TEST_EQUALS(aml::normalize(aml::Complex(1, 1)), aml::Complex(0.70710678f, 0.70710678f));
}

DEFINE_TEST(complex_structured_binding)
{
	FORCE_COMPILE_TIME({
		aml::Complex c(1, -2);

		const auto& [r, i] = c;

		if (r != 1) throw 0;
		if (i != -2) throw 0;

		if (aml::Complex(r, i) != c) throw 0;
	});
}

DEFINE_TEST(complex_reference_type)
{
	FORCE_COMPILE_TIME({
		int r = 100, i = 10;
		aml::Complex<int&> c(r, i);

		if (aml::Re(c) != 100) throw 0;
		if (aml::Im(c) != 10) throw 0;

		r = -1;
		i = 0;

		if (aml::Re(c) != -1) throw 0;
		if (aml::Im(c) != 0) throw 0;
	});
}

static_assert(std::is_same_v<
	aml::common_type<aml::Complex<int>, aml::Complex<unsigned>>,
	aml::Complex<unsigned>
>);

static_assert(std::is_same_v<
	aml::common_type<aml::Complex<int>, float>,
	aml::Complex<float>
>);

static_assert(std::is_same_v<
	aml::common_type<long long, aml::Complex<long>>,
	aml::Complex<long long>
>);

}

#include <AML/Vector.hpp>

namespace {

DEFINE_TEST(complex_vector)
{
	DEFINE_VAR aml::Complex cv1(aml::Vector(1, 2));
	DEFINE_VAR aml::Complex c2(5, -2);

	using aml::Vector;

	TEST_EQUALS((cv1 * c2), aml::Complex(Vector(5, 10), Vector(-2, -4)));
	TEST_EQUALS((cv1 / 2.5f), aml::Complex(Vector(0.4f, 0.8f)));

	TEST_EQUALS((cv1 - Vector(1, 2)), aml::zero);
}

}
