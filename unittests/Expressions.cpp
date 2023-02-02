#include <AML/Expressions.hpp>

#include <gtest/gtest.h>

namespace {

TEST(Expressions, init)
{
	static constexpr aml::Expression a("1.5 * 2 + (1/2)*(2/1) - 1");

	static constexpr auto res = a.calculate();
	
	ASSERT_DOUBLE_EQ(res, 3.0);
}




}