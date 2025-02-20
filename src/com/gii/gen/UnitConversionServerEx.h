#pragma once
#include <gii/gen/UnitConversionServer.h>
#include <gii/gen/Variable.h>
#include <gii/global.h>
#include <memory>

namespace sf
{

/**
 * @brief Extended #UnitConversionServer with master and slaved variables.
 */
class _GII_CLASS UnitConversionServerEx final
	: public UnitConversionServer
	, VariableHandler
{
	public:
		/**
		 * @brief Default constructor.
		 */
		UnitConversionServerEx();

		/**
		 * @brief Destructor.
		 */
		~UnitConversionServerEx() override;

		/**
		 * @brief  Flushes all conversion lists from this container.
		 */
		void flush();

		/**
		 * @brief Overridden from base class to intercept a change.
		 */
		void setUnitSystem(int us) override;

		/**
		 * @brief Load the conversion configuration.
		 *
		 * @param is Input stream.
		 * @return True on success.
		 */
		bool load(std::istream& is) override;

		/**
		 * @brief Load the conversion configuration.
		 *
		 * @param os Output stream.
		 * @return True on success.
		 */
		bool save(std::ostream& os) override;

		/**
		 * @brief Enables or disables the conversion on Variables that are following the master.
		 *
		 * This is normally controlled by the Enable variable when linked.
		 *
		 * @param enable True to enable False to disable.
		 */
		void setEnable(bool enable);

		/**
		 * @brief Gets the enabled variable ID.
		 */
		[[nodiscard]] id_type getEnableId() const;

		/**
		 * @brief Sets the enabled variable ID.
		 */
		void setEnableId(id_type);

		/**
		 * @brief Converts this variable when a slave using the masters value.
		 *
		 * @param var Variable ofg which the owner is taken.
		 * @param regular When True the variable is not checked against the mapped entries.
		 */
		void convertVariable(const Variable& var, bool regular = false);

		/**
		 * @brief Generates a lookup table and resets the existing conversion values.
		 */
		void reinitialize();

		/**
		 * @brief Assigns a handler for when a conversion is not provided.
		 * Calling this method with default arguments disables the handler.
		 */
		void setAskHandler(const UnitConversionServerClosure& closure = UnitConversionServerClosure());

		/**
		 * @brief Gets the name of the followers section.
		 */
		[[nodiscard]] static const char* getFollowersSectionName();

	protected:
		/**
		 * @brief Overridden from base class to intercept conversion failure.
		 */
		bool handler(UnitConversionEvent& ev) override;

	private:
		/**
		 * @brief Handler which is registered to handle the global unit conversion calls.
		 */
		void variableEventHandler(EEvent event, const Variable& call_var, Variable& link_var, bool same_inst) override;

		/**
		 * @brief Read or write the conversion configuration to ini-profile.
		 *
		 * To make changes happen for existing conversions call #reinitialize().
		 *
		 * @param save True for saving and False for reading.
		 */
		void configReadWrite(bool save);

		/**
		 * @brief Entry binding a slave variables to master variable.
		 */
		class Entry final : VariableHandler
		{
			public:
				/**
				 * @brief  Constructor.
				 * @param owner
				 */
				explicit Entry(UnitConversionServerEx* owner);

				/**
				 * @brief Holds the owner of this instance.
				 */
				UnitConversionServerEx* _owner;
				/**
				 * @brief Holds dependable variable for this list.
				 */
				Variable _vMaster;
				/**
				 * @brief Integer lists for holding the ID's needed for conversion.
				 */
				std::vector<Variable::id_type> _ids;
				/**
				 * @brief Unit after conversion.
				 */
				std::string _unit;
				/**
				 * @brief Script used to in the calculation.
				 */
				std::string _script;

				/**
				 * @brief Sets the conversion values on all existing ID's.
				 */
				void updateConversion();

				/**
				 * @brief Converts a variable dependency information
				 * @param var The variable instance.
				 * @return False when unable to do so.
				 */
				bool convertVariable(const Variable& var) const;

			private:
				/**
				 * @brief Implement abstract method from base class.
				 */
				void variableEventHandler(EEvent event, const Variable& call_var, Variable& link_var, bool same_inst) override;
		};

		/**
		 * @brief Holds the handler which opens a modal dialog to add a conversion to the conversion configuration.
		 */
		UnitConversionServerClosure _askHandler;
		/**
		 * @brief Holds the enable flag for special conversion of followers only.
		 */
		bool _enabled{false};
		/**
		 * @brief Variable for receiving NewId events and to disable or enable when attached.
		 */
		Variable _vEnable;
		/**
		 * @brief Holds the all entries.
		 */
		std::vector<std::unique_ptr<Entry>> _entries;
		/**
		 * @brief Lookup table for followers to entries in the list.
		 */
		std::map<Variable::id_type, Entry*> _map;
};

}// namespace sf
