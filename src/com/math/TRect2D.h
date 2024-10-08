#pragma once

#include "TVector2D.h"
#include <iostream>
#include <limits>

namespace sf
{

/**
 * @brief Class to store and manipulate a rectangle based on floating point values.
 *
 * @code
 *               Right-Top
 *    +--------------+
 *    |              |
 *    |              |
 *    |              |
 *    +--------------+
 * Left-Bottom
 * @endcode
 */
template<typename T>
class TRect2D
{
	public:
		// Prevent non-integer and non-float types from implementing this template.
		static_assert(std::is_floating_point<T>::value, "Type T must be a floating point type.");
		/**
		 * @brief Type accessible when implemented.
		 */
		typedef T value_type;

		/**
		 * @brief Default constructor.
		 */
		TRect2D() = default;

		/**
		 * @brief Copy constructor.
		 */
		TRect2D(const TRect2D& rect);

		/**
		 * @brief Move constructor.
		 */
		TRect2D(const TRect2D&& rect);

		/**
		 * @brief Initializing constructor.
		 */
		TRect2D(T left, T bottom, T right, T top);

		/**
		 * @brief Initializer list constructor.
		 */
		TRect2D(std::initializer_list<T> list);

		/**
		 * @brief Initializing constructor using two points.
		 */
		TRect2D(const TVector2D<T>& loLeft, const TVector2D<T>& upRight);

		/**
		 * @brief Assigns new values from the passed instance.
		 */
		TRect2D& assign(const TRect2D&);

		/**
		 * @brief Assigns new coordinate values.
		 */
		TRect2D& assign(T left, T bottom, T right, T top);

		/**
		 * @brief Assigns new coordinate using two points.
		 */
		TRect2D& assign(const TVector2D<T>& loLeft, const TVector2D<T>& upRight);

		/**
		 * @brief Assigns new coordinates using position x, y, width and height values.
		 */
		TRect2D& assignWidthHeight(T left, T bottom, T w, T h);

		/**
		 * @brief Assigns new coordinates using position and size vector
		 */
		TRect2D& assignWidthHeight(const TVector2D<T>& bottom_left, const TVector2D<T>& size);

		/**
		 * @brief Empties the rectangle setting it to the initial state.
		 */
		void clear();

		/**
		 * @brief Tests if all values are near zero according the tolerance value.
		 * @see tolerance
		 * @return True when empty.
		 */
		bool isEmpty() const;

		/**
		 * @brief Compare passes rectangle with this instance taking the tolerance value in to account.
		 * @see tolerance
		 * @return True when equal.
		 */
		bool isEqual(const TRect2D<T>& other) const;

		/**
		 * @brief Call the isEqual() function to compare.
		 * @see isEqual()
		 * @return True when equal.
		 */
		bool operator==(const TRect2D<T>&) const;

		/**
		 * @brief Returns true when the passed point represented by a vector resides within this rectangle.
		 */
		bool contains(const TVector2D<T>& point) const;

		/**
		 * @brief Returns true when the passed rectangle resides within this rectangle.
		 */
		bool contains(const TRect2D<T>& other) const;

		/**
		 * @brief Returns true when the passed rectangle overlaps with this rectangle.
		 */
		bool touches(const TRect2D<T>& other) const;

		/**
		 * @brief Gets the bottom-left corner of the rectangle as a vector.
		 */
		TVector2D<T> bottomLeft() const;

		/**
		 * @brief Gets the top-right corner of the rectangle as a vector.
		 */
		TVector2D<T> topRight() const;

		/**
		 * @brief Gets the bottom-right corner of the rectangle as a vector.
		 */
		TVector2D<T> bottomRight() const;

		/**
		 * @brief Gets the top-left corner of the rectangle as a vector.
		 */
		TVector2D<T> topLeft() const;

		/**
		 * @brief Gets the width of the rectangle.
		 */
		T width() const;

		/**
		 * @brief Gets the height of the rectangle.
		 */
		T height() const;

		/**
		 * @brief Gets the height and width of the rectangle as a vector.
		 */
		TVector2D<T> Size() const;

		/**
		 * @brief Gets the area formed by the rectangles width and height.
		 */
		T area() const;

		/**
		 * @brief Gets the center of the rectangle as vector.
		 */
		TVector2D<T> center() const;

		//T Subtract(const TRect2D& other, TRect2D result[]) const;

		/**
		 * @brief Normalizes this instance so that top-right and bottom-left are correct.
		 */
		TRect2D<T>& normalize();

		/**
		 * @brief Gets a normalized instance of this instance so that top-right and bottom-left are correct.
		 */
		TRect2D<T> normalized() const;

		/**
		 * @brief Offsets this instance by the passed values x and y keeping the same size.
		 */
		TRect2D<T>& offset(T dx, T dy);

		/**
		 * @brief Gets an instance offset by the passed values x and y keeping the same size.
		 */
		TRect2D<T> offsetBy(T dx, T dy) const;

		/**
		 * @brief Gets a moved instance from this instance where left-bottom represented by the passed values x and y having the same size.
		 */
		TRect2D<T> movedTo(T x, T y);

		/**
		 * @brief Moves this instance to a different left-bottom represented by the passed values x and y having the same size.
		 */
		TRect2D<T>& moveTo(T x, T y);

		/**
		 * @brief Offsets this instance using the passed delta vector as a positive.
		 */
		TRect2D<T>& operator+=(const TVector2D<T>& delta);

		/**
		 * @brief Offsets this instance using the passed delta vector as a negative.
		 */
		TRect2D<T>& operator-=(const TVector2D<T>& delta);

		/**
		 * @brief Inflates this instance using the passed delta values for x and y.
		 */
		TRect2D<T>& inflate(T dx, T dy);

		/**
		 * @brief Inflates this instance using the passed delta vector (x, y).
		 */
		TRect2D<T>& inflate(const TVector2D<T>& delta);

		/**
		 * @brief Gets an inflated rectangle using the passed delta values for x and y.
		 */
		TRect2D inflatedBy(T dx, T dy) const;

		/**
		 * @brief Gets inflated instance using the passed delta vector (x, y).
		 */
		TRect2D inflatedBy(const TVector2D<T>& delta) const;

		/**
		 * @brief Sets this instance to the overlapping part of both rectangles.
		 */
		TRect2D& operator&=(const TRect2D& other);

		/**
		 * @brief Gets an instance of the overlapping part of both rectangles.
		 */
		TRect2D operator&(const TRect2D& other) const;

		/**
		 * @brief Sets this instance to the combined rectangle containing both rectangles.
		 */
		TRect2D& operator|=(const TRect2D& other);

		/**
		 * @brief Gets an instance of the combined rectangle containing both rectangles.
		 */
		TRect2D operator|(const TRect2D& other) const;

		/**
		 * @brief Gets the string representation of the rectangle formed like '(left, bottom, right, top)'.
		 */
		std::string toString() const;

		/**
		 * @brief Gets rectangle value from the string representation formed like '(left, bottom, right, top)'.
		 */
		TRect2D<T>& fromString(const std::string& s) noexcept(false);

		/**
		 * @brief Tolerance for when comparing in the equal operator.
		 *
		 * Used as: std::fabs(x1 - x2) < tolerance
		 */
		static constexpr auto tolerance = TVector2D<T>::tolerance;

	protected:
		/**
		 * Use the same point type as the one of the 2D vector.
		 */
		typedef TVector2D<T>::data_type::point_type point_type;
		/**
		 * @brief Rectangle structure for binary storage purposes.
 		*/
		union data_type
		{
				T array[4];
				point_type pt[2];
				struct
				{
						T left;
						T bottom;
						T right;
						T top;
				} rect;
		} _data{0.0, 0.0, 0.0, 0.0};
};

/**
 * @brief Extracts the rectangle from an input stream.
 */
template<typename T>
std::istream& operator>>(std::istream& is, TRect2D<T>&);

/**
 * @brief Writes the rectangle into a output stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const TRect2D<T>&);

}// namespace sf

#include "TRect2D.hpp"