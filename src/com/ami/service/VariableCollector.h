#pragma once

#include <gii/gen/Variable.h>
#include <map>
#include <misc/gen/Sync.h>
#include <misc/gen/TFifoClass.h>

namespace sf
{

class VariableCollector : public VariableHandler
	, protected Sync
{
	public:
		explicit VariableCollector();

		~VariableCollector() override;

		void variableEventHandler(VariableTypes::EEvent event, const Variable& caller, Variable& link, bool same_inst) override;

	private:
		// Adds a variable to the map and links it.
		void addVariable(const Variable& v);
		// Holds the variable for listening to global events.
		Variable _variable{};
		// Maps a variable id to the variable instance pointer.
		std::map<id_type, Variable*> _map{};

		struct Data
		{
				enum
				{
					dtNone,
					dtAdd,
					dtRemove,
					dtValue,
					dtFlags
				} _type{dtNone};
				id_type _id{0};
				Value _value{};
		};
		//
		TFifoClass<Data> _fifo{1000};
};

}// namespace sf
