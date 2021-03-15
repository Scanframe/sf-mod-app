#include <catch2/catch.hpp>

#include <iostream>
#include <sstream>
#include <misc/gen/Range.h>

TEST_CASE("sf::Range", "[generic][range]")
{
	sf::RangeCompareExact = true;
	using Catch::Equals;

	SECTION("get*/is* Methods Self")
	{
		sf::Range r1(10, 20, 123);
		CHECK(r1.getStop() == 20);
		CHECK(r1.getStart() == 10);
		CHECK(r1.getId() == 123);
		CHECK(r1.getSize() == 10);
		CHECK(r1.isInRange(10));
		CHECK(r1.isInRange(19));
		CHECK_FALSE(r1.isInRange(20));
		CHECK(r1.isWithinSelf(sf::Range(12, 18)));
		CHECK(r1.isWithinOther(sf::Range(5, 25)));
		CHECK(r1.isWithinSelf(sf::Range(10, 20)));
		CHECK(r1.isWithinOther(sf::Range(10, 20)));
	}
		//
	SECTION("is* Methods w/ Other (15, 40)")
	{
		sf::Range r1(10, 20);
		sf::Range r2(15, 40);
		CHECK(r1.compare(r2) == sf::Range::cmpOverlapsSelf);
		CHECK(r1.isOverlapped(r2));
		CHECK_FALSE(r1.isExtension(r2));
		CHECK(r1.isMergeable(r2));
	}
		//
	SECTION("is* Methods w/ Other (20, 40)")
	{
		sf::Range r1(10, 20);
		sf::Range r2(20, 40);
		CHECK(r1.compare(r2) == sf::Range::cmpExtendsSelf);
		CHECK_FALSE(r1.isOverlapped(r2));
		CHECK(r1.isExtension(r2));
		CHECK(r1.isMergeable(r2));
	}
		//
	SECTION("is* Methods w/ Other (30, 40)")
	{
		sf::Range r1(10, 20);
		sf::Range r2(30, 40);
		CHECK(r1.compare(r2) == sf::Range::cmpAfterSelf);
		CHECK_FALSE(r1.isOverlapped(r2));
		CHECK_FALSE(r1.isExtension(r2));
		CHECK_FALSE(r1.isMergeable(r2));
	}
		//
	SECTION("is* Methods w/ Other (40, 40)")
	{
		sf::Range r1(10, 20);
		sf::Range r2(40, 40);
		CHECK(r1.compare(r2) == sf::Range::cmpOtherEmpty);
		CHECK_FALSE(r1.isOverlapped(r2));
		CHECK_FALSE(r1.isExtension(r2));
		CHECK(r1.isMergeable(r2));
	}
		//
	SECTION("Un-/Equal Operators")
	{
		sf::Range r1(10, 20);
		CHECK(r1 == sf::Range(10, 20));
		CHECK_FALSE(r1 != sf::Range(10, 20));
		CHECK(r1 != sf::Range(10, 21));
		CHECK_FALSE(r1 == sf::Range(10, 21));
	}
		//
	SECTION("Addition Operator")
	{
		CHECK(sf::Range(10, 20) + sf::Range(15, 40) == sf::Range(10, 40));
		// Adding 2 ranges with a gap in between results in a range encapsulating both.
		CHECK(sf::Range(10, 20) + sf::Range(30, 40) == sf::Range(10, 40));
	}

	SECTION("And Operator")
	{
		// And operator resulting in a range that is within both ranges.
		CHECK((sf::Range(20, 30) & sf::Range(10, 40)) == sf::Range(20, 30));
		CHECK((sf::Range(10, 20) & sf::Range(15, 40)) == sf::Range(15, 20));
		CHECK((sf::Range(15, 40) & sf::Range(10, 20)) == sf::Range(15, 20));
		CHECK((sf::Range(10, 40) & sf::Range(20, 30)) == sf::Range(20, 30));
		CHECK((sf::Range(10, 20) & sf::Range(30, 40)) == sf::Range(0, 0));
	}

	SECTION("Less Then Operator")
	{
		// And operator resulting in a range that is within both ranges.
		CHECK(sf::Range(10, 40) < sf::Range(0, 0));
		CHECK(sf::Range(10, 20) < sf::Range(15, 40));
		CHECK_FALSE(sf::Range(15, 40) < sf::Range(10, 20));
		CHECK(sf::Range(10, 40) < sf::Range(20, 30));
		CHECK(sf::Range(10, 20) < sf::Range(30, 40));
	}

	SECTION("Exclude Function")
	{
		sf::Range r, rest;

		// Single result.
		CHECK(r.assign(10, 40).exclude({80, 90}, rest) == 1);
		CHECK(r == sf::Range(10, 40));

		// Double results.
		REQUIRE(r.assign(10, 40).exclude({20, 30}, rest) == 2);
		CHECK(r == sf::Range(10, 20));
		CHECK(rest == sf::Range(30, 40));

		// Single result.
		CHECK(r.assign(10, 40).exclude({20, 40}, rest) == 1);
		CHECK(r == sf::Range(10, 20));

		// No results.
		CHECK(r.assign(10, 40).exclude({10, 40}, rest) == 0);
		CHECK(r == sf::Range(0, 0));

		// No results.
		CHECK(r.assign(10, 40).exclude({0, 50}, rest) == 0);
		CHECK(r == sf::Range(0, 0));

		// Single result.
		CHECK(r.assign(10, 40).exclude({0, 20}, rest) == 1);
		CHECK(r == sf::Range(20, 40));

		// Single result.
		CHECK(r.assign(10, 40).exclude({10, 30}, rest) == 1);
		CHECK(r == sf::Range(30, 40));
	}

	SECTION("Special")
	{
		sf::Range r1(55, 60);
		sf::Range r2(50, 60);
		CHECK(r1.compare(r2) == sf::Range::cmpWithinOther);
	}

	sf::RangeCompareExact = false;
}

TEST_CASE("sf::RangeVector", "[generic][range]")
{
	sf::RangeCompareExact = true;

	SECTION("Vector Method Checks")
	{
		//std::clog << "Range: " << os.str() << std::endl;

		SECTION("Splitting into segments size 15")
		{
			sf::Range r1(10, 20);
			sf::Range::Vector rl;
			r1.split(15, rl);
			CHECK(rl == sf::Range::Vector{{{10, 15, 0}, {15, 20, 1}}});
		}

		SECTION("Splitting in segments size 4")
		{
			sf::Range r1(10, 20);
			sf::Range::Vector rl;
			r1.split(4, rl);
			CHECK(rl == sf::Range::Vector{{{10, 12, 2}, {12, 16, 3}, {16, 20, 4}}});
		}

		SECTION("Rearrange Vector")
		{
			int i = 1;
			sf::Range::Vector rl{
				{
					{10, 0, i++},
					{30, 40, i++},
					{90, 60, i++},
					{20, 35, i++},
					{0, 0, i++},
					{50, 70, i++}
				}
			};
			rl.rearrange();
			CHECK(rl == (sf::Range::Vector{{{0, 10, 1}, {20, 40, 4}, {50, 90, 6}}}));
		}

		SECTION("Exclude Vector")
		{
			sf::Range::Vector rel{
				{
					{0, 20},
					{30, 40},
					{50, 60},
					{70, 80}
				}
			};
			//
			sf::Range::Vector r0{{{20, 30, -1}, {40, 50, -1}}};
			sf::Range::Vector ex{{{40, 50}}};
			CHECK(r0.exclude(ex) == sf::Range::Vector{{{20, 30, -1}}});
			//
			sf::Range::Vector r1{{{10, 65, -1}}};
			CHECK(r1.exclude(rel) == (sf::Range::Vector{{{20, 30, -1}, {40, 50, -1}, {60, 65, -1}}}));
			//
			sf::Range::Vector r2{{{40, 70, -2}}};
			CHECK(r2.exclude(rel) == (sf::Range::Vector{{{40, 50, -2}, {60, 70, -2}}}));
			//
			sf::Range::Vector r3{{{30, 60, -3}}};
			CHECK(r3.exclude(rel) == (sf::Range::Vector{{{40, 50, -3}}}));
			//
			sf::Range::Vector r4{{{30, 60, -1}, {65, 90, -2}}};
			CHECK(r4.exclude(rel) == (sf::Range::Vector{{{40, 50, -1}, {65, 70, -2}, {80, 90, -2}}}));
		}

		SECTION("Extract Vector")
		{
			sf::Range::Vector rl{
				{
					{0, 20, -1},
					{30, 40, -2},
					{30, 40, -7},
					{50, 60, -3},
					{70, 80, -4},
					{70, 80, -6},
					{85, 90, -5}
				}
			};
			sf::Range::Vector covered{{{15, 55}, {65, 95}}};
			sf::Range::Vector extracted;
			rl.extract(covered, extracted);
			CHECK(extracted == sf::Range::Vector{{{30, 40, -2}, {30, 40, -7},{70, 80, -4}, {70, 80, -6}, {85, 90, -5}}});
		}
	}

	sf::RangeCompareExact = false;
}

