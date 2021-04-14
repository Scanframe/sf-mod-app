#include <catch2/catch.hpp>

#include <string>
#include <iostream>
#include <utility>
#include <misc/gen/TVector.h>
#include <gii/gen/FileMappedStorage.h>

TEST_CASE("sf::FileMappedStorage", "[result]")
{
	SECTION("Storage:Creation")
	{
		// Bytes
		typedef int32_t block_type;
		size_t bytes_per_block = sizeof(block_type);
		size_t blocks_per_seg = 100;

		// Create a storage element.
		auto ds = new sf::FileMappedStorage(blocks_per_seg, bytes_per_block);
		// Reserve 10K * int32_t = 40KByte which results
		REQUIRE(ds->reserve(1000));
		// Check if this the case.
		REQUIRE(ds->getSize() == 4000);
		REQUIRE(ds->getBlockSize() == 4);
		REQUIRE(ds->getSegmentSize() == 100);
		REQUIRE(ds->getSegmentCount() == 10);
		// Check the status output.
		std::ostringstream os;
		ds->writeStatus(os);
		CHECK(os.str() == R"(Total Allocated Bytes: 4000
Segment Size: 100
Block Size: 4
Segment Count: 10
Block Count: 1000
)");
		// Create buffer containing 110 blocks.
		sf::TVector<block_type> buffer_write(12);
		// Initialize the buffer with some recognisable data.
		for (auto& i: buffer_write)
		{
			i = 0xdeadc0de;
		}
		//
		sf::TVector<block_type> buffer_read(buffer_write.size());
		// Write the buffer.
		REQUIRE(ds->blockWrite(0, buffer_write.size(), buffer_write.data()));
		// Read the same data into the read buffer.
		REQUIRE(ds->blockRead(0, buffer_read.size(), buffer_read.data()));
		// Compare the to data sets.
		REQUIRE(buffer_write == buffer_read);

		// Write a buffer across the segment boundary.
		REQUIRE(ds->blockWrite(blocks_per_seg - buffer_write.size() / 2, buffer_write.size(), buffer_write.data()));
		// Initialize the read buffer.
		for (auto& i: buffer_read) {i = 0xAAAAAAAA;}
		// Read the same data into the read buffer.
		REQUIRE(ds->blockRead(blocks_per_seg - buffer_write.size() / 2, buffer_read.size(), buffer_read.data()));
		// Compare the to data sets.
		REQUIRE(buffer_write == buffer_read);
		// Scope to lock pointer and access to the file map memory.
		{
			sf::FileMappedStorage::Lock lock(*ds);
			//
			REQUIRE(lock.acquire(0));
			// Lock the segments and check the written content entries.
			auto p = lock.ptr<int32_t>(0);
			for (sf::FileMappedStorage::size_type i = 0; i < ds->getSegmentSize(); i++)
			{
				//std::clog << "[" << std::dec << i << "] " << std::hex << p[i] << std::endl;
				CHECK(p[i] == (i < buffer_write.size() || i >= (blocks_per_seg - buffer_write.size() / 2) ? 0xdeadc0de : 0x0));
			}
		}
		delete ds;

/*
		std::clog << "Server: " << handler_server << std::endl;
		std::clog << "Client: " << handler_client << std::endl;
*/
	}
}

