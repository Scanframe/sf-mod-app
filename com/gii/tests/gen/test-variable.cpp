#include <catch2/catch.hpp>

#include <string>
#include <iostream>
#include <utility>
#include <gii/gen/Variable.h>
#include <gii/gen/UnitConversionServer.h>

namespace
{

// Formula (T × 9/5) + 32

const char* IniContent = R"(

[Metric]
m/s,1=mm/s,1000,0,-2
m/s,2=mm/s,1000,0,-1
m/s,3=mm/s,1000,0,0
°C,1=°C,1,0,1

[Imperial]
m/s,0="/s,39.3700787401,0,0
m/s,1="/s,39.3700787401,0,1
m/s,2="/s,39.3700787401,0,2
°C,1=°F,1.8,32,1

)";

/**
 * Helper class getting streamed output in to a string for comparison of type and value.
 */
struct
{
	std::string operator()(const sf::Value& v)
	{
		std::ostringstream os;
		return dynamic_cast<std::ostringstream&>(os << v).str();
	}
} Helper;

/**
 * Structure for registering and comparing the event flow.
 */
struct VarEvent :sf::VariableTypes
{
	VarEvent(sf::Variable::EEvent event, const sf::Variable* call_var, const sf::Variable* link_var, bool same_inst,
		id_type id, std::string any = {})
		:_event(event), _call_var(call_var), _link_var(link_var), _same_inst(same_inst), _id(id), _any(std::move(any)) {}

	EEvent _event;
	const sf::Variable* _call_var;
	const sf::Variable* _link_var;
	sf::VariableTypes::id_type _id;
	bool _same_inst;
	std::string _any;

	bool operator==(const VarEvent& ev) const;

	typedef sf::TVector<VarEvent> Vector;
};

bool VarEvent::operator==(const VarEvent& ev) const
{
	return _event == ev._event &&
		_call_var == ev._call_var &&
		_link_var == ev._link_var &&
		_same_inst == ev._same_inst &&
		_id == ev._id &&
		_any == ev._any;
}

std::ostream& operator<<(std::ostream& os, const VarEvent& ev)
{
	return os
		<< std::endl
		<< "(E:ve" << sf::Variable::getEventName(ev._event) << ", "
		<< "C:" << ev._call_var << ", "
		<< "L:" << ev._link_var << ") "
		<< "S:" << std::boolalpha << ev._same_inst << ", I:0x" << std::hex << ev._id << ", \"" << ev._any << "\")";
}

struct VarHandler :sf::VariableHandler
{
	VarEvent::Vector _events;

	void variableEventHandler(sf::Variable::EEvent event, const sf::Variable& call_var, sf::Variable& link_var,
		bool same_inst) override
	{
		// Add the even to the list.
		VarEvent ve{event, &call_var, &link_var, same_inst, call_var.getId()};
		switch (event)
		{
			case veValueChange:
			{
				bool owned = link_var.isOwner();
				// Server report non-converted value.
				ve._any = link_var.getCur(!owned).getString(link_var.getSigDigits(!owned));
				break;
			}

			case veFlagsChange:
				ve._any = link_var.getCurFlagsString();
				break;

			case veDesiredId:
				ve._any = sf::stringf("0x%llX", link_var.getDesiredId());
				break;

			case veConverted:
				ve._any = link_var.getUnit(false) + '|' + sf::itostr(link_var.getSigDigits(false), 10)
					+ ' ' + link_var.getUnit(true) + '|' + sf::itostr(link_var.getSigDigits(true), 10);
				break;

			default:
				break;
		}
		_events.add(ve);
	}
};

std::ostream& operator<<(std::ostream& os, const VarHandler& vh)
{
	return os << vh._events;
}

} // namespace

TEST_CASE("sf::Variable", "[variable]")
{
	sf::Variable::initialize();

	SECTION("Events:Creation")
	{
		// No instances here at the start.
		REQUIRE_FALSE(sf::Variable::getInstanceCount());

		VarHandler handler_client;
		VarHandler handler_owner1;
		//VarHandler handler_owner2;

		sf::Variable v_client;
		v_client.setHandler(&handler_client);
		v_client.setDesiredId(0x2016);
		// Create owner instance.
		sf::Variable v_owner1(std::string("0x2016,High Speed,m/s,A,High speed velocity setting,FLOAT,FLOAT,1,10,0,20"));
		v_owner1.setHandler(&handler_owner1);
		// Create other owner instance.
		sf::Variable v_owner2(std::string(
			"0x3005,Low Pass Filter,,Low pass filters OFF; 2.5 MHz; 5.0 MHz; 10.0 MHz,A,INTEGER,LONG,1,0,0,3,OFF=0,2.5 MHz=1,5.0 MHz=2,10.0 MHz=3"));
		//v_owner2.setHandler(&handler_owner2);
		// Expected event flow.
		REQUIRE(handler_client._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_client, &v_client, true, 0x0},
				{sf::Variable::veDesiredId, &v_client, &v_client, true, 0x0, "0x2016"},
				{sf::Variable::veNewId, &v_owner1, &v_client, false, 0x2016},
				{sf::Variable::veIdChanged, &v_client, &v_client, true, 0x2016},
				{sf::Variable::veNewId, &v_owner2, &v_client, false, 0x3005},
			}
		});
		// Clear the events
		handler_client._events.clear();
		handler_owner1._events.clear();
		// Instance count so far.
		REQUIRE(sf::Variable::getInstanceCount() == 3);
		// Checking the created states.
		sf::Variable::State::Vector states{
			{
				{"OFF", sf::Value(0)},
				{"2.5 MHz", sf::Value(1)},
				{"5.0 MHz", sf::Value(2)},
				{"10.0 MHz", sf::Value(3)}
			}
		};
		REQUIRE(v_owner2.getStates() == states);
		// Check individual state function.
		REQUIRE(v_owner2.getStateName(2) == "5.0 MHz");
		// Check individual state function.
		REQUIRE(v_owner2.getStateValue(2) == sf::Value(2));
		// Checking the setup string of the variable.
		REQUIRE(v_owner2.getSetupString() ==
			"0x3005,Low Pass Filter,,LFH,A,INTEGER,,1,0,0,3,OFF=0,2.5 MHz=1,5.0 MHz=2,10.0 MHz=3");
		//
		sf::Variable* pv_hijack;
		// New scope to make v_hijack3 go out of scope.
		{
			handler_owner1._events.clear();
			VarHandler handler_hijack;
			sf::Variable v_hijack;
			pv_hijack = &v_hijack;
			v_hijack.setHandler(&handler_hijack);
			v_hijack.setup(v_owner1);
			// Event flow.
			REQUIRE(handler_hijack._events == VarEvent::Vector{
				{
					{sf::Variable::veLinked, &v_hijack, &v_hijack, true, 0x0},
					{sf::Variable::veIdChanged, &v_hijack, &v_hijack, true, 0x2016},
				}
			});
			handler_hijack._events.clear();
			// Check if owner.
			REQUIRE(v_owner1.isOwner());
			// Check if NOT owner.
			REQUIRE_FALSE(v_hijack.isOwner());
			// Take owner ship.
			v_hijack.makeOwner();
			// Check if NOT owner.
			REQUIRE_FALSE(v_owner1.isOwner());
			// Check if owner.
			REQUIRE(v_hijack.isOwner());
			// Event flow owner 1.
			REQUIRE(handler_owner1._events == VarEvent::Vector{
				{
					{sf::Variable::veLostOwner, &v_hijack, &v_owner1, false, 0x2016},
				}
			});
			// Event flow hijack.
			REQUIRE(handler_hijack._events == VarEvent::Vector{
				{
					{sf::Variable::veGetOwner, &v_hijack, &v_hijack, true, 0x2016},
				}
			});
			// Clear events before going out of scope.
			handler_owner1._events.clear();
		}
		// Check events of hijack owner getting out of scope.
		REQUIRE(handler_owner1._events == VarEvent::Vector{
			{
				{sf::Variable::veInvalid, pv_hijack, &v_owner1, false, 0x2016},
				{sf::Variable::veIdChanged, &v_owner1, &v_owner1, true, 0x0},
			}
		});
	}

	SECTION("Events:Value/Flags")
	{
		// No instances here at the start.
		REQUIRE_FALSE(sf::Variable::getInstanceCount());

		VarHandler handler_server;
		VarHandler handler_client;

		// Create owner instance.
		sf::Variable v_server("0x1000,High Speed,m/s,A,High speed velocity setting,FLOAT,FLOAT,0.1,10,0,20", 0x200000);
		v_server.setHandler(&handler_server);

		sf::Variable v_client;
		v_client.setHandler(&handler_client);
		v_client.setup(0x201000, true);
		// Check server instance events.
		REQUIRE(handler_server._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_server, &v_server, true, 0x201000},
			}
		});
		// Check server instance events.
		REQUIRE(handler_client._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_client, &v_client, true, 0x0},
				{sf::Variable::veDesiredId, &v_client, &v_client, true, 0x0, "0x201000"},
				{sf::Variable::veIdChanged, &v_client, &v_client, true, 0x201000},
			}
		});
		// Clear the events for the next checks.
		handler_client._events.clear();
		handler_server._events.clear();
		// Change the server current value.
		REQUIRE(v_server.setCur(sf::Value(1), false));
		// Changing to the "same" because 1.1 is rounded to 1 value should return false.
		REQUIRE_FALSE(v_server.setCur(sf::Value(1.01), false));
		// Now the client is changing the value.
		REQUIRE(v_client.setCur(sf::Value(2), true));
		// Increment current value by steps.
		REQUIRE(v_client.increase(14, true));
		// Server sets a flag.
		REQUIRE(v_server.setFlag(sf::Variable::flgReadonly));
		// Server sets a flag.
		REQUIRE(v_server.setFlag(sf::Variable::flgParameter));
		// Server unsets the flags.
		REQUIRE(v_server.unsetFlag(sf::Variable::flgParameter | sf::Variable::flgReadonly));
		// Check client instance events.
		REQUIRE(handler_client._events == VarEvent::Vector{
			{
				{sf::Variable::veValueChange, &v_server, &v_client, false, 0x201000, "1.0"},
				//{sf::Variable::veValueChange, &v_client, &v_client, true, 0x201000, "2.0"},
				{sf::Variable::veFlagsChange, &v_server, &v_client, false, 0x201000, "RA"},
				{sf::Variable::veFlagsChange, &v_server, &v_client, false, 0x201000, "RAP"},
				{sf::Variable::veFlagsChange, &v_server, &v_client, false, 0x201000, "A"},
			}
		});
		// Check server instance events.
		REQUIRE(handler_server._events == VarEvent::Vector{
			{
				{sf::Variable::veValueChange, &v_server, &v_server, true, 0x201000, "1.0"},
				{sf::Variable::veValueChange, &v_client, &v_server, false, 0x201000, "2.0"},
				{sf::Variable::veValueChange, &v_client, &v_server, false, 0x201000, "3.4"},
				{sf::Variable::veFlagsChange, &v_server, &v_server, true, 0x201000, "RA"},
				{sf::Variable::veFlagsChange, &v_server, &v_server, true, 0x201000, "RAP"},
				{sf::Variable::veFlagsChange, &v_server, &v_server, true, 0x201000, "A"},
			}
		});
/*
		// Clear the events for the next checks.
		handler_client._events.clear();
		handler_server._events.clear();

		std::clog << "Server: " << handler_server << std::endl;
		std::clog << "Client: " << handler_client << std::endl;
*/
	}

	SECTION("Definition Structure")
	{
		std::string def_str("0x1000,High Speed,m/s,A,High speed velocity setting,FLOAT,DOUBLE,0.1,10,0,20");
		sf::Variable::Definition definition = sf::Variable::getDefinition(def_str);
		sf::Variable v_server(definition);
		REQUIRE(v_server.isValid());
		REQUIRE(v_server.getSetupString() == def_str);
	}

	SECTION("Local Instance")
	{
		VarHandler handler_server;
		VarHandler handler_client;

		// Create server instance.
		sf::Variable v_server;
		v_server.setHandler(&handler_server);
		v_server.setup(sf::Variable::getDefinition("0x1000,High Speed,m/s,A,High speed velocity setting,FLOAT,FLOAT,0.1,10,0,20"), 0x300000);

		// Create local variable.
		sf::Variable v_client;
		// Make it a local.
		REQUIRE(v_client.setGlobal(false));
		// Assign a handler.
		v_client.setHandler(&handler_client);

		// Only global ones should be counted.
		REQUIRE(sf::Variable::getInstanceCount(false) == 2);
		// Attach the reference only for the properties but not for the current values.
		REQUIRE(v_client.setup(0x301000));
		// Only global ones are counted.
		REQUIRE(sf::Variable::getInstanceCount(false) == 2);

		// Change the server value which should not have effect on client.
		REQUIRE(v_server.setCur(sf::Value(2.2)));
		// Change the client value which should not hev effect on the server.
		REQUIRE(v_client.setCur(sf::Value(3.3)));
		// Change the client flags.
		REQUIRE(v_client.setFlag(sf::Variable::flgHidden));

		// Check server instance events.
		REQUIRE(handler_server._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_server, &v_server, true, 0x0},
				{sf::Variable::veIdChanged, &v_server, &v_server, true, 0x301000},
				{sf::Variable::veNewId, &v_server, &v_server, true, 0x301000},
				{sf::Variable::veSetup, &v_server, &v_server, true, 0x301000},
				{sf::Variable::veValueChange, &v_server, &v_server, true, 0x301000, "2.2"},
			}
		});
		// Check client instance events.
		REQUIRE(handler_client._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_client, &v_client, true, 0x0},
				{sf::Variable::veIdChanged, &v_client, &v_client, true, 0x301000},
				{sf::Variable::veValueChange, &v_client, &v_client, true, 0x301000, "3.3"},
				{sf::Variable::veFlagsChange, &v_client, &v_client, true, 0x301000, "AH"},
			}
		});
	}

	SECTION("Temporary")
	{
		VarHandler handler_server;
		VarHandler handler_client;

		// Create server instance.
		sf::Variable v_server;
		v_server.setHandler(&handler_server);
		v_server.setup(
			sf::Variable::getDefinition("0x1,High Speed,m/s,A,High speed velocity setting,FLOAT,FLOAT,0.1,10,0,20"), 0x100);

		// Create local variable.
		sf::Variable v_client;
		v_client.setHandler(&handler_client);
		// Make the client use of a temporary value.
		v_client.setup(0x101);
		v_client.setTemporary(true);
		REQUIRE(v_client.setCur(sf::Value(5.5)));
		REQUIRE(v_client.isTemporaryDifferent());
		REQUIRE(v_client.applyTemporary());
		// Check server instance events.
		REQUIRE(handler_server._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_server, &v_server, true, 0x0},
				{sf::Variable::veIdChanged, &v_server, &v_server, true, 0x101},
				{sf::Variable::veNewId, &v_server, &v_server, true, 0x101},
				{sf::Variable::veSetup, &v_server, &v_server, true, 0x101},
				{sf::Variable::veValueChange, &v_client, &v_server, false, 0x101, "5.5"},
			}
		});

		// Check client instance events.
		REQUIRE(handler_client._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_client, &v_client, true, 0x0},
				{sf::Variable::veIdChanged, &v_client, &v_client, true, 0x101},
				{sf::Variable::veValueChange, &v_client, &v_client, true, 0x101, "10.0"},
				{sf::Variable::veValueChange, &v_client, &v_client, true, 0x101, "10.0"},
			}
		});
	}

	SECTION("Export")
	{
		VarHandler handler_server;
		VarHandler handler_client;

		// Create local variable.
		sf::Variable v_client;
		v_client.setHandler(&handler_client);
		v_client.setup(0x201, true);

		// Create server local instance.
		sf::Variable v_server;
		REQUIRE(v_server.setGlobal(false));
		v_server.setHandler(&handler_server);
		REQUIRE(v_server.setup(sf::Variable::getDefinition("0x1,High Speed,m/s,A,High speed velocity setting,FLOAT,FLOAT,0.1,10,0,20"), 0x200));
		// Only export when flag export has been set.
		REQUIRE_FALSE(v_server.setExport(true));
		// Set the export flag and try again.
		REQUIRE(v_server.setFlag(sf::Variable::flgExport));
		//
		REQUIRE(sf::Variable::getList().empty());
		// Server should not be exported yet.
		REQUIRE_FALSE(v_server.isExported());
		// Make the server available for referencing.
		REQUIRE(v_server.setExport(true));
		// Server should be exported now.
		REQUIRE(v_server.isExported());
		// Check if server variable is now the only  available one.
		REQUIRE(sf::Variable::getList() == sf::Variable::PtrVector{&v_server});
		//
		REQUIRE(v_server.setCur(sf::Value(1.1), true));
		// Make revert the export.
		REQUIRE(v_server.setExport(false));
		//
		REQUIRE(sf::Variable::getList().empty());
		// Server should not be exported now.
		REQUIRE_FALSE(v_server.isExported());

		// Check server instance events.
		REQUIRE(handler_server._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_server, &v_server, true, 0x0},
				{sf::Variable::veIdChanged, &v_server, &v_server, true, 0x201},
				{sf::Variable::veSetup, &v_server, &v_server, true, 0x201},
				{sf::Variable::veFlagsChange, &v_server, &v_server, true, 0x201, "AE"},
			}
		});
		// Check client instance events.
		REQUIRE(handler_client._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_client, &v_client, true, 0x0},
				{sf::Variable::veDesiredId, &v_client, &v_client, true, 0x0, "0x201"},
				{sf::Variable::veNewId, &v_server, &v_client, false, 0x201},
				{sf::Variable::veIdChanged, &v_client, &v_client, true, 0x201},
				{sf::Variable::veValueChange, &v_server, &v_client, false, 0x201, "1.1"},
				{sf::Variable::veIdChanged, &v_client, &v_client, true, 0x0},
			}
		});
	}

	SECTION("Unit Conversion")
	{
		std::istringstream is(IniContent);
		sf::UnitConversionServer ucs;
		ucs.load(is);
		ucs.setUnitSystem(sf::UnitConversionServer::usImperial);

		VarHandler handler_server;
		VarHandler handler_client;

		// Create server instance.
		sf::Variable v_server;
		v_server.setHandler(&handler_server);
		v_server.setup(sf::Variable::getDefinition("0x1,Outside Temperature,°C,A,Temperature measured outside,FLOAT,DOUBLE,0.1,18,-30,50"), 0x100);
		// Makes the server instance retrieve new conversion values.
		REQUIRE(v_server.setConvertValues(true));

		// Create local variable.
		sf::Variable v_client;
		v_client.setHandler(&handler_client);
		// Assign the global id.
		v_client.setup(0x101);

		REQUIRE(v_client.getUnit() == "°C");

		REQUIRE(v_client.setConvert(true));

		REQUIRE(v_client.getUnit() == "°F");

		REQUIRE(v_client.setCur(sf::Value(90)));

		// Check server instance events.
		REQUIRE(handler_server._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_server, &v_server, true, 0x0},
				{sf::Variable::veIdChanged, &v_server, &v_server, true, 0x101},
				{sf::Variable::veNewId, &v_server, &v_server, true, 0x101},
				{sf::Variable::veSetup, &v_server, &v_server, true, 0x101},
				{sf::Variable::veConverted, &v_server, &v_server, true, 0x101, "°C|1 °F|1"},
				{sf::Variable::veValueChange, &v_client, &v_server, false, 0x101, "32.2"},
			}
		});
		// Check client instance events.
		REQUIRE(handler_client._events == VarEvent::Vector{
			{
				{sf::Variable::veLinked, &v_client, &v_client, true, 0x0},
				{sf::Variable::veIdChanged, &v_client, &v_client, true, 0x101},
				{sf::Variable::veConverted, &v_client, &v_client, true, 0x101, "°C|1 °F|1"},
				{sf::Variable::veValueChange, &v_client, &v_client, true, 0x101, "90.0"},
			}
		});

/*
		std::clog << v_server.getName() << ' ' << v_server.getCurString() << ' ' << v_server.getUnit() << std::endl;
		std::clog << v_server.getName() << ' ' << v_client.getCurString() << ' ' << v_client.getUnit() << std::endl;
*/

/*
		std::clog << "Server: " << handler_server << std::endl;
		std::clog << "Client: " << handler_client << std::endl;
*/
	}

	SECTION("SetCur Bug")
	{
		sf::Variable var;
		var.setup(R"(0xE0008,Storage|Logfile Path,,ASPE,File path to the log file which determines the next file number.,STRING,,256,C:\DOCUME~1\bcbdebug\LOCALS~1\Temp\stogii32,,)");
		var.setCur(sf::Value(sf::unescape(R"(D:\\Data\\files)")));
	}

	sf::Variable::deinitialize();

}
