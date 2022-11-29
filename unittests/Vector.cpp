
#include <AML/Vector.hpp>
#include <gtest/gtest.h>

using namespace aml;

namespace {

TEST(VectorTest, init_using_array) {
	Vector<int, 1> a({ 2 });
	Vector<int, 2> b({ 3, 4 });
	Vector<int, 3> c({ 100, -100, 200 });

	Vector<int, 6> d({ 1, 2, 3, 4, 5, 6 });
}

TEST(VectorTest, init_using_variadic) {
	Vector<int, 2> a(1, 2);
	Vector<int, 3> b(3, 4, 5);

	Vector<int, 6> c(1, 2, 3, 4, 5, 6);
}

TEST(VectorTest, checkSize) {
	Vector<int, 1> a({ 200 });
	EXPECT_EQ(a.size(), 1);

	Vector<int, 2> b({ -5, 1 });
	EXPECT_EQ(b.size(), 2);

	const Vector<int, 4> c({ 0, 1, 2, 3 });
	EXPECT_EQ(c.size(), 4);

	Vector<int, 6> d({ 1, 2, 3, 4, 5, 6 });
	EXPECT_EQ(d.size(), 6);

	const Vector<int, 10> f({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
	EXPECT_EQ(f.size(), 10);
}

TEST(VectorTest, static_call_index_operator) {
	Vector<int, 2> a({ 500, 1000 });
	EXPECT_EQ(a[VI::X], 500);
	EXPECT_EQ(a[VI::Y], 1000);

	const Vector<unsigned int, 3> b({ 300, 800, 200 });
	EXPECT_EQ(b[VI::X], 300);
	EXPECT_EQ(b[VI::Y], 800);
	EXPECT_EQ(b[VI::Z], 200);

	Vector<int, 7> c({ 1, 2, 3, 4, 5, 6, 7 });
	EXPECT_EQ(c[VI::X], 1);
	EXPECT_EQ(c[VI::Y], 2);
	EXPECT_EQ(c[VI::Z], 3);
	EXPECT_EQ(c[VI::Index<6>{}], 7);
}

TEST(VectorTest, runtime_call_index_operator) {
	Vector<int, 2> a({ 1, 2 });
	EXPECT_EQ(a[0], 1);
	EXPECT_EQ(a[1], 2);

	Vector<unsigned int, 4> b({ 4, 5, 6, 7 });
	EXPECT_EQ(b[0], 4);
	EXPECT_EQ(b[1], 5);
	EXPECT_EQ(b[2], 6);
	EXPECT_EQ(b[3], 7);

	Vector<int, 8> c({ 1, 2, 3, 4, 5, 6, 7, 8 });
	EXPECT_EQ(c[0], 1);
	EXPECT_EQ(c[1], 2);
	EXPECT_EQ(c[2], 3);
	EXPECT_EQ(c[6], 7);
}

TEST(VectorTest, iterate_constant) {
	const Vector<int, 4> a(10, 20, 30, 40);

	std::size_t ai = 0;
	for (const auto& i : a) {
		if (ai == 0) EXPECT_EQ(i, 10);
		else if (ai == 1) EXPECT_EQ(i, 20);
		else if (ai == 2) EXPECT_EQ(i, 30);
		else if (ai == 3) EXPECT_EQ(i, 40);
		++ai;
	}

	ai = 0;
	for (auto i = a.begin(); i != a.end(); ++i) {

		if (ai == 0) EXPECT_EQ(*i, 10);
		else if (ai == 1) EXPECT_EQ(*i, 20);
		else if (ai == 2) EXPECT_EQ(*i, 30);
		else if (ai == 3) EXPECT_EQ(*i, 40);

		++ai;
	}

	const Vector<int, 7> b(13, 23, 33, 43, 53, 63, 73);

	std::size_t bi = 0;
	for (const auto& i : b) {
		if (bi == 0) EXPECT_EQ(i, 13);
		else if (bi == 1) EXPECT_EQ(i, 23);
		else if (bi == 2) EXPECT_EQ(i, 33);
		else if (bi == 3) EXPECT_EQ(i, 43);
		else if (bi == 4) EXPECT_EQ(i, 53);
		else if (bi == 5) EXPECT_EQ(i, 63);
		else if (bi == 6) EXPECT_EQ(i, 73);

		++bi;
	}

	bi = 0;
	for (auto i = b.begin(); i != b.end(); ++i) {
		if (bi == 0) EXPECT_EQ(*i, 13);
		else if (bi == 1) EXPECT_EQ(*i, 23);
		else if (bi == 2) EXPECT_EQ(*i, 33);
		else if (bi == 3) EXPECT_EQ(*i, 43);
		else if (bi == 4) EXPECT_EQ(*i, 53);
		else if (bi == 5) EXPECT_EQ(*i, 63);
		else if (bi == 6) EXPECT_EQ(*i, 73);

		++bi;
	}
}

TEST(VectorTest, iterate) {
	Vector<int, 3> a(10, 20, 30);

	std::size_t ai = 0;
	for (auto& i : a) {
		if (ai == 0) EXPECT_EQ(i, 10);
		else if (ai == 1) EXPECT_EQ(i, 20);
		else if (ai == 2) EXPECT_EQ(i, 30);
		++ai;
	}

	ai = 0;
	for (auto& i : a) {
		i = static_cast<int>(ai * 10);

		if (ai == 0) EXPECT_EQ(i, 0);
		else if (ai == 1) EXPECT_EQ(i, 10);
		else if (ai == 2) EXPECT_EQ(i, 20);
		++ai;
	}

	Vector<int, 7> b(11, 21, 31, 41, 51, 61, 71);

	std::size_t bi = 0;
	for (auto& i : b) {
		if (bi == 0) EXPECT_EQ(i, 11);
		else if (bi == 1) EXPECT_EQ(i, 21);
		else if (bi == 2) EXPECT_EQ(i, 31);
		else if (bi == 3) EXPECT_EQ(i, 41);
		else if (bi == 4) EXPECT_EQ(i, 51);
		else if (bi == 5) EXPECT_EQ(i, 61);
		else if (bi == 6) EXPECT_EQ(i, 71);
		++bi;
	}

	bi = 0;
	for (auto& i : b) {
		i = static_cast<int>(bi * 11);

		if (bi == 0) EXPECT_EQ(i, 0);
		else if (bi == 1) EXPECT_EQ(i, 11);
		else if (bi == 2) EXPECT_EQ(i, 22);
		else if (bi == 3) EXPECT_EQ(i, 33);
		else if (bi == 4) EXPECT_EQ(i, 44);
		else if (bi == 5) EXPECT_EQ(i, 55);
		else if (bi == 6) EXPECT_EQ(i, 66);
		++bi;
	}
}

}
