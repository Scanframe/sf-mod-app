#pragma once

#include <misc/gen/TVector.h>
#include <misc/gen/gen_utils.h>

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
		 * @brief Type used for the identifying integer.
		 */
		typedef uint64_t id_type;
		/**
		 * @brief Type used for the flag integer.
		 */
		typedef int32_t flags_type;
		/**
		 * @brief Type used for size of vectors.
		 */
		typedef size_t size_type;
		/**
		 * @brief Vector for lists of information ID's
		 */
		typedef TVector<id_type> IdVector;
		/**
		 * @brief Vector for containing different information base derived classes.
		 */
		typedef TVector<InformationBase*> Vector;
};

}