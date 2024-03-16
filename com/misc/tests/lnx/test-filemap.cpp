#include "misc/gen/Md5Hash.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <misc/gen/Exception.h>
#include <misc/lnx/FileMapper.h>
#include <misc/lnx/SegmentFaultHandler.h>
#include <misc/lnx/lnx_utils.h>
#include <test/catch.h>

TEST_CASE("sf::lnx::FileMapper", "[con][linux][file]")
{
	SECTION("Create")
	{
		// Size of the file.
		size_t sz = 256;
		// Characters to fill the file with.
		auto* str = "abcdefghijklmnopqrstuvwxyz0123456789\n";
		// Scope
		auto* fm = new sf::lnx::FileMapper;
		// Initialize using temporary file which is removed at destruction.
		fm->initialize();
		// Create view for a size.
		fm->createView(sz);
		// Get the file name of the used file.
		auto fp = fm->getFile().getPath();
		// Report the file created.
		//std::clog << "File:" << fp << std::endl;
		// Get the status of the file and compare the size.
		REQUIRE(fm->getFile().getStatus().st_size == sz);

		// Get the readonly pointer.
		auto ptr = fm->lock<char>(true);
		// Segment Fault must occur in this function.
		auto f1 = [ptr]() -> void {
			ptr[0] = 'A';
		};
		// Call the lambda function wrapped in a SegmentFaultHandler class.
		REQUIRE(!sf::SegmentFaultHandler(f1));

		// Unlock readonly pointer.
		fm->unlock();
		// Check if pointer is null.
		REQUIRE(fm->getPtr() == nullptr);

		// Get writable pointer.
		ptr = fm->lock<char>(false);
		//auto ptr = (char*) fm->getDataPtr();
		auto f2 = [ptr, sz, str]() -> void {
			for (size_t i = 0; i < sz; ++i)
			{
				// Write entire space with data.
				ptr[i] = str[i % 37];
			}
		};
		// Call the lambda function wrapped in a SegmentFaultHandler class.
		REQUIRE(sf::SegmentFaultHandler(f2));
		// Read the still existing file into a string.
		std::stringstream ss;
		ss << std::ifstream(fp).rdbuf();
		//std::clog << ss.str();
		// Sizes must be the same.
		REQUIRE(ss.str().length() == sz);
		// Also the MD5 hash on both must be the same.

		REQUIRE(sf::Md5Hash(std::string(ss.str())) == sf::Md5Hash(ptr, sz));

		// Release the locked pointer.
		fm->unlock();
		// Check if pointer is null.
		REQUIRE(fm->getPtr() == nullptr);

		// Delete the file map which implicates unlinking the temporary file.
		delete fm;
		// Check if the temp file has been deleted.
		REQUIRE(!sf::fileExists(fp));
	}
}
