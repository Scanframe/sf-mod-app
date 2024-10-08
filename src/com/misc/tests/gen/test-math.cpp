#include <test/catch.h>

#include <misc/gen/file.h>
#include <misc/gen/pointer.h>
#include <misc/gen/string.h>
#include <unistd.h>

namespace sf
{

}

TEST_CASE("sf::General-Utils", "[con][generic][utils]")
{
	using Catch::Matchers::Equals;

	SECTION("sf::calculateOffset", "offset calculation.")
	{
		CHECK(sf::calculateOffset(10000000ll, -1000000000ll, 1000000000ll, 1000ll, true) == 505ll);
		CHECK(sf::calculateOffset(10, -1000, 1000, 1000ll, true) == 505ll);

		CHECK(sf::calculateOffset(10000000ll, -1000000000ll, 1000000000ll, 1000.0f, true) == 505.0f);
		CHECK(sf::calculateOffset(10, -1000, 1000, 1000.0f, true) == 505.0f);

		CHECK(sf::calculateOffset(10e6, -1e9, 1e9, 1000000000000ll, true) == 505000000000ll);
	}

	SECTION("sf::random", "Random range number.")
	{
		sf::TVector<int> results(2);
		for (auto& i: results)
		{
			i = sf::random(-100, 100);
		}
		//std::clog << "Results: " << results << std::endl;
		CHECK(results[0] != results[1]);
	}

	SECTION("sf::round", "Template function round<T>()")
	{
		CHECK(sf::round(13l, 3l) == 12l);
		CHECK(sf::round(14l, 3l) == 15l);
		CHECK(sf::round(-14l, 3l) == -12l);
		CHECK(sf::round(14ul, 3ul) == 15ul);

		CHECK(sf::round<double>(14.0, 3.0) == 15.0);
		CHECK(sf::round<double>(12.35, 0.25) == 12.25);
		CHECK(sf::round<double>(-12.35, 0.25) == -12.25);
		CHECK(sf::round<float>(14.0, 3.0) == 15);
	}

	SECTION("sf::modulo", "Template function modulo<T>()")
	{
		CHECK(sf::modulo(12, 3) == 0);
		CHECK(sf::modulo(12l, 3l) == 0l);
		CHECK(sf::modulo(12.45, 0.25) == Catch::Approx(0.20).margin(1E-8));
		CHECK(sf::modulo(-12.45, 0.25) == Catch::Approx(0.05).margin(1E-8));
		/*
		CHECK(sf::modulo(14l, 3l) == 15l);
		CHECK(sf::modulo(-14l, 3l) == -12l);
		CHECK(sf::modulo(14ul, 3ul) == 15ul);

		CHECK(sf::modulo<double>(14.0, 3.0) == 15.0);
		CHECK(sf::modulo<double>(12.35, 0.25) == 12.25);
		CHECK(sf::modulo<double>(-12.35, 0.25) == -12.25);
		CHECK(sf::modulo<float>(14.0, 3.0) == 15);
*/
	}

	SECTION("sf::floating-point", "Floating value inquiry template functions")
	{
		// Magnitude of a floating .
		CHECK(sf::magnitude(double(0.001234)) == -2);
		CHECK(sf::magnitude((long double) (-0.001234)) == -2);
		CHECK(sf::magnitude(double(0.123400)) == 0);
		CHECK(sf::magnitude(double(1.23400)) == 1);
		CHECK(sf::magnitude((long double) (123400.0)) == 6);
		CHECK(sf::magnitude(double(-123400.0)) == 6);
		// Digits.
		CHECK(sf::digits(double(12300.0)) == -2);
		CHECK(sf::digits(double(0.0123)) == 4);
		CHECK(sf::digits(double(123.0)) == 0);
		// Precision.
		CHECK(sf::precision(double(12300.0)) == 3);
		CHECK(sf::precision(double(0.0123)) == 3);
		CHECK(sf::precision(double(123.0)) == 3);
	}

	SECTION("sf::null_ref", "null_ref, not_null_ref")
	{
		// Check if not_ref_null reports right value.
		{
			auto& sz(sf::null_ref<size_t>());
			CHECK(sf::not_ref_null(sz));
		}
		// Check if not_ref_null.
		{
			size_t size;
			auto& sz(size);
			CHECK_FALSE(sf::not_ref_null(sz));
		}
	}

	SECTION("sf::toDegrees, sf::toRadials", "Back and forth radial to degrees conversion.")
	{
		// Scope for type T as double.
		{
			typedef double T;
			CHECK(sf::toDegrees<T>(std::numbers::pi_v<T> / 2.0) == Catch::Approx(90.0).margin(std::numeric_limits<T>::epsilon() * 10));
			CHECK(sf::toRadials<T>(90.0) == Catch::Approx(std::numbers::pi_v<T> / 2.0).margin(std::numeric_limits<T>::epsilon() * 10));
		}
		// Scope for type T as long double.
		{
			typedef double T;
			CHECK(sf::toDegrees<T>(std::numbers::pi_v<T> / 2.0) == Catch::Approx(90.0).margin(std::numeric_limits<T>::epsilon() * 10));
			CHECK(sf::toRadials<T>(90.0) == Catch::Approx(std::numbers::pi_v<T> / 2.0).margin(std::numeric_limits<T>::epsilon() * 10));
		}
	}

	SECTION("sf::getExecutableName", "Executable functions")
	{
#if IS_WIN
		std::string name = "t_sf-misc.exe";
#else
		std::string name = "t_sf-misc.bin";
#endif
		REQUIRE(sf::getExecutableName() == name);
	}
}
