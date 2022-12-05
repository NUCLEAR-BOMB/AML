
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

}
