
#include <AML/Functions.hpp>
#include <gtest/gtest.h>

#include <limits>

namespace {

TEST(FunctionsTest, sqr)
{
	ASSERT_EQ(aml::sqr(2), 4);
	ASSERT_FLOAT_EQ(aml::sqr(5.5f), 30.25f);
	ASSERT_DOUBLE_EQ(aml::sqr(3.1415926535897932), 9.8696044010893586);
}

TEST(FunctionsTest, abs)
{
	ASSERT_EQ(aml::abs(int(0)), 0);
	ASSERT_EQ(aml::abs(int(-10)), 10);

	ASSERT_EQ(aml::abs(unsigned(2)), unsigned(2));
	ASSERT_EQ(aml::abs(unsigned(5)), unsigned(5));
	
	ASSERT_EQ(aml::abs<unsigned>(int(5)), unsigned(5));
	ASSERT_EQ(aml::abs<unsigned>(int(-5)), unsigned(5));

	ASSERT_FLOAT_EQ(aml::abs(0.5f), 0.5f);
	ASSERT_FLOAT_EQ(aml::abs(-0.5f), 0.5f);

	ASSERT_DOUBLE_EQ(aml::abs(-0.0), 0.0);
}

TEST(FunctionsTest, max)
{
	ASSERT_EQ(aml::max(5), 5);
	ASSERT_EQ(aml::max(-5), -5);

	ASSERT_EQ(aml::max(-7, 7), 7);
	ASSERT_EQ(aml::max(14, 7), 14);

	ASSERT_EQ(aml::max(1, 2, 3), 3);
	ASSERT_EQ(aml::max(1, 2, 3, 4, 5, 6, 7, 8, 9, 0), 9);

	ASSERT_FLOAT_EQ(aml::max(1.f, 2.f, -100000.f), 2.f);
}

TEST(FunctionsTest, equal)
{
	ASSERT_TRUE(aml::equal(1, 1));
	ASSERT_TRUE(aml::equal(1u, 1u));
	ASSERT_FALSE(aml::equal(2, 6));

	ASSERT_TRUE(aml::equal(1.5f, 1.5f));
	ASSERT_FALSE(aml::equal(-3.f, 4.f));
	ASSERT_TRUE(aml::equal(1, 1.f));
	ASSERT_TRUE(aml::equal(3.0, 3.f));

	ASSERT_FALSE(aml::equal(2.f, 3));
	ASSERT_FALSE(aml::equal(999999.0, -1));

	constexpr auto a = std::numeric_limits<float>::max();
	ASSERT_FALSE(aml::equal(a, a - a / 2.f));
	
	constexpr auto b = std::numeric_limits<double>::epsilon();
	ASSERT_FALSE(aml::equal(b, b * 5));
}

TEST(FunctionsTest, odd_even)
{
	ASSERT_TRUE(aml::odd(3));
	ASSERT_TRUE(aml::even(8));
	ASSERT_FALSE(aml::odd(246));
	ASSERT_FALSE(aml::even(123));
}

TEST(FunctionTest, floor)
{
	ASSERT_EQ(aml::floor<int>(50.5f), 50);
	ASSERT_FLOAT_EQ(aml::floor(123.456f), 123.f);
	ASSERT_FLOAT_EQ(aml::floor(100.f), 100.f);

	ASSERT_EQ(aml::floor<int>(-6.7f), -7);
	ASSERT_FLOAT_EQ(aml::floor(-1000.1f), -1001.f);
	ASSERT_FLOAT_EQ(aml::floor(-500.f), -500.f);

	ASSERT_EQ(aml::floor(50), 50);
	ASSERT_EQ(aml::floor(-20), -20);
}

TEST(FunctionTest, ceil)
{
	ASSERT_EQ(aml::ceil<int>(50.5f), 51);
	ASSERT_FLOAT_EQ(aml::ceil(7890.9f), 7891.f);
	ASSERT_FLOAT_EQ(aml::ceil(100.f), 100.f);

	ASSERT_EQ(aml::ceil<int>(-4.26454f), -4);
	ASSERT_FLOAT_EQ(aml::ceil(-1000000.9f), -1000000.f);
	ASSERT_FLOAT_EQ(aml::ceil(-400.f), -400.f);

	ASSERT_EQ(aml::floor(75), 75);
	ASSERT_EQ(aml::floor(-100), -100);
}

}
