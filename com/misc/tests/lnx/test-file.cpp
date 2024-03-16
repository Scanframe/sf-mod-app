#include <cstdio>
#include <filesystem>
#include <iostream>
#include <misc/gen/gen_utils.h>
#include <misc/lnx/File.h>
#include <test/catch.h>

TEST_CASE("sf::lnx::File", "[con][linux][file]")
{
	using Catch::Matchers::Equals;
	using Catch::Matchers::Matches;

	SECTION("Create Temp", "Create, write, read, close, reopen and delete a temporary file.")
	{
		sf::lnx::File file;
		// Open a unique named file using a template.
		file.createTemporary(std::filesystem::temp_directory_path().append("temp-file-test-{}.dat"));
		// Print the file name.
		//std::clog << "File path: " << file.getPath() << std::endl;
		// Check if the resulting filename is correct.
		CHECK_THAT(file.getPath(), Matches("^\\/tmp\\/temp-file-test-[a-zA-z0-9]{6}.dat$", Catch::CaseSensitive::Yes));

		size_t bytes = 0;
		// Write to some lines.
		for (int i = 0; i < 10; ++i)
		{
			// From a line.
			auto ln = sf::string_format("Some text forming as line (%02i).\n", i);
			// Keep track of the bytes written.
			bytes += ln.length();
			// Write the line.
			file.write(ln);
		}
		// Update compare the sizes.
		CHECK(file.getStatus(true).st_size == bytes);
		// Sync to disk. If no exception is thrown it is okay.
		file.synchronise();
		// Close the file.
		file.close();
		//
		CHECK(file.isOpen() == false);
		// Open the closed file.
		file.open();
		//
		CHECK(file.isOpen() == true);
		// Do not update the stat structure and compare the size.
		CHECK(file.getStatus().st_size == bytes);
		// Close the and remove it.
		file.remove();
		// File does not exist.
		CHECK(file.exists() == false);
	}

	SECTION("Open", "Create file binary write, read, truncate, delete a file")
	{
		sf::lnx::File file(std::filesystem::temp_directory_path().append("temp-file-test.dat"));
		// Open the file.
		file.open();
// Binary structure to write and read.
#pragma pack(push, 1)
		struct MyType
		{
				int _int_value;
				short _short_value;
				long _long_value;
		};
// Restore the pack option.
#pragma pack(pop)
		// Initialize the binary structure.
		MyType mts{0x1234567, 0x1234, 0x123456789};
		// Write the binary structure.
		file.write(mts);
		// Initialize the binary structure before reading.
		MyType mts2{0, 0, 0};
		// Read the structure.
		file.read(mts2, 0);
		// Truncate the file.
		file.truncate(0);
		//
		CHECK(file.getStatus(true).st_size == 0);
		// Compare the structures.
		CHECK(memcmp(&mts, &mts2, sizeof(MyType)) == 0);
		// Remove the used file.
		file.remove();
	}
}
