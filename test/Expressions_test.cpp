#include "Testing.hpp"

#include <AML/Expressions.hpp>

#include <gtest/gtest.h>

namespace {

TEST(aa, expressions) 
{
	static constexpr aml::Expression expr1{"1.5 * x + (1/2)*(2/1) - 1"};
	static constexpr auto result1 = expr1.function();
	static constexpr auto res = result1(2);
	ASSERT_EQ(res, 3.0);
}

}

