#include <AML/Complex.hpp>

#include <gtest/gtest.h>
#include <ostream>

namespace aml
{
template<class T>
std::ostream& operator<<(std::ostream& os, const Complex<T>& right) {
	os << '(' << aml::Re(right) << ',' << aml::Im(right) << ")\n";
	return os;
}
}

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


}