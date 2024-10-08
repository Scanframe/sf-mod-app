#pragma once

#include <misc/gen/TVector.h>

#include "../global.h"

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
};

}// namespace sf