#pragma once

#include "GmiTypes.h"
#include "global.h"

namespace sf::gmi
{

/**
 * @brief All axis values in one structure
 */
class _GMI_CLASS AxesCoord
{
	public:
		/**
		 * @brief Default constructor.
		 */
		AxesCoord();
		/**
		 * @brief copyFrom constructor.
		 * @param ac
		 */
		AxesCoord(const AxesCoord& ac);
		/**
		 * @brief Assignment operator.
		 * @param ac
		 * @return
		 */
		AxesCoord& operator=(const AxesCoord& ac);
		/**
		 * @brief Constructor using a string containing the axis values.
		 * @param s
		 */
		AxesCoord(std::string s);
		/**
		 * @brief Clears all values in the coordinate.
		 */
		void Clear();
		/**
		 * @brief Sets an axis value.
		 * @param axis_loc
		 * @param value
		 */
		void Set(unsigned int axis_loc, double value);
		/**
		 * @brief Unsets an axis value.
		 * @param axis_loc
		 */
		void Unset(int axis_loc);
		/**
		 * @brief Returns true if value of location 'axis_loc' is available.
		 * @param axis_loc
		 * @return
		 */
		bool IsSet(int axis_loc) const;
		/**
		 * @brief Returns the value in the array.
		 * @param axis_loc Axis location.
		 * @return
		 */
		double& Value(unsigned int axis_loc);
		/**
		 * @brief Gets axis value of the passed axis.
		 * @param axis_loc
		 * @return Axis value.
		 */
		double Value(unsigned int axis_loc) const;
		/**
		 * @brief Gets the bitmap of set axes.
		 * @return Bitmap of axis.
		 */
		int GetMap() const;
		/**
		 * @brief Sets the bitmap of axes.
		 * @param map
		 */
		void SetMap(int map);
		/**
		 * @brief Returns a string with all axes information.
		 * @return
		 */
		std::string GetString() const;
		/**
		 * @brief Converts a string to the values and returns true in case of success.
		 * @param s
		 * @return
		 */
		bool SetString(std::string s);
		/**
		 * @brief Sets a axis value.
		 * @param av
		 * @return
		 */
		AxesCoord& operator<<(const AxisValue& av);
		AxesCoord& operator=(const AxisValue& av);
		AxesCoord& operator-=(const AxisValue& av);
		AxesCoord& operator+=(const AxisValue& av);
		/**
		 * @brief Compare operators for exact binary comparison.
		 * @param ac
		 * @return
		 */
		bool operator==(const AxesCoord& ac) const;
		bool operator!=(const AxesCoord& ac) const;
		/**
		 * @brief Operator which add the axes not yet set in this instance.
		 * @param ac
		 * @return
		 */
		AxesCoord& operator|=(const AxesCoord& ac);
		/**
		 * @brief Operator which sets the axes already set in this instance.
		 * @param ac
		 * @return
		 */
		AxesCoord& operator&=(const AxesCoord& ac);
		/**
		 * @brief Subtracts only axis values form axes set in this instance.
		 * @param ac
		 * @return
		 */
		AxesCoord& operator-=(const AxesCoord& ac);
		/**
		 * @brief Adds only axis values form axes set in this instance.
		 * @param ac
		 * @return
		 */
		AxesCoord& operator+=(const AxesCoord& ac);
		/**
		 * @brief Result of the compare function.
		 */
		enum ECompare : unsigned int
		{
			/**
			 * @brief Both values are exactly the same according to the max-diff values.
			 * All specified values are equal to those specified by the other.
			 */
			crEQUAL = 0,
			/**
			 * @brief This instance specified values are the same as the other instance values. Only for the by this instance specified values.
			 */
			crTHIS_EQUAL = 1,
			/**
			 * @brief Other instance specified values are the same as these instance values. Only for the by the other instance specified values.
			 */
			crOTHER_EQUAL = 2,
			/**
			 * @brief One of the values specified in both instances were not equal.
			 */
			crNOT_EQUAL = 3
		};
		/**
		 * @brief Compares two coordinates using a second coordinate containing max differences.
		 * Tests only the values in the coordinate that exists in both coordinates.
		 * For radial unlimited axes the default axis movements are used.
		 * @param ac Axes coord being compared.
		 * @param tolerance Axes values containing accuracy of the comparison.
		 * @return True when the two values do not exceed the maximum difference.
		 */
		ECompare Compare(const AxesCoord& ac, const AxesCoord& tolerance) const;
		/**
		 * @brief Same as above except the radial unlimited axes can be specified.
		 * @param ac Axes coord being compared.
		 * @param maxdiff Axes values containing accuracy of the comparison.
		 * @param radunlim Set of radial unlimited axis.
		 * @return
		 */
		ECompare Compare(const AxesCoord& ac, const AxesCoord& tolerance, const AxisLocations rad_unlimited) const;
		/**
		 * @brief Unsets a axis value.
		 * @param axis_loc Axis location to remove.
		 * @return This instance reference.
		 */
		AxesCoord& operator>>(unsigned int axis_loc);
		/**
		 * @brief Assigns a specific axis value to this structure.
		 * @param axis_loc
		 * @return This instance reference.
		 */
		AxisValue operator[](size_t axis_loc) const;
		/**
		 * @brief Gets the 3D vector formed by the XYZ axis.
		 * @return The 3D -vector.
		 */
		Vector3D GetVector() const;
		/**
		 * @brief Sets the 3D vector formed by the XYZ axis.
		 * @param v 3D vector.
		 * @return This instance as reference.
		 */
		AxesCoord& SetVector(const Vector3D& v);
		/**
		 * @brief Adds an offset to the existing coordinate.
		 * @param ofs 3D vector offset.
		 * @return This instance reference.
		 */
		AxesCoord& Offset(const Vector3D& ofs);
		/**
		 * @brief Adds an offset to the coordinate and returns the new formed coord.
		 * @param ofs
		 * @return Offset copy of this instance reference.
		 */
		AxesCoord OffsetBy(const Vector3D& ofs) const;

	private:
		/**
		 * @brief Integer used as map for valid entries.
		 */
		TSet<int> FMap;
		/**
		 * @brief Holds an array of all possible position entries.
		 */
		double FData[alLAST_ENTRY];
};

/**
 * @brief Type definition.
 */
typedef TVector<AxesCoord> TAxesCoords;

}// namespace sf::gmi

/**
 * @brief Operator for writing the #sf::gmi::TAxesCoord to an output-stream.
 * @param os Output stream.
 * @param ac Axes coordinate  to stream.
 * @return The passed output stream.
 */
_GMI_FUNC std::ostream& operator<<(std::ostream& os, const sf::gmi::AxesCoord& ac);

/**
 * @brief Operator for reading the #sf::gmi::TAxesCoord from an input stream.
 * @param is Input stream.
 * @param ac Axes coordinate  to stream.
 * @return The passed input stream.
 */
_GMI_FUNC std::istream& operator>>(std::istream& is, sf::gmi::AxesCoord& ac);

namespace sf
{

/**
 * @brief Inlined operator to allow streaming of the class possible in 'sf' namespace .
 */
inline std::ostream& operator<<(std::ostream& os, const sf::gmi::AxesCoord& ac)
{
	return ::operator<<(os, ac);
}

/**
 * @brief Inlined operator to allow streaming of the class possible in 'sf' namespace.
 */
inline std::istream& operator>>(std::istream& is, sf::gmi::AxesCoord& ac)
{
	return ::operator>>(is, ac);
}

}// namespace sf