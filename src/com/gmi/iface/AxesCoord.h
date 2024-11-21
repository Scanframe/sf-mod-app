#pragma once

#include "GmiTypes.h"
#include "global.h"

namespace sf::gmi
{

/**
 * @brief All axis values in one structure
 */
class _GMI_CLASS TAxesCoord
{
	public:
		/**
		 * @brief Default constructor.
		 */
		TAxesCoord();
		/**
		 * @brief copyFrom constructor.
		 * @param ac
		 */
		TAxesCoord(const TAxesCoord& ac);
		/**
		 * @brief Assignment operator.
		 * @param ac
		 * @return
		 */
		TAxesCoord& operator=(const TAxesCoord& ac);
		/**
		 * @brief Constructor using a string containing the axis values.
		 * @param s
		 */
		TAxesCoord(std::string s);
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
		TAxesCoord& operator<<(const TAxisValue& av);
		TAxesCoord& operator=(const TAxisValue& av);
		TAxesCoord& operator-=(const TAxisValue& av);
		TAxesCoord& operator+=(const TAxisValue& av);
		/**
		 * @brief Compare operators for exact binary comparison.
		 * @param ac
		 * @return
		 */
		bool operator==(const TAxesCoord& ac) const;
		bool operator!=(const TAxesCoord& ac) const;
		/**
		 * @brief Operator which add the axes not yet set in this instance.
		 * @param ac
		 * @return
		 */
		TAxesCoord& operator|=(const TAxesCoord& ac);
		/**
		 * @brief Operator which sets the axes already set in this instance.
		 * @param ac
		 * @return
		 */
		TAxesCoord& operator&=(const TAxesCoord& ac);
		/**
		 * @brief Subtracts only axis values form axes set in this instance.
		 * @param ac
		 * @return
		 */
		TAxesCoord& operator-=(const TAxesCoord& ac);
		/**
		 * @brief Adds only axis values form axes set in this instance.
		 * @param ac
		 * @return
		 */
		TAxesCoord& operator+=(const TAxesCoord& ac);
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
		ECompare Compare(const TAxesCoord& ac, const TAxesCoord& tolerance) const;
		/**
		 * @brief Same as above except the radial unlimited axes can be specified.
		 * @param ac Axes coord being compared.
		 * @param maxdiff Axes values containing accuracy of the comparison.
		 * @param radunlim Set of radial unlimited axis.
		 * @return
		 */
		ECompare Compare(const TAxesCoord& ac, const TAxesCoord& tolerance, const AxisLocations rad_unlimited) const;
		/**
		 * @brief Unsets a axis value.
		 * @param axis_loc Axis location to remove.
		 * @return This instance reference.
		 */
		TAxesCoord& operator>>(unsigned int axis_loc);
		/**
		 * @brief Assigns a specific axis value to this structure.
		 * @param axis_loc
		 * @return This instance reference.
		 */
		TAxisValue operator[](size_t axis_loc) const;
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
		TAxesCoord& SetVector(const Vector3D& v);
		/**
		 * @brief Adds an offset to the existing coordinate.
		 * @param ofs 3D vector offset.
		 * @return This instance reference.
		 */
		TAxesCoord& Offset(const Vector3D& ofs);
		/**
		 * @brief Adds an offset to the coordinate and returns the new formed coord.
		 * @param ofs
		 * @return Offset copy of this instance reference.
		 */
		TAxesCoord OffsetBy(const Vector3D& ofs) const;

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
typedef TVector<TAxesCoord> TAxesCoords;

}// namespace sf::gmi

/**
 * @brief Operator for writing the #sf::gmi::TAxesCoord to an output-stream.
 * @param os Output stream.
 * @param ac Axes coordinate  to stream.
 * @return The passed output stream.
 */
_GMI_FUNC std::ostream& operator<<(std::ostream& os, const sf::gmi::TAxesCoord& ac);

/**
 * @brief Operator for reading the #sf::gmi::TAxesCoord from an input stream.
 * @param is Input stream.
 * @param ac Axes coordinate  to stream.
 * @return The passed input stream.
 */
_GMI_FUNC std::istream& operator>>(std::istream& is, sf::gmi::TAxesCoord& ac);

namespace sf
{

/**
 * @brief Inlined operator to allow streaming of the class possible in 'sf' namespace .
 */
inline std::ostream& operator<<(std::ostream& os, const sf::gmi::TAxesCoord& ac)
{
	return ::operator<<(os, ac);
}

/**
 * @brief Inlined operator to allow streaming of the class possible in 'sf' namespace.
 */
inline std::istream& operator>>(std::istream& is, sf::gmi::TAxesCoord& ac)
{
	return ::operator>>(is, ac);
}

}// namespace sf