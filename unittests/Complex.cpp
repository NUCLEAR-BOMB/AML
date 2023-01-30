#include <AML/Complex.hpp>

#include <gtest/gtest.h>

using namespace aml;
namespace {

TEST(ComplexTest, init)
{
	const Complex a(100);
	EXPECT_EQ(aml::Re(a), 100);
	EXPECT_EQ(aml::Im(a), 0);

	Complex b(1, 2);
	EXPECT_EQ(aml::Re(b), 1);
	EXPECT_EQ(aml::Im(b), 2);
}

TEST(ComplexTest, init_with_special_modifiers)
{
	const Complex<int> a(aml::zero);
	EXPECT_EQ(a, Complex(0, 0));
	
	const Complex<int> b(aml::one);
	EXPECT_EQ(b, Complex(1, 1));

	const Complex<int> c(aml::unit<0>);
	EXPECT_EQ(c, Complex(1, 0));

	const Complex<int> d(aml::unit<1>);
	EXPECT_EQ(d, Complex(0, 1));
}

TEST(ComplexTest, cast)
{
	const Complex a(2.5, 100.0);

	const Complex b = static_cast<Complex<int>>(a);

	const Complex c(a);
}

TEST(ComplexTest, Re_and_Im)
{
	const Complex a(1, 2);

	EXPECT_EQ(aml::Re(a), 1);
	EXPECT_EQ(aml::Im(a), 2);

	EXPECT_EQ(aml::Re(100), 100);
	EXPECT_EQ(aml::Im(12313231), 0);
}

TEST(ComplexTest, operators)
{
	const Complex a(1, 2);
	const Complex b(-1, -2);
	Complex c = a;

	// +
	EXPECT_EQ((a + b), Complex(0, 0));
	EXPECT_EQ((a + 10), Complex(11, 2));
	EXPECT_EQ((10 + b), Complex(9, -2));

	c += -1;
	EXPECT_EQ(c, Complex(0, 2));
	c += b;
	EXPECT_EQ(c, Complex(-1, 0));

	// -
	EXPECT_EQ((a - b), Complex(2, 4));
	EXPECT_EQ((b - 2), Complex(-3, -2));
	EXPECT_EQ((2 - b), Complex(3, 2));

	c -= 5;
	EXPECT_EQ(c, Complex(-6, 0));
	c -= a;
	EXPECT_EQ(c, Complex(-7, -2));

	// *
	EXPECT_EQ((a * 2), Complex(2, 4));
	EXPECT_EQ((-2 * a), Complex(-2, -4));
	EXPECT_EQ((a * b), Complex(3, -4));
	
	c *= 6;
	EXPECT_EQ(c, Complex(-42, -12));

	c *= b;
	EXPECT_EQ(c, Complex(18, 96));
	
	// /
	EXPECT_EQ((a / 2), Complex(0, 1));
	EXPECT_EQ((10 / a), Complex(2, -4));
	EXPECT_EQ((a / b), Complex(-1, 0));

	c /= 7;
	EXPECT_EQ(c, Complex(2, 13));
	c /= a;
	EXPECT_EQ(c, Complex(5, 1));

}

TEST(ComplexTest, abs)
{
	const Complex a(3, 4);

	EXPECT_FLOAT_EQ(aml::abs(a), 5.f);
}

TEST(ComplexTest, sqrt)
{
	const Complex a(10, -10);

	auto b = aml::sqrt(a);

	EXPECT_EQ(b, Complex(3.4743442f, -1.43912049f));
}

TEST(ComplexTest, structured_binding)
{
	{
		const Complex a(1, -2);

		const auto& [r, i] = a;
		
		EXPECT_EQ(r, 1);
		EXPECT_EQ(i, -2);
	}
	{
		Complex a(123.456f, 789.112f);

		auto& [r, i] = a;

		r = 1.f;
		i = r;

		EXPECT_FLOAT_EQ(r, 1.f);
		EXPECT_FLOAT_EQ(i, 1.f);
	}
}

TEST(ComplexTest, lvalue_reference)
{
	int r = 100, i = 10;

	Complex<int&> a(r, i);

	EXPECT_EQ(aml::Re(a), 100);
	EXPECT_EQ(aml::Im(a), 10);

	r = 0;
	i = -1;

	EXPECT_EQ(aml::Re(a), 0);
	EXPECT_EQ(aml::Im(a), -1);
}


}