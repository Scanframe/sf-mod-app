#pragma once
#include <gii/global.h>
#include <misc/gen/TVector.h>

namespace sf
{

/**
 * @brief Base class for all generic information objects to be able to put them in a typed list together.
*/
class _GII_CLASS InformationBase
{
	public:
		/**
		 * @brief Virtual destructor so derived classes can be destroyed by a pointer of this type.
		 */
		virtual ~InformationBase() = default;
};

/**
 * @brief Base class for all generic information objects to be able to put them in a typed list together.
 */
class _GII_CLASS InformationTypes
{
	public:
		/**
		 * @brief Type used for the identifying integer (64-bits).
		 */
		typedef unsigned long long id_type;
		/**
		 * @brief Type used for the flag integer.
		 */
		typedef int32_t flags_type;
		/**
		 * @brief Type used for size of vectors.
		 */
		typedef size_t size_type;
		/**
		 * @brief Type used for containing a single data element which is the largest integer.
		 */
		typedef uint64_t data_type;
		/**
		 * @brief Type used for containing a single data element which is the largest signed integer.
		 */
		typedef int64_t sdata_type;
		/**
		 * @brief Vector for lists of information ID's
		 */
		typedef TVector<id_type> IdVector;
		/**
		 * @brief Vector for containing different information base derived classes.
		 */
		typedef TVector<InformationBase*> Vector;
		/**
		 * @brief Unsigned size value indicating not found or no index.
		 */
		static constexpr size_t npos = std::numeric_limits<size_type>::max();

		/**
		 * @brief Casts any type to a #data_type value.
		 * @tparam T Type to be converted but equal or less than the size of #data_type.
		 * @param value Data type value.
		 * @return Cast value.
		 */
		template<typename T>
		constexpr std::enable_if_t<(sizeof(T) <= sizeof(data_type)), data_type> toDataType(T value) const
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return reinterpret_cast<data_type>(value);// Safe for pointers
			}
			else
			{
				return static_cast<data_type>(value);// Safe for integral and smaller types
			}
		}

		/**
		 * @brief Casts a #data_type value to a given type.
		 * @tparam T Type to be converted but equal or less than the size of #data_type.
		 * @param value
		 * @return Cast value.
		 */
		template<typename T>
		constexpr std::enable_if_t<(sizeof(T) <= sizeof(data_type)), T> fromDataType(data_type value) const
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return reinterpret_cast<T>(value);
			}
			else
			{
				return static_cast<T>(value);
			}
		}
};

}// namespace sf
