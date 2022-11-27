#include <gtest/gtest.h>

#include <AML/Vector.hpp>

using namespace aml;

namespace {

	template<class> struct VectorSuite : testing::Test{};
	using VectorTypeList = testing::Types<int, float, char, double>;

	TYPED_TEST_SUITE(VectorSuite, VectorTypeList);

	TYPED_TEST(VectorSuite, initUsingArray) {
		Vector<TypeParam, 1> a({static_cast<TypeParam>(2)});
		Vector<TypeParam, 2> b({static_cast<TypeParam>(3), static_cast<TypeParam>(4)});
		Vector<TypeParam, 3> c({static_cast<TypeParam>(100), static_cast<TypeParam>(-100), static_cast<TypeParam>(200)});
	}

	TEST(VectorTest, checkSize) {
		Vector<int, 1> a({200});
		ASSERT_EQ(a.size(), 1);

		Vector<int, 2> b({-5, 1});
		ASSERT_EQ(b.size(), 2);

		Vector<int, 4> c({0, 1, 2, 3});
		ASSERT_EQ(c.size(), 4);
	}

	TEST(VectorTest, static_call_index_operator) {
		Vector<int, 2> a({500, 1000});
		ASSERT_EQ(a.operator[](VI::X), 500);
		ASSERT_EQ(a.operator[](VI::Y), 1000);
		const Vector<unsigned int, 3> b({300, 800, 200});
		ASSERT_EQ(b.operator[](VI::X), 300);
		ASSERT_EQ(b.operator[](VI::Y), 800);
		ASSERT_EQ(b.operator[](VI::Z), 200);
	}

	int main(int argc, char **argv)
	{
		::testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
}
