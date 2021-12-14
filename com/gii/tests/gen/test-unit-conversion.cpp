#include <test/catch.h>

#include <gii/gen/UnitConversionServer.h>

namespace {

const char* IniContent = R"(

[Metric]
m/s,0=m/s,1,0,0
m/s,2=mm/s,1000,0,-1
m/s,3=mm/s,1000,0,0
m/s,4=mm/s,1000,0,1
m/s,5=mm/s,1000,0,2

rad,0=deg,57.2958,0,0
rad,1=deg,57.2958,0,0
rad,2=deg,57.2958,0,1
rad,3=deg,57.2958,0,2
rad,4=deg,57.2958,0,3
rad,5=deg,57.2958,0,4
rad,6=deg,57.2958,0,5

[Imperial]
m/s,0="/s,39.37007874015748031496062992126,0,0
m/s,1="/s,39.37007874015748031496062992126,0,1
m/s,2="/s,39.37007874015748031496062992126,0,2
m/s,3="/s,39.37007874015748031496062992126,0,3
m/s,4="/s,39.37007874015748031496062992126,0,4
m/s,5="/s,39.37007874015748031496062992126,0,5

rad,0=deg,57.2958,0,0
rad,1=deg,57.2958,0,0
rad,2=deg,57.2958,0,1
rad,3=deg,57.2958,0,2
rad,4=deg,57.2958,0,3
rad,5=deg,57.2958,0,4
rad,6=deg,57.2958,0,5
°C,1=°F,1.8,32,1

)";

}

TEST_CASE("sf::UnitConversion", "[unit][conversion]")
{
	using Catch::Matchers::Equals;
	using Catch::Matchers::Matches;
	using Catch::Approx;

	std::istringstream is(IniContent);
	sf::UnitConversionServer ucs;
	ucs.load(is);

	double multiplier{0};
	double offset{0};
	std::string unit;
	int precision{0};

	SECTION("Ini-File")
	{
		ucs.setUnitSystem(sf::UnitConversionServer::usMetric);

		const_cast<sf::IniProfile&>(ucs.getProfile()).setSection("Metric");

		CHECK(ucs.getProfile().getSection() == "Metric");

		CHECK(ucs.getProfile().getString("m/s,5") == "mm/s,1000,0,2");
	}

	SECTION("Query Metric")
	{
		ucs.setUnitSystem(sf::UnitConversionServer::usMetric);

		REQUIRE(sf::getUnitConversion("", "m/s", 5, multiplier, offset, unit, precision));
		//std::clog << "New U:" << unit << ", M:" << multiplier << ", M:" << offset << ", P:" << precision << std::endl;
		REQUIRE(multiplier == Approx(1000.0).margin(0.001));
		REQUIRE(offset == Approx(0.0).margin(0.001));
		REQUIRE(unit == "mm/s");
		REQUIRE(precision == 2);

		REQUIRE(sf::getUnitConversion("", "rad", 5, multiplier, offset, unit, precision));
		//std::clog << "New U:" << unit << ", M:" << multiplier << ", M:" << offset << ", P:" << precision << std::endl;
		REQUIRE(multiplier == Approx(57.2958).margin(0.001));
		REQUIRE(offset == Approx(0.0).margin(0.001));
		REQUIRE(unit == "deg");
		REQUIRE(precision == 4);

		REQUIRE_FALSE(sf::getUnitConversion("", "\U000000B0C", 1, multiplier, offset, unit, precision));
		//std::clog << "New U:" << unit << ", M:" << multiplier << ", M:" << offset << ", P:" << precision << std::endl;
	}

	SECTION("Query Imperial")
	{
		ucs.setUnitSystem(sf::UnitConversionServer::usImperial);

		sf::getUnitConversion("", "\U000000B0C", 1, multiplier, offset, unit, precision);
		//std::clog << "New U:" << unit << ", M:" << multiplier << ", M:" << offset << ", P:" << precision << std::endl;
		REQUIRE(multiplier == Approx(1.8).margin(0.0000001));
		REQUIRE(offset == Approx(32).margin(0.0000001));
		REQUIRE(unit == "\U000000B0F");
		REQUIRE(precision == 1);

	}
}
