
#include <AML/MathAlgorithms.hpp>
#include <gtest/gtest.h>

namespace {

TEST(algorithmsTest, newton_sqrt)
{
	const auto a = aml::algorithms::newton_sqrt<true>(1234567891011121314151617181920.0);
	ASSERT_DOUBLE_EQ(a, 1111111106510560.1);

	const auto b = static_cast<int>(aml::algorithms::newton_sqrt<true>(4));
	ASSERT_EQ(b, 2);

	const auto c = static_cast<int>(aml::algorithms::newton_sqrt<true>(100));
	ASSERT_EQ(c, 10);
}

TEST(algorithmsTest, binary_pow)
{
	const auto a = aml::algorithms::binary_pow(5.65f, 20u);
	ASSERT_FLOAT_EQ(a, 1.0989274E+15f);
}


}