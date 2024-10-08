#pragma once

#include "../global.h"
#include <gii/gen/UnitConversion.h>
#include <iostream>
#include <misc/gen/IniProfile.h>
#include <string>

namespace sf
{

/**
 * @brief Implementation of a unit conversion server using an ini-file.
 *
 * This conversion class installs a handler using #setUnitConversionHandler() using a #UnitConversionServerClosure type of function.
 *
 */
class _GII_CLASS UnitConversionServer
{
	public:
		/**
		 * @brief Default constructor.
		 */
		UnitConversionServer();

		/**
		 * @brief Copying this class is not possible.
		 */
		UnitConversionServer(const UnitConversionServer&) = delete;

		/**
		 * @brief Loads the conversion settings.
		 *
		 * @param is Input stream.
		 */
		virtual bool load(std::istream& is);

		/**
		 * @brief Saves the conversion settings.
		 *
		 * @param os Output stream.
		 */
		virtual bool save(std::ostream& os);

		/**
		 * @brief Gets the dirty status of the configuration.
		 *
		 * @return True when the config has been changed.
		 */
		[[nodiscard]] bool isDirty() const;

		/**
		 * @brief Writes or over writes a conversion entry in the profile.
		 */
		void setConversion(UnitConversionEvent& ev);

		/**
		 * @brief Removes a conversion entry in the profile.
		 */
		void removeConversion(const std::string& key);

		/**
		 * @brief Destructor.
		 */
		~UnitConversionServer();

		/**
		 * @brief Different unit conversion systems.
		 */
		enum EUnitSystem : int
		{
			/**
			 * No conversion use as is which should be SI-units.
			 */
			usPassThrough,
			/**
			 * Conversion in the metric system depending on precision.
			 * Radials become degrees and 'm/s' can become 'km/h'.
			 */
			usMetric,
			/**
			 * Conversion to the imperial system depending on precision.
			 * Celsius becomes Fahrenheit and 'm/s' become 'mph'.
			 */
			usImperial
		};

		/**
		 * @brief Sets the unit system to be applied.
		 *
		 * @param us unit system vale #EUnitSystem.
		 */
		virtual void setUnitSystem(int us);

		/**
		 * @brief Gets the unit system being applied.
		 */
		[[nodiscard]] EUnitSystem getUnitSystem() const;

		/**
		 * @brief Gets the internal storage profile.
		 */
		[[nodiscard]] IniProfile& getProfile();

		/**
		 * @brief Gets the section name of the passed enumerate.
		 *
		 * @param us unit system enumerate.
		 * @return Name of the system.
		 */
		static const char* getUnitSystemName(int us);

		/**
		 * @brief Type to combine a system enumerate and name.
		 */
		typedef std::pair<UnitConversionServer::EUnitSystem, const char*> UnitSystemPair;

		/**
		 * Gets all the system unit entries as a pair in a vector.
		 * @return
		 */
		static const std::vector<UnitSystemPair>& getUnitSystemNames();

	protected:
		/**
		 * @brief Handler according the unit conversion function type.
		 */
		virtual bool Handler(UnitConversionEvent& ev);

		/**
		 * Ini profile keeping all conversion information.
		 */
		IniProfile _profile;

	private:
		/**
		 * Holds the unit sys set using #setUnitSystem()
		 */
		EUnitSystem _unitSystem{usPassThrough};
};

}// namespace sf