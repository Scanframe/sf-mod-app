#include <gii/gen/UnitConversionServerEx.h>
#include <misc/gen/math.h>
#include <test/catch.h>

namespace
{

const char* IniContent = R"(

[Followers]

0x10='x',m,0x100

; <from-unit>,<from-precision>=<to-unit>,<multiplier>,<offset>,<to-precision>

[Metric]
m/s,-1=m/s,1,0,0
m/s,0=m/s,1,0,0
m/s,1=mm/s,1000,0,-2
m/s,2=mm/s,1000,0,-1
m/s,3=mm/s,1000,0,0
m/s,4=mm/s,1000,0,1
m/s,5=mm/s,1000,0,2

m,-3=m,1,0,-3
m,-2=m,1,0,-2
m,1=mm,1,0,-1
m,2=mm,1000,0,-1
m,3=mm,1000,0,0
m,4=mm,1000,0,1
m,5=mm,1000,0,2
m,6=mm,1000,0,3

s,0=s,1,0,0
s,1=s,1,0,1
s,2=s,1,0,2
s,3=ms,1e3,0,0
s,4=ms,1e3,0,1
s,5=ms,1e3,0,2
s,6=µs,1e6,0,0
s,7=µs,1e6,0,1
s,8=µs,1e6,0,2
s,9=µs,1e6,0,3

rad,0=deg,57.2958,0,0
rad,1=deg,57.2958,0,0
rad,2=deg,57.2958,0,1
rad,3=deg,57.2958,0,2
rad,4=deg,57.2958,0,3
rad,5=deg,57.2958,0,4
rad,6=deg,57.2958,0,5

°C,1=°C,1

[Imperial]
m/s,0="/s,39.3700787401,0,0
m/s,1="/s,39.3700787401,0,1
m/s,2="/s,39.3700787401,0,2
m/s,3="/s,39.3700787401,0,3
m/s,4="/s,39.3700787401,0,4
m/s,5="/s,39.3700787401,0,5

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

TEST_CASE("sf::UnitConversionEx", "[unit][conversion][variable]")
{
	using Catch::Approx;
	using Catch::Matchers::Equals;
	using Catch::Matchers::Matches;

	sf::Variable::initialize();

	std::istringstream is(IniContent);
	sf::UnitConversionServerEx& ucs(*new sf::UnitConversionServerEx());
	ucs.load(is);

	double multiplier{0};
	double offset{0};
	std::string unit;
	int precision{0};

	SECTION("uce:Metric")
	{
		ucs.setUnitSystem(sf::UnitConversionServer::usMetric);

		REQUIRE(sf::getUnitConversion("", "m/s", 5, multiplier, offset, unit, precision));
		REQUIRE(multiplier == Approx(1000.0).margin(0.001));
		REQUIRE(offset == Approx(0.0).margin(0.001));
		REQUIRE(unit == "mm/s");
		REQUIRE(precision == 2);

		REQUIRE(sf::getUnitConversion("", "rad", 5, multiplier, offset, unit, precision));
		REQUIRE(multiplier == Approx(57.2958).margin(0.001));
		REQUIRE(offset == Approx(0.0).margin(0.001));
		REQUIRE(unit == "deg");
		REQUIRE(precision == 4);

		REQUIRE_FALSE(sf::getUnitConversion("", "\U000000B0C", 1, multiplier, offset, unit, precision));
	}

	SECTION("uce:Imperial")
	{
		ucs.setUnitSystem(sf::UnitConversionServer::usImperial);
		sf::getUnitConversion("", "\U000000B0C", 1, multiplier, offset, unit, precision);
		REQUIRE(multiplier == Approx(1.8).margin(0.0000001));
		REQUIRE(offset == Approx(32).margin(0.0000001));
		REQUIRE(unit == "\U000000B0F");
		REQUIRE(precision == 1);
	}

	SECTION("uce:Variable")
	{
		// Enable unit conversion.
		ucs.setEnable(true);
		// Set the unit system to use.
		ucs.setUnitSystem(sf::UnitConversionServer::usMetric);
		// Create master instance.
		sf::Variable v_master("0x10,Sound Velocity,m/s,A,Sound velocity setting,FLOAT,,10,3000,100,10000", 0);
		// Create server instance.
		sf::Variable v_server("0x100,Time of Flight,s,A,High speed velocity setting,FLOAT,,10e-7,33e-06,0,100e-6", 0);
		// Makes the server instance retrieve new conversion values.
		//CHECK(v_server.setConvertValues(true));
		// Create client variable.
		sf::Variable v_client(0x100, true);
		// Use converted values on the client.
		v_client.setConvert(true);
		// Unit should be a distance.
		CHECK(v_client.getUnit() == "mm");
		// Value should be a multiplication with 2 decimals.
		CHECK(v_client.getCurString() == "99");

		CHECK(v_master.getSigDigits() == -1);
		CHECK(v_client.getSigDigits(false) == 6);
		auto val = v_master.getCur().getFloat();
		CHECK(sf::magnitude(val) == 4);

		v_server.setCur(sf::Value(100e-6));
		// Unit should still be a distance.
		CHECK(v_client.getUnit() == "mm");
		// Value should be a multiplication with 3 decimals.
		CHECK(v_client.getCurString() == "300");
		//std::clog << "Client value: " << v_client.getCurString() << " " << v_client.getUnit() << std::endl;
	}
	// Delete the conversion server first before deinitializing the variable static stuff.
	delete &ucs;
	sf::Variable::uninitialize();
}
