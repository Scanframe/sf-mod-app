#include "../BufferChopper.h"
#include <ami/service/BufferStitcher.h>
#include <misc/gen/ElapseTimer.h>
#include <misc/gen/dbgutils.h>
#include <test/catch.h>

TEST_CASE("AmiModService", "[buffer]")
{
	using Catch::Matchers::Equals;

	SECTION("BufferChopper")
	{
		sf::ElapseTimer timer(sf::TimeSpec(0.1));
		// When debugging skip checking on endless loops.
		if (sf::isDebuggerActive())
		{
			timer.disable();
		}
		constexpr size_t bufSize = 1000;
		constexpr size_t chopSize = 60;
		auto* buf = new char[bufSize];
		for (size_t i = 0; i < bufSize; i++)
		{
			buf[i] = (char) (i % chopSize);
		}
		sf::BufferChopper bc(chopSize);
		bc.assign(buf, bufSize);
		if (!bc.isDone())
		{
			do
			{
				// Check if the work is done in time. We don't like endless loops.
				REQUIRE_FALSE(timer.isActive());
				//
				auto* ch = (uint8_t*) bc.getChunkData();
				for (size_t i = 0; i < bc.getChunkSize(); i++)
				{
					CHECK(ch[i] == i);
				}
			} while (bc.moveNext());
		}
		CHECK(bc.getChunkData() == nullptr);
		CHECK(bc.getChunkSize() == 0);

		delete[] buf;
	}

	SECTION("BufferStitcher")
	{
		sf::ElapseTimer timer(sf::TimeSpec(0.1));
		// When debugging skip checking on endless loops.
		if (sf::isDebuggerActive())
		{
			timer.disable();
		}
		std::string_view sv("ABCDEF");
		std::string buffer;
		buffer.reserve(1024);
		buffer.assign(53, 'X');
		size_t len = 50;
		sf::BufferStitcher bs;
		bs.assign(buffer.data(), len);
		size_t sz = 0;
		size_t pos = 0;
		CHECK(buffer.data() == bs.getWritePointer());
		CHECK_FALSE(bs.isDone());
		if (!bs.isDone())
		{
			do
			{
				// Check if the work is done in time. We don't like endless loops.
				REQUIRE_FALSE(timer.isActive());
				//
				CHECK(std::distance(buffer.data(), bs.getWritePointer()) == pos);
				sz = std::min(bs.getWriteSize(), sv.length());
				memcpy(bs.getWritePointer(), sv.data(), sz);
				pos += sz;
			} while (!bs.movePosition(sz));
		}
		// Check the remainder is correct.
		CHECK(sz == len % sv.length());
		// Check if the last part of the buffer is not overwritten.
		CHECK(buffer.substr(len) == std::string(buffer.size() - len, 'X'));
		//
		CHECK(buffer == "ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABXXX");
	}
}