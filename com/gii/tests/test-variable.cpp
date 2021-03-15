#include <catch2/catch.hpp>

#include <iostream>
#include <gii/gen/Variable.h>

TEST_CASE("sf::Variable", "[variable]")
{

	using Catch::Equals;

	sf::Variable::initialize();

	SECTION("Zero")
	{
		struct ClientClass
		{
			void MyHandler(sf::Variable::EEvent event, const sf::Variable& call_var, sf::Variable& link_var, bool same_inst)
			{
				std::clog << "Event: " << sf::Variable::getEventName(event) << ", ID: 0x"<< std::hex << call_var.getId() << std::endl;
				std::clog << "current: " << call_var.getCur() << std::endl;
			}
		} ClientInstance;

		sf::TVariableHandler<ClientClass> handler(&ClientInstance, &ClientClass::MyHandler);
		sf::Variable v4;
		v4.setHandler(&handler);
		v4.setDesiredId(0x2016);

		sf::Variable v1(std::string("0x2016,High Speed,m/s,High speed velocity setting,A,FLOAT,FLOAT,1,10,0,20"));
		sf::Variable v2(std::string("0x3005,Low Pass Filter setting.,,Low pass filters OFF; 2.5 MHz; 5.0 MHz; 10.0 MHz,A,INTEGER,LONG,1,0,0,3,OFF=0,2.5 MHz=1,5.0 MHz=2,10.0 MHz=3"));
		//
		std::clog << "Instance Count: " << sf::Variable::getInstanceCount() << std::endl;

		std::clog << v2;
		std::clog << v2.getStates() << std::endl;
		auto state_idx = v2.getState(sf::Value(2));
		std::clog << state_idx << " >> " << v2.getStateName(state_idx) << std::endl;

		std::clog << "Owner v1: " << v1.isOwner() << std::endl;

		sf::Variable v3(v1);
		std::clog << "Owner v3: " << v3.isOwner() << std::endl;
		v3.setCur(sf::Value(5.0));
		//v3.SetLink()


	}

	//sf::Variable::Deinitialize();
}

