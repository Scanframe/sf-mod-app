// This tells Catch to provide a main() - only do this in one cpp file
//#define CATCH_CONFIG_MAIN // Cannot be used because of Windows missing WinMain complaint.
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

int main(int argc, char* argv[])
{
	return  Catch::Session().run(argc, argv);
}