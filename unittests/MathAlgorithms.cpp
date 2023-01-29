
#include <AML/MathAlgorithms.hpp>
#include <gtest/gtest.h>

namespace {


TEST(algorithmsTest, binary_pow)
{
	const auto a = aml::algorithms::binary_pow(5.65f, 20u);
	ASSERT_FLOAT_EQ(a, 1.0989274E+15f);
}


}