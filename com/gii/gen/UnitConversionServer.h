#pragma once

#include <string>
#include <iostream>
#include <misc/gen/IniProfile.h>
#include <gii/gen/UnitConversion.h>
#include "../global.h"


namespace sf
{

/**
 * @brief Implementation of a unit conversion server using an ini-file.
 *
 * This conversion class installs a handler using #setUnitConversionHandler() using a #UnitConversionServerCallBack type of function.
 *
 */
class _GII_CLASS UnitConversionServer
{
	public:

		/**
		 * @brief Constructor.
		 * @param ini_filepath Path to ini file.
		 */
		explicit UnitConversionServer(const std::string& ini_filepath);

		/**
		 * @brief Constructor using an input stream.
		 *
		 * @param is Input stream.
		 */
		explicit UnitConversionServer(std::istream& is);

		/**
		 * @brief Destructor.
		 */
		~UnitConversionServer();

		/**
		 * @brief Copying this class is not possible.
		 */
		UnitConversionServer(const UnitConversionServer&) = delete;

		/**
		 * Different conversion modes.
		 */
		enum EUnitSystem
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
		 * Sets the unit system to be applied.
		 * @param us Unit system.
		 */
		void setUnitSystem(EUnitSystem us);

	private:
		/**
		 * Handler according
		 */
		static bool Handler
			(
				void* data,
				const std::string& option,
				const std::string& from_unit,
				int from_precision,
				double& multiplier,
				double& offset,
				std::string& to_unit,
				int& to_precision
			);

		/**
		 * Ini profile keeping all conversion information.
		 */
		IniProfile _profile;
		/**
		 * Holds the unit sys set using #setUnitSystem
		 */
		EUnitSystem _unitSystem{usPassThrough};

		/**
		 * Returns the section name.
		 * @param us
		 * @return
		 */
		static std::string getUnitSystemName(EUnitSystem us);

};

}