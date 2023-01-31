#include <gtest/gtest.h>

#include <AML/Algorithms/Newtons_method.hpp>

namespace {

TEST(Newtons_method_test, raw)
{
	const auto fun = [](auto x) {
		return x - ((aml::sqr(x) + -11 * x + 10) / (2 * x - 11));
	};

	auto res1 = aml::algorithms::raw_newtons_method<int>(fun);
	ASSERT_FLOAT_EQ(res1, 1.f);

	auto res2 = aml::algorithms::raw_newtons_method(fun, 100);
	ASSERT_FLOAT_EQ(res2, 10.f);
}

TEST(Newtons_method_test, strong)
{
	const auto fun = [](auto x) {
		return x - (-123 * aml::sqr(x) + 456 * x + 789) / (456 - 246 * x);
	};

	auto res1 = aml::algorithms::raw_strong_newtons_method<int>(fun);
	ASSERT_FLOAT_EQ(res1, -1.2849214f);

	auto res2 = aml::algorithms::raw_strong_newtons_method(fun, 100);
	ASSERT_FLOAT_EQ(res2, 4.99223848f);
}

TEST(Newtons_method_test, with_max_iter)
{
	const auto fun = [](auto x) {
		return x - ((aml::sqr(x) + -11 * x + 10) / (2 * x - 11));
	};

	auto res1 = aml::algorithms::raw_newtons_method_with_max_iteration<int>(fun, 10u);
	ASSERT_FLOAT_EQ(res1, 1.f);

	auto res2 = aml::algorithms::raw_newtons_method_with_max_iteration<int>(fun, 10u, 100);
	ASSERT_FLOAT_EQ(res2, 10.f);
}


}