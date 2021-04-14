// This tells Catch to provide a main() - only do this in one cpp file
//#define CATCH_CONFIG_MAIN // Cannot be used because of Windows missing WinMain complaint.
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

// Some user variable you want to be able to set
int debug_level = 0;

int main(int argc, char* argv[])
{
#if 0
	return  Catch::Session().run(argc, argv);
#else

	Catch::Session session; // There must be exactly one instance
	int debug = 0;
	// Build a new parser on top of Catch's
	using namespace Catch::clara;
	auto cli
		// Get Catch's composite command line parser
		= session.cli()
			// bind variable to a new option, with a hint string
			| Opt(debug_level, "level")
			// the option names it will respond to
		["--debug"]
			// description string for the help output
			("Custom option for a debug level.");
	// Now pass the new composite back to Catch so it uses that
	session.cli(cli);
	// Let Catch (using Clara) parse the command line
	int returnCode = session.applyCommandLine(argc, argv);
	if (returnCode != 0)
	{ // Indicates a command line error
		return returnCode;
	}
	// if set on the command line then 'height' is now set at this point
	if (debug > 0)
	{
		std::cout << "Debug: " << debug << std::endl;
	}
	return session.run();
#endif
}
