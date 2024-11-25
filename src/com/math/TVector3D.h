#pragma once
#include <limits>
#include <string>

namespace sf
{

// Forward declaration.
template<typename T>
class TMatrix44;

/**
 * @brief 3-dimensional vector for math operations.
 */
template<typename T>
class TVector3D
{
	public:
		/**
		 * @brief Type declaration of the coordinate storage values.
		 */
		typedef T value_type;

		/**
		 * @brief Default constructor.
		 */
		TVector3D() = default;

		/**
		 * @brief Copy constructor.
		 */
		TVector3D(const TVector3D<T>& v);

		/**
		 * @brief Move constructor.
		 */
		TVector3D(TVector3D<T>&&) noexcept = default;

		/**
		 * @brief Initializing constructor.
		 * @param xp X-axis value.
		 * @param yp Y-axis value.
		 * @param zp Z-axis value.
		 */
		TVector3D(T xp, T yp, T zp);

		/**
		 * @brief Assignment of new coordinate values.
		 * @param xp X-axis value.
		 * @param yp Y-axis value.
		 * @param xp Y-axis value.
		 * @return Itself.
		 */
		TVector3D<T>& assign(T xp, T yp, T zp);

		/**
		 * @brief Assignment of vector instance.
		 * @param x X-axis value.
		 * @param y Y-axis value.
		 * @param z Z-axis value.
		 * @return Itself.
		 */
		TVector3D<T>& assign(const TVector3D<T>& v);

		/**
		 * @brief Assignment operator.
		 */
		TVector3D<T>& operator=(TVector3D<T>& v) noexcept;

		/**
		 * @brief Assignment move operator.
		 */
		TVector3D<T>& operator=(TVector3D<T>&& v) noexcept;

		/**
		 * @brief Matrix transform assignment operator.
		 */
		TVector3D<T>& operator*=(const TMatrix44<T>& mtx);

		/**
		 * @brief Matrix transform operator.
		 */
		TVector3D<T> operator*(const TMatrix44<T>& mtx);

		/**
		 * @brief Inversion math operator inverting the vector.
		 */
		TVector3D<T> operator-() const;

		/**
		 * @brief Compound addition operator.
		 */
		TVector3D<T>& operator+=(const TVector3D<T>&);

		/**
		 * @brief Compound subtraction operator.
		 */
		TVector3D<T>& operator-=(const TVector3D<T>&);

		/**
		 * @brief Compound multiplier operator.
		 */
		TVector3D<T>& operator*=(T c);

		/**
		 * @brief Compound division operator.
		 */
		TVector3D<T>& operator/=(T);

		/**
		 * @brief Addition operator.
		 */
		TVector3D<T> operator+(const TVector3D<T>&) const;

		/**
		 * @brief Subtraction operator.
		 */
		TVector3D<T> operator-(const TVector3D<T>&) const;

		/**
		 * @brief Division operator.
		 */
		TVector3D<T> operator/(T c) const;

		/**
		 * @brief Compares the passed vector within the set tolerance.
		 * @param v Vector to compare with.
		 * @param tol The tolerance when comparing which has a default.
		 * @return True when equal.
		 */
		bool isEqual(const TVector3D& v, T tol = tolerance) const;

		/**
		 * @brief Equality operator.
		 */
		bool operator==(const TVector3D<T>&) const;

		/**
		 * @brief Inequality operator.
		 */
		bool operator!=(const TVector3D<T>&) const;

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
		 * @brief Gets the z-coordinate value.
		 */
		constexpr T z() const;

		/**
		 * @brief Gets the z-coordinate value as reference.
		 */
		constexpr T& z();

		/**
		 * @brief Gets the length or magnitude of the vector.
		 *
		 * Note that this has to return a double because it uses the sqrt function.
		 * @return Length of the vector.
		 */
		T length() const;

		/**
		 * @brief Gets the length or magnitude of the vector for only X and Y axis.
		 *
		 * Note that this has to return a double because it uses the sqrt function.
		 * @return Length of the vector.
		 */
		T length2D() const;

		/**
		 * @brief Gets the squared length or magnitude of the vector.
		 * @return Squared length.
		 */
		T lengthSqr() const;

		/**
		 * @brief Gets the squared length or magnitude of the vector for only the X and Y axis.
		 * @return Squared length.
		 */
		T lengthSqr2D() const;

		/**
		 * @brief Normalizes the vector also called a unit-vector, set to length 1.
		 * @return normalized vector.
		 */
		TVector3D<T>& normalize();

		/**
		 * @brief Gets a normalized vector also called a unit-vector, made of length 1.
		 * @return normalized vector.
		 */
		TVector3D<T> normalized() const;

		/**
		 * @brief Scales the vector by multiplying all axis with the passed factor.
		 * @return This scaled vector.
		 */
		TVector3D<T>& scale(T factor);

		/**
		 * @brief Scales the vector by multiplying all axis with the passed factor.
		 * @return A scaled vector.
		 */
		TVector3D<T> scaled(T factor) const;

		/**
		 * @brief Gets the cross (out) product of 2 vectors.
		 * @return Cross product of 2 vectors.
		 */
		TVector3D<T> crossProduct(const TVector3D<T>&) const;

		/**
		 * @brief Gets the dot (in) product of 2 vectors.
		 * @return Dot (in) product of 2 vectors.
		 */
		T dotProduct(const TVector3D<T>&) const;

		/**
		 * @brief Gets the dot (in) product of 2 vectors for only the X and Y axis.
		 * @return Dot (in) product of 2 vectors.
		 */
		T dotProduct2D(const TVector3D<T>&) const;

		/**
		 * @brief Gets the cross (out) product of 2 vectors.
		 * @return Cross product of 2 vectors.
		 */
		TVector3D<T> operator*(const TVector3D<T>& v) const;

		/**
		 * @brief Gets the angle between the two vectors.
		 * @return angle between the two vectors.
		 */
		T angle(const TVector3D<T>&) const;

		/**
		 * @brief Returns a normalized positive angle of function @Angle().
		 * @return normalized positive angle.
		 */
		T angleNormalized() const;

		/**
		 * @brief Gets the distance between this and the passed vector.
		 * Note that this has to return a double because it uses the sqrt() function.
		 * @return distance to another vector.
		 */
		T distance(const TVector3D<T>&) const;

		/**
		 * @brief Gets the distance between this and the passed vector for only the X and Y axis.
		 * Note that this has to return a double because it uses the sqrt() function.
		 * @return distance to another vector.
		 */
		T distance2D(const TVector3D<T>&) const;

		/**
		 * @brief Gets the squared distance between 2 given points.
		 * Avoids taking an expensive sqrt call.
		 * Useful when comparing different distances.
		 * @return squared distance between 2 given points
		 */
		T distanceSqr(const TVector3D<T>&) const;

		/**
		 * @brief Gets the squared distance between 2 given points in 2D only (z is ignored).
		 * Avoids taking an expensive sqrt call.
		 * Useful when comparing different distances.
		 * @return squared distance between 2 given points
		 */
		T distanceSqr2D(const TVector3D<T>&) const;

		/**
		 * @brief Copy only those values of x,y or z which are smaller.
		 * @param vertex
		 */
		void updateMin(const TVector3D<T>& vertex);

		/**
		 * @brief Copy only those values of x,y or z which are larger.
		 * @param vertex
		 */
		void updateMax(const TVector3D<T>& vertex);

		/**
		 * @brief Gets the string representation of the 2D vector formed like '(1.23,4.56)'.
		 */
		std::string toString() const;

		/**
		 * @brief Gets the vector value from the string representation formed like '(1.23,4.56,7.89)'.
		 * Throws an exception when the string is not in the correct format.
		 * @throw std::invalid_argument
		 */
		TVector3D<T>& fromString(const std::string& s) noexcept(false);

		/**
		 * @brief Gets the dominant axis where x=0, y=1 and z=2.
		 * @return Integer value representing the dominant axis.
		 */
		int dominantAxis();

		/**
		 * @brief Tolerance for when comparing in the equal operator.
		 * Empirical chosen epsilon multiplier to make it work after calculations.
		 * Used as: std::fabs(x1 - x2) < tolerance
		 */
		static constexpr auto tolerance = std::numeric_limits<T>::epsilon() * T(15.0);

		/**
		 * @brief Copies the X, Y and Z values to an array of type 'T' or type 'TVector3D<T>::value_type'.
		 */
		void copyTo(T fa[3]) const;

		/**
		 * @brief Gets an array pointer of floating point type F of the X, Y and Z axis.
		 * Used to pass to OpenGL where a specific floating point value  is required.
		 * This function is thread safe by returning a thread local static variable.
		 * @return A pointer of the floating point values converted to a float type.
		 */
		template<typename F = float>
		const F* floatPtr() const;

	protected:
		/**
		 * @brief Storage union of the 3D coordinate making the x,y,z accessible as an array.
		 */
		union data_type
		{
				T array[3];
				struct point_type
				{
						T x;
						T y;
						T z;
				} coord;
		} _data{0, 0, 0};
};

template<typename T>
TVector3D<T> operator*(T c, const TVector3D<T>& v)
{
	return {v.x() * c, v.y() * c, v.z() * c};
}

template<typename T>
TVector3D<T> operator*(const TVector3D<T>& v, T c)
{
	return {v.x() * c, v.y() * c, v.z() * c};
}

/**
 * @brief Operator for reading a vector from a stream.
 */
template<typename T>
std::istream& operator>>(std::istream& is, TVector3D<T>& v) noexcept(false)
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
std::ostream& operator<<(std::ostream& os, const TVector3D<T>& v)
{
	return os << v.toString();
}

}// namespace sf

#include <math/TVector3D.hpp>
