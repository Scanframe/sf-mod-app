#pragma once

#include <limits>
#include <string>

namespace sf
{

// Forward declaration.
template<typename T>
class TMatrix22;

// Forward declaration.
template<typename T>
class TRectangle2D;

/**
 * @brief 2-dimensional vector for math operations.
 */
template<typename T>
class TVector2D
{
	public:
		/**
		 * @brief Type declaration of the coordinate storage values.
		 */
		typedef T value_type;

		/**
		 * @brief Default constructor.
		 */
		TVector2D() = default;

		/**
		 * @brief Copy constructor.
		 */
		TVector2D(const TVector2D<T>& v);

		/**
		 * @brief Move constructor.
		 */
		TVector2D(TVector2D<T>&&) noexcept = default;

		/**
		 * @brief Initializing constructor.
		 * @param xp X-axis value.
		 * @param yp Y-axis value.
		 */
		TVector2D(T xp, T yp);

		/**
		 * @brief Assignment of new coordinate values.
		 * @param x X-axis value.
		 * @param y Y-axis value.
		 * @return Itself.
		 */
		TVector2D<T>& assign(T xp, T yp);

		/**
		 * @brief Assignment of vector instance.
		 * @param x X-axis value.
		 * @param y Y-axis value.
		 * @return Itself.
		 */
		TVector2D<T>& assign(const TVector2D<T>& v);

		/**
		 * @brief Assignment operator.
		 */
		TVector2D<T>& operator=(TVector2D<T>& v) noexcept;

		/**
		 * @brief Assignment move operator.
		 */
		TVector2D<T>& operator=(TVector2D<T>&& v) noexcept;

		/**
		 * @brief Matrix transform operator.
		 */
		TVector2D<T>& operator*=(const TMatrix22<T>& mtx);

		/**
		 * @brief Inversion math operator inverting the vector.
		 */
		TVector2D<T> operator-() const;

		/**
		 * @brief Compound addition operator.
		 */
		TVector2D<T>& operator+=(const TVector2D<T>&);

		/**
		 * @brief Compound subtraction operator.
		 */
		TVector2D<T>& operator-=(const TVector2D<T>&);

		/**
		 * @brief Compound multiplier operator.
		 */
		TVector2D<T>& operator*=(T c);

		/**
		 * @brief Compound division operator.
		 */
		TVector2D<T>& operator/=(T);

		/**
		 * @brief Addition operator.
		 */
		TVector2D<T> operator+(const TVector2D<T>&) const;

		/**
		 * @brief Subtraction operator.
		 */
		TVector2D<T> operator-(const TVector2D<T>&) const;

		/**
		 * @brief Division operator.
		 */
		TVector2D<T> operator/(T) const;

		/**
		 * @brief Compares the passed vector within the set tolerance.
		 * @param v Vector to compare with.
		 * @param tol The tolerance when comparing which has a default.
		 * @return True when equal.
		 */
		bool isEqual(const TVector2D& v, T tol = tolerance) const;

		/**
		 * @brief Equality operator.
		 */
		bool operator==(const TVector2D<T>&) const;

		/**
		 * @brief Inequality operator.
		 */
		bool operator!=(const TVector2D<T>&) const;

		/**
		 * @brief Access like an array.
		 * @throw std::out_of_range
		 */
		T& operator[](size_t);

		/**
		 * @brief Access like a const array.
		 * @throw std::out_of_range
		 */
		const T& operator[](size_t) const;

		/**
		 * @brief Cast operator for pointer.
		 */
		constexpr operator T*();

		/**
		 * @brief Cast operator for const pointer.
		 */
		operator const T*() const;

		/**
		 * @brief Gets the x-coordinate value.
		 */
		constexpr T x() const;

		/**
		 * @brief Gets the x-coordinate value as reference.
		 */
		constexpr T& x();

		/**
		 * @brief Gets the y-coordinate value.
		 */
		constexpr T y() const;

		/**
		 * @brief Gets the y-coordinate value as reference.
		 */
		constexpr T& y();

		/**
		 * @brief Gets the length or magnitude of the vector.
		 *
		 * Note that this has to return a double because it uses the sqrt function.
		 * @return Length of the vector.
		 */
		T length() const;

		/**
		 * @brief Gets the squared length or magnitude of the vector.
		 * @return Squared length.
		 */
		T lengthSqr() const;

		/**
		 * @brief Normalizes the vector also called a unit-vector, set to length 1.
		 * @return This normalized vector.
		 */
		TVector2D<T>& normalize();

		/**
		 * @brief Gets a normalized vector also called a unit-vector, made of length 1.
		 * @return A normalized vector.
		 */
		TVector2D<T> normalized() const;

		/**
		 * @brief Scales the vector by multiplying all axis with the passed factor.
		 * @return This scaled vector.
		 */
		TVector2D<T>& scale(T factor);

		/**
		 * @brief Scales the vector by multiplying all axis with the passed factor.
		 * @return A scaled vector.
		 */
		TVector2D<T> scaled(T factor) const;

		/**
		 * @brief Gets the cross (out) product of 2 vectors.
		 * @return Cross product of 2 vectors.
		 */
		T crossProduct(const TVector2D<T>&) const;

		/**
		 * @brief Gets the dot (in) product of 2 vectors.
		 * @return Dot (in) product of 2 vectors.
		 */
		T dotProduct(const TVector2D<T>&) const;

		/**
		 * @brief Gets the cross (out) product of 2 vectors.
		 * @return Cross product of 2 vectors.
		 */
		T operator*(const TVector2D<T>& v) const;

		/**
		 * @brief Gets the angle between the two vectors.
		 * @return angle between the two vectors.
		 */
		T angle(const TVector2D<T>&) const;

		/**
		 * @brief Gets the angle of vector in xy-plane.
		 * @return angle of vector in xy-plane.
		 */
		T angle() const;

		/**
		 * @brief Returns a normalized positive angle of function @Angle().
		 * @return normalized positive angle.
		 */
		T angleNormalized() const;

		/**
		 * @brief Gets the slope (coefficient) of the line formed with the origin (0, 0).
		 * When it is an infinite value std::numeric_limits<T>::max() is returned.
		 * @return slope with the origin.
		 */
		T slope() const;

		/**
		 * @brief Gets the distance between this and the passed vector.
		 * Note that this has to return a double because it uses the sqrt() function.
		 * @return distance to another vector.
		 */
		T distance(const TVector2D<T>&) const;

		/**
		 * @brief Gets the squared distance between 2 given points.
		 * Avoids taking an expensive sqrt call.
		 * Useful when comparing different distances.
		 * @return squared distance between 2 given points
		 */
		T distanceSqr(const TVector2D<T>&) const;

		/**
		 * @brief Copy only those values of x or y which are smaller.
		 * @param vertex
		 */
		void updateMin(const TVector2D<T>& vertex);

		/**
		 * @brief Copy only those values of x or y which are larger.
		 * @param vertex
		 */
		void updateMax(const TVector2D<T>& vertex);

		/**
		 * @brief Gets the string representation of the 2D vector formed like '(1.23,4.56)'.
		 */
		std::string toString() const;

		/**
		 * @brief Gets the vector value from the string representation formed like '(1.23,4.56)'.
		 * Throws an exception when the string is not in the correct format.
		 * @throw std::invalid_argument
		 */
		TVector2D<T>& fromString(const std::string& s) noexcept(false);

		/**
		 * @brief Tolerance for when comparing in the equal operator.
		 * Empirical chosen epsilon multiplier to make it work.
		 * Used in: #sf::isEqual(f1, f2, tolerance)
		 */
		static constexpr auto tolerance = std::numeric_limits<T>::epsilon() * 10.0;

		/**
		 * @brief Checks where 2 points lie in relation to a given line given by 2 points.
		 * @param lp1 Point 1 which forms the line.
		 * @param lp2 Point 2 which forms the line.
		 * @param p1 Free point 1.
		 * @param p2 Free point 2.
		 * @return >0: Same side, <0: Opposite sides, 0 : A point on the line
		 */
		static int areOnSameSide(
			const TVector2D<T>& lp1,
			const TVector2D<T>& lp2,
			const TVector2D<T>& p1,
			const TVector2D<T>& p2
		);

	protected:
		/**
		 * @brief Storage union of the 2D coordinate making the x,y accessible as an array.
		 */
		union data_type
		{
				T array[2];
				struct point_type
				{
						T x;
						T y;
				} coord;
		} _data{0, 0};

		friend class TRectangle2D<T>;
};

template<typename T>
TVector2D<T> operator*(T c, const TVector2D<T>& v)
{
	return {v.x() * c, v.y() * c};
}

template<typename T>
TVector2D<T> operator*(const TVector2D<T>& v, T c)
{
	return {v.x() * c, v.y() * c};
}

/**
 * @brief Operator for reading a vector from a stream.
 */
template<typename T>
std::istream& operator>>(std::istream& is, TVector2D<T>& v) noexcept(false)
{
	std::string s;
	auto delimiter = ')';
	std::getline(is, s, delimiter);
	v.fromString(s.append(1, delimiter));
	return is;
}

/**
 * @brief Operator for writing a vector to a stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const TVector2D<T>& v)
{
	return os << v.toString();
}

}// namespace sf

#include "TVector2D.hpp"
