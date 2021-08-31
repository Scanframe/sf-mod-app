#include <catch2/catch.hpp>

#include <string>
#include <iostream>
#include <utility>
#include <gii/gen/ResultData.h>

extern int debug_level;

namespace
{

/**
 * Structure for registering and comparing the event flow.
 */
struct ResEvent :sf::ResultDataTypes
{
	ResEvent(sf::ResultData::EEvent event, const sf::ResultData* call_res, const sf::ResultData* link_res, bool same_inst,
		id_type id, std::string any = "")
		:_event(event), _call_res(call_res), _link_res(link_res), _same_inst(same_inst), _id(id), _any(std::move(any)) {}

	EEvent _event;
	const sf::ResultData* _call_res;
	const sf::ResultData* _link_res;
	sf::ResultDataTypes::id_type _id;
	bool _same_inst;
	std::string _any;

	bool operator==(const ResEvent& ev) const;

	typedef sf::TVector<ResEvent> Vector;

};

bool ResEvent::operator==(const ResEvent& ev) const
{
	return _event == ev._event &&
		_call_res == ev._call_res &&
		_link_res == ev._link_res &&
		_same_inst == ev._same_inst &&
		_id == ev._id &&
		_any == ev._any;
}

std::ostream& operator<<(std::ostream& os, const ResEvent& ev)
{
	return os
		<< std::endl
		<< "(E:re" << sf::ResultData::getEventName(ev._event) << ", "
		<< "C:" << ev._call_res << ", "
		<< "L:" << ev._link_res << ") "
		<< "S:" << std::boolalpha << ev._same_inst << ", I:0x" << std::hex << ev._id << ", \"" << ev._any << "\")";
}

struct ResHandler :sf::ResultDataHandler
{
	ResEvent::Vector _events;

	sf::TClosure<void, sf::ResultData::EEvent, const sf::ResultData&, sf::ResultData&, const sf::Range&, bool> _callback;

	void resultDataEventHandler
		(
			sf::ResultData::EEvent event,
			const sf::ResultData& call_res,
			sf::ResultData& link_res,
			const sf::Range& range,
			bool same_inst) override
	{
		// Add the even to the list.
		ResEvent ve{event, &call_res, &link_res, same_inst, call_res.getId()};
		switch (event)
		{
			case reFlagsChange:
				ve._any = link_res.getCurFlagsString();
				break;

			case reDesiredId:
				ve._any = sf::stringf("0x%llX", link_res.getDesiredId());
				break;

			case reReserve:
			case reAccessChange:
			case reCommitted:
			case reGetRange:
			case reGotRange:
			{
				std::ostringstream os;
				os << range;
				ve._any = os.str();
				break;
			}

			default:
				break;
		}
		_events.add(ve);
		if (_callback)
		{
			_callback(event, call_res, link_res, range, same_inst);
		}
	}

};

std::ostream& operator<<(std::ostream& os, const ResHandler& rh)
{
	return os << rh._events;
}

} // namespace

TEST_CASE("sf::ResultData", "[result]")
{
	sf::ResultData::initialize();

	SECTION("Definition")
	{
		{
			auto def = sf::ResultData::getDefinition(
				"0x9001,Test INT8,A,Compiler Test result type INT8.,INT8,1024,256,8,127");
			REQUIRE(def._valid);
			CHECK(def._id == 0x9001);
			CHECK(def._name == "Test INT8");
			CHECK(def._flags == sf::ResultData::flgArchive);
			CHECK(def._description == "Compiler Test result type INT8.");
			CHECK(def._type == sf::ResultDataTypes::rtInt8);
			CHECK(def._blockSize == 1024);
			CHECK(def._segmentSize == 256);
			CHECK(def._significantBits == 8);
			CHECK(def._offset == 127);
			//
			sf::ResultData rd(def);
			// Check setup string.
			REQUIRE(rd.getSetupString() == "0x9001,Test INT8,A,Compiler Test result type INT8.,INT8,1024,256,8,127");
		}
		{
			auto def = sf::ResultData::getDefinition(
				"0x9001,Test INT8,A,Compiler Test result type INT8.,INT8,1024,256");
			REQUIRE(def._valid);
			CHECK(def._id == 0x9001);
			CHECK(def._name == "Test INT8");
			CHECK(def._flags == sf::ResultData::flgArchive);
			CHECK(def._description == "Compiler Test result type INT8.");
			CHECK(def._type == sf::ResultDataTypes::rtInt8);
			CHECK(def._blockSize == 1024);
			CHECK(def._segmentSize == 256);
			CHECK(def._significantBits == 0);
			CHECK(def._offset == 0);
			//
			sf::ResultData rd(def);
			// Check setup string.
			REQUIRE(rd.getSetupString() == "0x9001,Test INT8,A,Compiler Test result type INT8.,INT8,1024,256,8,0");
		}
	}

	SECTION("Events:Creation")
	{
		// No instances here at the start.
		REQUIRE_FALSE(sf::ResultData::getInstanceCount());

		ResHandler handler_client;
		ResHandler handler_server1;
		ResHandler handler_server2;

		sf::ResultData r_client;
		r_client.setHandler(&handler_client);
		r_client.setDesiredId(0x9003);

		// Create owner instance.
		sf::ResultData r_server1(std::string("0x1,Test INT8,A,Compiler Test result type INT8.,INT8,1024,256"), 0x9000);
		r_server1.setHandler(&handler_server1);

		// Create other owner instance.
		sf::ResultData r_server2(std::string("0x3,Test INT32,SA,Compiler Test result type INT32.,INT32,25,1024"), 0x9000);
		r_server2.setHandler(&handler_server2);

		// Expected event flow.
		REQUIRE(handler_server2._events == ResEvent::Vector{
			{
				{sf::ResultData::reLinked, &r_server2, &r_server2, true, 0x9003},
			}
		});

		// Expected event flow.
		REQUIRE(handler_client._events == ResEvent::Vector{
			{
				{sf::ResultData::reLinked, &r_client, &r_client, true, 0x0},
				{sf::ResultData::reDesiredId, &r_client, &r_client, true, 0x0, "0x9003"},
				{sf::ResultData::reNewId, &r_server1, &r_client, false, 0x9001},
				{sf::ResultData::reNewId, &r_server2, &r_client, false, 0x9003},
				{sf::ResultData::reIdChanged, &r_client, &r_client, true, 0x9003},
			}
		});
		// Clear the events
		handler_client._events.clear();
		handler_server1._events.clear();
		handler_server2._events.clear();
		// Instance count so far.
		REQUIRE(sf::ResultData::getInstanceCount() == 3);
		//
		REQUIRE_FALSE(r_client.setReservedBlockCount(1000));
		//
		r_server2.setAccessRange({0, 10}, false);

		// Expected event flow.
		REQUIRE(handler_server2._events == ResEvent::Vector{
			{
				{sf::ResultData::reReserve, &r_server2, &r_server2, true, 0x9003, "(0,1024,36867)"},
				{sf::ResultData::reAccessChange, &r_server2, &r_server2, true, 0x9003, "(0,10,0)"},
			}
		});
		// Expected event flow.
		REQUIRE(handler_client._events == ResEvent::Vector{
			{
				{sf::ResultData::reReserve, &r_server2, &r_client, false, 0x9003, "(0,1024,36867)"},
				{sf::ResultData::reAccessChange, &r_server2, &r_client, false, 0x9003, "(0,10,0)"},
			}
		});
	}

	SECTION("Data:Server-Initiated")
	{
		ResHandler handler_server;
		ResHandler handler_client;
		// Create server/owner instance.
		sf::ResultData r_server(std::string("0x1,Data,S,Just some data.,INT32,1,20,24,1024"));
		r_server.setDebug(debug_level);
		r_server.setHandler(&handler_server);
		// Create client instance.
		sf::ResultData r_client;
		r_client.setHandler(&handler_client);
		r_client.setup(0x1, true);
		// Check if linked to the server.
		REQUIRE(r_client.getId() == 0x1);
		// Expected event flow.
		REQUIRE(handler_server._events == ResEvent::Vector{
			{
				{sf::ResultData::reLinked, &r_server, &r_server, true, 0x1},
			}
		});
		// Expected event flow.
		REQUIRE(handler_client._events == ResEvent::Vector{
			{
				{sf::ResultData::reLinked, &r_client, &r_client, true, 0x0},
				{sf::ResultData::reDesiredId, &r_client, &r_client, true, 0x0, "0x1"},
				{sf::ResultData::reIdChanged, &r_client, &r_client, true, 0x1},
			}
		});
		// Clear events for next checks.
		handler_client._events.clear();
		handler_server._events.clear();
		// Size buffer is 10 blocks.
		sf::ResultData::size_type blocks = 10;
		// Check if the needed bytes are calculated right.
		REQUIRE(r_server.getBufferSize(blocks) == r_server.getTypeSize() * r_server.getBlockSize() * blocks);
		// Create buffer buffer with some additional space to check overflow.
		sf::TVector<uint8_t> write_buf(r_server.getBufferSize(blocks));
		// Fill buffer with data.
		for (size_t i = 0; i < write_buf.size(); i++) {write_buf[i] = (uint8_t) (i + 'A');}
		// Must fail because of no space to write.
		REQUIRE_FALSE(r_server.blockWrite(0, blocks, write_buf.data(), false));
		// Must succeed because of auto reserve being enabled.
		REQUIRE(r_server.blockWrite(0, blocks, write_buf.data(), true));
		// Must fail noting has been committed what has been written.
		REQUIRE(r_server.blockWrite(blocks + 2, 2, write_buf.data(), true));
		// Check the range legible to be committed.
		REQUIRE(r_server.getCommitList() == sf::Range::Vector{{{0, 10, 1}, {12, 14, 1}}});
		// Check if a range is valid before it is committed.
		REQUIRE_FALSE(r_client.isRangeValid(sf::Range(1, 4)));
		// Signal all that the written blocks are accessible.
		r_server.commitValidations(false);
		// Now the same range has become valid.
		REQUIRE(r_client.isRangeValid(sf::Range(1, 4)));
		// Expected server event flow.
		REQUIRE(handler_server._events == ResEvent::Vector{
			{
				{sf::ResultData::reReserve, &r_server, &r_server, true, 0x1, "(0,20,0)"},
				{sf::ResultData::reAccessChange, &r_server, &r_server, true, 0x1, "(0,14,1)"},
				{sf::ResultData::reCommitted, &r_server, &r_server, true, 0x1, "(0,14,1)"},
			}
		});
		// Expected client event flow.
		REQUIRE(handler_client._events == ResEvent::Vector{
			{
				{sf::ResultData::reReserve, &r_server, &r_client, false, 0x1, "(0,20,0)"},
				{sf::ResultData::reAccessChange, &r_server, &r_client, false, 0x1, "(0,14,1)"},
				{sf::ResultData::reCommitted, &r_server, &r_client, false, 0x1, "(0,14,1)"},
			}
		});
		// Create read buffer.
		sf::TVector<uint8_t> read_buf(r_server.getBufferSize(blocks));
		// Must succeed because of auto reserve being enabled.
		REQUIRE(r_client.blockRead(0, blocks, read_buf.data(), true));
		// Check if the data is the same.
		REQUIRE(read_buf == write_buf);
	}

/*
		std::clog << "Server: " << handler_server << std::endl;
		std::clog << "Client: " << handler_client << std::endl;
*/

	SECTION("Data:Client-Initiated")
	{
		ResHandler handler_server;
		ResHandler handler_client;

		// Create client instance.
		sf::ResultData r_client;
		r_client.setHandler(&handler_client);
		// Waiting for Id '1' to appear.
		r_client.setup(0x1, true);

		// Create server/owner instance.
		sf::ResultData r_server(std::string("0x1,Data,S,Just some data.,INT32,1,20,24,1024"));
		r_server.setHandler(&handler_server);
		r_server.setDebug(debug_level);
		// Check if linked to the server.
		REQUIRE(r_client.getId() == 0x1);
		// Expected event flow.
		REQUIRE(handler_server._events == ResEvent::Vector{
			{
				{sf::ResultData::reLinked, &r_server, &r_server, true, 0x1},
			}
		});
		// Expected event flow.
		REQUIRE(handler_client._events == ResEvent::Vector{
			{
				{sf::ResultData::reLinked, &r_client, &r_client, true, 0x0},
				{sf::ResultData::reDesiredId, &r_client, &r_client, true, 0x0, "0x1"},
				{sf::ResultData::reNewId, &r_server, &r_client, false, 0x1},
				{sf::ResultData::reIdChanged, &r_client, &r_client, true, 0x1},
			}
		});
		// Clear events for next checks.
		handler_client._events.clear();
		handler_server._events.clear();

		// Create an initialized buffer of 10 blocks.
		sf::ResultData::size_type blocks = 10;
		// Check if the needed bytes are calculated right.
		REQUIRE(r_server.getBufferSize(blocks) == r_server.getTypeSize() * r_server.getBlockSize() * blocks);
		// Create buffer buffer with some additional space to check overflow.
		sf::TVector<uint8_t> write_buf(r_server.getBufferSize(blocks));
		// Fill buffer with data.
		for (size_t i = 0; i < write_buf.size(); i++) {write_buf[i] = (uint8_t) (i + 'A');}

		// Set callback function to handle some client events.
		handler_client._callback = [blocks, write_buf](sf::ResultData::EEvent event, const sf::ResultData& call_res,
			sf::ResultData& link_res, const sf::Range& range, bool same_inst) -> void
		{
			//
			// Use only CHECKS in the handlers because nested REQUIRE are not working well.
			//
			if (event == sf::ResultData::reAccessChange)
			{
				// Request some range.
				CHECK(link_res.requestRange(24, blocks));
			}
			else if (event == sf::ResultData::reGotRange)
			{
				// Create read buffer.
				sf::TVector<uint8_t> read_buf(link_res.getBufferSize(range.getSize()));
				// Must succeed.
				CHECK(link_res.blockRead(range, read_buf.data()));
				// Check if the data is the same.
				CHECK(read_buf == write_buf);
			}
		};

		// Set callback function to handle some server events.
		handler_server._callback = [write_buf](sf::ResultData::EEvent event, const sf::ResultData& call_res,
			sf::ResultData& link_res, const sf::Range& range, bool same_inst) -> void
		{
			//
			// Use only CHECKS in the handlers because nested REQUIRE are not working well.
			//
			if (event == sf::ResultData::reGetRange)
			{
				// Must succeed.
				CHECK(link_res.blockWrite(range, write_buf.data()));
				// More the one event must have been emitted.
				CHECK(link_res.commitValidations() > 0);
			}
		};

		// Server advertises the accessible range is. For example after reading a stored file.
		REQUIRE(r_server.setAccessRange({0, 100}, false));
		// Smaller amount has no effect now.
		REQUIRE_FALSE(r_server.setAccessRange({0, 90}, false));
		// Amount of segments must equal 5 now.
		REQUIRE(r_server.getSegmentCount() == 5);

		// Expected server event flow.
		REQUIRE(handler_server._events == ResEvent::Vector{
			{
				{sf::ResultData::reReserve, &r_server, &r_server, true, 0x1, "(0,100,1)"},
				{sf::ResultData::reAccessChange, &r_server, &r_server, true, 0x1, "(0,100,0)"},
				{sf::ResultData::reGetRange, &r_client, &r_server, false, 0x1, "(24,34,1)"},
				{sf::ResultData::reCommitted, &r_server, &r_server, true, 0x1, "(0,100,0)"},
			}
		});
		// Expected client event flow.
		REQUIRE(handler_client._events == ResEvent::Vector{
			{
				{sf::ResultData::reReserve, &r_server, &r_client, false, 0x1, "(0,100,1)"},
				{sf::ResultData::reAccessChange, &r_server, &r_client, false, 0x1, "(0,100,0)"},
				{sf::ResultData::reGotRange, &r_server, &r_client, false, 0x1, "(24,34,2)"},
				{sf::ResultData::reCommitted, &r_server, &r_client, false, 0x1, "(0,100,0)"},
			}
		});
	}

	sf::ResultData::deinitialize();

}

